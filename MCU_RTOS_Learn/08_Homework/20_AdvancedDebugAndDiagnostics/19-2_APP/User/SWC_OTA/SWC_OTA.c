/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file Ota_task.c
 * 
 * @par dependencies 
 * - Ota_task.h
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Functions related to reading and writing in the chip's flash area.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-09-13
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "SWC_OTA.h"
#include "ymodem.h"

#include "FreeRTOS.h"

#include "gpio.h"
#include "semphr.h"

#include "w25qxx_Handler.h"

/* Private typedef -----------------------------------------------------------*/
osThreadId_t OTA_taskHandle;
const osThreadAttr_t OTA_task_attributes = {
  .name = "OTA_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

osThreadId_t DownloadAppData_TaskHandle;
const osThreadAttr_t DownloadAppData_task_attributes = {
  .name = "DownloadAppData",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t g_au8_YmodemRec_A[1030];
uint8_t g_au8_YmodemRec_B[1030];

/* OTA 执行的管理状态机 */
static OTA_State_t g_Ota_State = eOTA_WAIT_REQ_DOWNLOAD ;

static uint8_t g_au8_ota_cmd[4] = {0} ;

extern QueueHandle_t Q_YmodemReclength;

extern int32_t packet_length ;

uint32_t g_u32_datalength = 0;


/* 这个队列的主要作用 */
QueueHandle_t Queue_AppDataBuffer;

SemaphoreHandle_t Semphore_ExtFlashState ;

// extern uint16_t s_u16_YmodRecLength;

/* Private function prototypes -----------------------------------------------*/
static int8_t KeyScan(void);
void SoftReset();

void DownloadAppData_task_runnable(void *argument);


/* Private functions ---------------------------------------------------------*/

/* extern variables ---------------------------------------------------------*/

/**
 * @brief   OTA 线程任务，执行 OTA 的主要相关逻辑
 * 
 * @param [in]  argument 
 * 
 * @details
 * @note
 * @warning
 */
void ota_task_runnable(void *argument)
{
  /* USER CODE BEGIN key_task_runnable */
    uint8_t req_updateAckCmd[3]= {0x44, 0x55, 0x66};
    uint8_t t_u8_rec_length = 0;

    int32_t app_data_length = 0 ;
        /* 清除 EEProm 的状态*/
        uint8_t EEP_Flag_ota_state = 0 ;
        ee_WriteBytes(&EEP_Flag_ota_state, 0x00 , 1);
  /* Infinite loop */
    for(;;)
    {
        // Ymodem_Receive(g_au8_YmodemRec);
        switch (g_Ota_State)
        {
            case eOTA_WAIT_REQ_DOWNLOAD:
                /* 阻塞式接收串口命令*/

                if( HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_au8_ota_cmd, 4) )
                {
                    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
                }
                /* 等待数据接收完成*/
                xQueueReceive(Q_YmodemReclength, &t_u8_rec_length, portMAX_DELAY);
                /* 校验数据长度*/
                if( 3 == t_u8_rec_length )
                {
                    /* 校验数据内容 */
                    if( (0x11 == g_au8_ota_cmd[0]) && (0x22 == g_au8_ota_cmd[1])&& (0x33 == g_au8_ota_cmd[2]) )
                    {
                        /* 切换状态 */
                        g_Ota_State = eOTA_DOWNLOAD ;
                        // TODO：创建线程以及相关队列
                        DownloadAppData_TaskHandle = osThreadNew(DownloadAppData_task_runnable, NULL, &DownloadAppData_task_attributes);
                        Queue_AppDataBuffer = xQueueCreate( 2, sizeof(uint8_t *));   // uint8_t * 
                        Semphore_ExtFlashState = xSemaphoreCreateMutex();

                        EEP_Flag_ota_state = 0x11 ;
                        ee_WriteBytes(&EEP_Flag_ota_state, 0x00 , 1);
                    }
                    else{
                        /* 清零数据 */
                        g_au8_ota_cmd[0] = 0 ;
                        g_au8_ota_cmd[1] = 0 ;
                        g_au8_ota_cmd[2] = 0 ;
                        g_au8_ota_cmd[3] = 0 ;                    
                    }
                }
                else{
                    g_au8_ota_cmd[0] = 0 ;
                    g_au8_ota_cmd[1] = 0 ;
                    g_au8_ota_cmd[2] = 0 ;
                    g_au8_ota_cmd[3] = 0 ;    
                }

                break;
            case eOTA_DOWNLOAD:
                /* 进入下载状态并校验下载状态*/
                app_data_length = Ymodem_Receive(g_au8_YmodemRec_A, g_au8_YmodemRec_B) ;
                if( 0 < app_data_length )
                {
                    /* 切换状态并发送指令，数据已经接收完毕 */
                    g_Ota_State = eOTA_WAIT_REQ_UPDATE ;

                    HAL_UART_Transmit(&huart1, req_updateAckCmd, 3 , 1000);
                        osDelay(10);    // 加一个小延时，看是否能阻塞输出
                    /* 切换状态，并清除一切生成的资源 */
                    // TODO:
                    xSemaphoreTake(Semphore_ExtFlashState, portMAX_DELAY);      // 为什么拿了又释放？
                    xSemaphoreGive(Semphore_ExtFlashState);
                    // 写入剩余数据
                    W25Q64_WriteData_End();

                    vTaskDelete(DownloadAppData_TaskHandle);
                    vQueueDelete(Queue_AppDataBuffer);
                    vSemaphoreDelete(Semphore_ExtFlashState);

                    /* 下载成功 */
                    EEP_Flag_ota_state = 0x22 ;
                    ee_WriteBytes(&EEP_Flag_ota_state, 0x00 , 1);

                    // Test
                    uint8_t read_ota_state = 0 ;
                    ee_ReadBytes(&read_ota_state, 0x00, 1);
                    HAL_UART_Transmit(&huart1, &read_ota_state, 1 , 1000);

                    /* 写入数据长度 */
                    ee_WriteBytes( &app_data_length, 0x01, sizeof(app_data_length) );
                    /* 这里是否应该等待数据完全下载完成在写入数据长度？否则如果下载失败，还需要清理，残留 ？*/

                    // Test
                    uint8_t read_app_data_length[4] = {0} ;
                    ee_ReadBytes(&read_app_data_length, 0x01, 4);
                    HAL_UART_Transmit(&huart1, read_app_data_length, 4, 1000);
                }
                else
                {
                    g_Ota_State = eOTA_WAIT_REQ_DOWNLOAD ;
                    /* 切换回状态，并清除一切生成的资源 */
                    // TODO:
                    vTaskDelete(DownloadAppData_TaskHandle);
                    vQueueDelete(Queue_AppDataBuffer);
                    vSemaphoreDelete(Semphore_ExtFlashState);
                }
                break;
            case eOTA_WAIT_REQ_UPDATE:
                /* 阻塞式接收串口命令*/
                if( HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_au8_ota_cmd, 4) )
                {
                    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
                }
                /* 等待数据接收完成*/
                xQueueReceive(Q_YmodemReclength, &t_u8_rec_length, portMAX_DELAY);
                /* 校验数据长度*/
                if( 3 == t_u8_rec_length )
                {
                    /* 校验数据内容 */
                    if( (0x77 == g_au8_ota_cmd[0]) && (0x88 == g_au8_ota_cmd[1])&& (0x99 == g_au8_ota_cmd[2]) )
                    {
                        /* 切换状态 */
                        g_Ota_State = eOTA_END ;
                        // TODO：

                    }
                    else{
                        /* 清零数据 */
                        g_au8_ota_cmd[0] = 0 ;
                        g_au8_ota_cmd[1] = 0 ;
                        g_au8_ota_cmd[2] = 0 ;
                        g_au8_ota_cmd[3] = 0 ;                    
                    }
                }
                else{
                    g_au8_ota_cmd[0] = 0 ;
                    g_au8_ota_cmd[1] = 0 ;
                    g_au8_ota_cmd[2] = 0 ;
                    g_au8_ota_cmd[3] = 0 ;    
                }
                break;
            case eOTA_END:
                /* 判断按键,在 20s 内判断按键是否按下  */
                if( 1 == KeyScan() )
                {
                    /* 执行软复位*/
                    SoftReset();
                }
                else
                {
                    /* 无限阻塞*/
                    g_Ota_State = eOTA_WAIT_REQ_DOWNLOAD;
                }
                break;
            
            default:
                break;
        }
    }
  /* USER CODE END key_task_runnable */
}

/**
 * @brief  按键扫描检测逻辑
 *          在 20s 的时间内检测按键是否被按下
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
static int8_t KeyScan(void)
{
    uint16_t key_scan_cnt = 0 ;
    for(  ; key_scan_cnt < 400; key_scan_cnt++ )
    {
        if( GPIO_PIN_RESET == HAL_GPIO_ReadPin( KEY_GPIO_Port, KEY_Pin) )
        {   
            HAL_Delay(20);
            if( GPIO_PIN_RESET == HAL_GPIO_ReadPin( KEY_GPIO_Port, KEY_Pin) )
            {
                /* 判断为按下 */
                return 1 ;
            }
        }
        HAL_Delay(30);
    }

    return -1 ;
}

/**
 * @brief  软复位
 * 
 * @details
 * @note
 * @warning
 */
void SoftReset()
{
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
    // NVIC_SystemReset();
}


/**
 * @brief  OTA 数据下载线程，将接收的APP数据写入外部 Flash 
 * 
 * @param [in]  argument 
 * 
 * @details
 * @note
 * @warning
 */
void DownloadAppData_task_runnable(void *argument)
{

    uint8_t * pu8_data = NULL ;
    uint32_t * pu32_size = NULL ; 
    // 第一帧数据接收的是 文件头数据，长度等
    xQueueReceive(Queue_AppDataBuffer, &pu32_size, portMAX_DELAY);   // 这里从 32 位强制转换为 8 位不会丢失数据吗？ 
    /* 擦除所有的 flash size  扇区*/
    // TODO :
    xSemaphoreGive( Semphore_ExtFlashState );
    while(1)
    {
        xQueueReceive(Queue_AppDataBuffer, &pu8_data, portMAX_DELAY);
        xSemaphoreTake(Semphore_ExtFlashState, 0);
        if( NULL == pu8_data )
        {
            continue ;
        }
        
        /* 执行写入w25q的逻辑 */
        W25Q64_WriteData(pu8_data, g_u32_datalength); 
        xSemaphoreGive( Semphore_ExtFlashState );

    }

}
