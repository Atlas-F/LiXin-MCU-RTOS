/**
 * ********************************************************************************
 * @file   ADC_DMA_Sampling.c
 *
 * @brief  ADC and DMA sampling, double-buffer draft.
 *
 * @author AtlasFg
 * @date   2026-08-01
 * @version 1.0
 * ********************************************************************************
 */
/* Private includes ----------------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "elog.h"
#include "SEGGER_RTT.h"

#include "stdio.h"
#include <stdint.h>

#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "semphr.h"

#include "ADC_DMA_Sampling.h"


/* Private define ------------------------------------------------------------*/

#define TAG_ADCDMA    "ADC_DMA"

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
    eMAIL_INIT               = 0,
    eMAIL_DMA_EOC            = 1,
    eMAIL_WAKETASKDATACONVER = 2,
    eMAIL_BUF_A_RECEIVE      = 3,
    eMAIL_BUF_B_RECEIVE      = 4,
    eMAIL_RESERVED           = 0x7FFFFFFF
} Mailbox_Data_t;

/* Private variables ---------------------------------------------------------*/

uint16_t g_buffer_A[16] = {0};       // origin : 1
uint16_t g_buffer_B[16] = {0};

bool g_buf_use_A = true;

QueueHandle_t g_Mailbox = NULL;
QueueHandle_t g_Mailbox_DataConver = NULL;

SemaphoreHandle_t g_DMA_ISR_mutex = NULL ;

SemaphoreHandle_t g_Mail_Dataconvert = NULL ;
SemaphoreHandle_t g_Mail_BufferFree = NULL ;



/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  ADC DMA 转换运输的应用层初始化
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BaseType_t AppADC_DMA_Init(void)
{
    elog_i(LOG_TAG, " AppADC_DMA_Init start !\r\n");
    printf("// printf SetDMATask create SUCCESS! \n");

    /* 创建任务：切换DMA目标、数据处理*/
    BaseType_t SetDMA = xTaskCreate(SetDMATargetTask, " Set DMA Buffer Target ",
                                    1000, NULL, 25, NULL);
    if (NULL != SetDMA)
    {
        // printf("SetDMATask create SUCCESS! \n");
        elog_i(LOG_TAG, "SetDMATask create SUCCESS! \n");
    }
    else
    {
        // printf("SetDMATask create FAIL! \n");
        elog_i(LOG_TAG, "SetDMATask create FAIL! \n");
    }
    elog_i(LOG_TAG, " SetDM:[%d] !\r\n", SetDMA);

    BaseType_t DataConver = xTaskCreate(DataConversionTask, " ADC data conversion  ",
                                        1000, NULL, 25, NULL);
    if (pdPASS == DataConver)
    {
        // printf("DataConverATask create SUCCESS! \n");
        elog_i(LOG_TAG, "DataConverATask create SUCCESS! \n");
    }
    else
    {
        // printf("DataConverTask create FAIL! \n");
        elog_i(LOG_TAG, "DataConverTask create FAIL! \n");
    }

    /* 创建消息队列：DMA 传输完成、任务间通知*/
    g_Mailbox = xQueueCreate((UBaseType_t)1, sizeof(Mailbox_Data_t));
    if (NULL == g_Mailbox)
    {
        // printf(" g_Mailbox queue create FAIL! \n ");
    }
    g_Mailbox_DataConver = xQueueCreate((UBaseType_t)1, sizeof(Mailbox_Data_t));
    if (NULL == g_Mailbox_DataConver)
    {
        // printf(" g_Mailbox_DataConver queue create FAIL! \n ");
    }

    /* 创建 互斥锁、二值信号量*/
    g_DMA_ISR_mutex = xSemaphoreCreateMutex();

    g_Mail_Dataconvert = xSemaphoreCreateBinary();
    g_Mail_BufferFree = xSemaphoreCreateBinary();

    /* 启动ADC、DMA 传输*/
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_A, 16);
    g_buf_use_A = true;

    elog_i(LOG_TAG, " ADC_DMA start !\r\n");

    return pdPASS;
}

/**
 * @brief  
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BaseType_t Switch_DMA_BufferTarget(void)
{
    return 0;
}

/**
 * @brief  任务：设置 DMA 运输目标 任务 A
 * 
 * @param [in]  pvparameters 
 * @return      
 * 
 * @details
 *      设置g_buf_use_A 标志位来决定 使用哪个 buffer， 并确定传输信号决定处理哪一个buffer
 * @note
 * @warning
 */
BaseType_t SetDMATargetTask(void *pvparameters)
{
    for (;;)
    {
        Mailbox_Data_t mail_eoc = eMAIL_INIT;
        Mailbox_Data_t mail_data = eMAIL_INIT;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        /* 邮箱接收消息*/
        // xQueuePeek(g_Mailbox, &mail_eoc, 0);
            // xSemaphoreTake(g_DMA_ISR_mutex, 10 );
        if(pdPASS == xQueuePeek(g_Mailbox, &mail_eoc, 0))
        {
            /* 通过添加 xQueueReceive 来确保严格同步的阻塞状态*/
            if(pdPASS == xQueueReceive(g_Mailbox, &mail_eoc, 0))
            {
                // xSemaphoreGive(g_DMA_ISR_mutex);
                if (eMAIL_DMA_EOC == mail_eoc)
                {
                    if (true == g_buf_use_A)    // 使用 buf_A 切换为使用 buf_B
                    {
                        mail_data = eMAIL_BUF_A_RECEIVE;
                        xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
                            // V 操作 DMA 转换已完成，可以进行数据处理
                            xSemaphoreGive(g_Mail_Dataconvert );
                            // P 操作，数据处理已经完成，可以开始下一缓冲区目标的切换
                        if( pdPASS == xSemaphoreTake(g_Mail_BufferFree, 10 ) )
                        {
                            HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_B, 16);
                            g_buf_use_A = false;
                        }
                    }
                    else
                    {
                        mail_data = eMAIL_BUF_B_RECEIVE;
                        xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
                            xSemaphoreGive(g_Mail_Dataconvert );
                        if( pdPASS == xSemaphoreTake(g_Mail_BufferFree, 10 ) )
                        {
                            HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_A, 16);
                            g_buf_use_A = true;
                        }
                    }

                }
            }
            // xSemaphoreGive(g_DMA_ISR_mutex);


        }
    }
}

/**
 * @brief  ADC  DMA 转运数据处理任务  任务 B
 * 
 * @param [in]  pvparameters 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BaseType_t DataConversionTask(void *pvparameters)
{
    for (;;)
    {
        Mailbox_Data_t mail_data = eMAIL_INIT;
                    // xSemaphoreTake(g_Mail_Dataconvert, 10 );
        if( pdPASS == xSemaphoreTake(g_Mail_Dataconvert, 10 ) )
        {
            if(pdPASS == xQueuePeek(g_Mailbox_DataConver, &mail_data, 0))
            {
                if(pdPASS == xQueueReceive(g_Mailbox_DataConver, &mail_data, 0) )
                {
                    // 接收同步
                    if (eMAIL_BUF_A_RECEIVE == mail_data)
                    {
                        /* 处理数据并log 输出*/
                        uint32_t buf_num = 0;
                        uint32_t buf_ave;

                        for (int i = 0; i < 16; i++) {
                            buf_num += g_buffer_A[i];
                        }
                        buf_ave = buf_num / 16;
                        float voltage = buf_ave * 3.3f / 4095.0f;
                        elog_i(TAG_ADCDMA, " Voltage A = [%.2f] ", voltage);

                    }
                    else
                    {
                        uint32_t buf_num = 0;
                        uint32_t buf_ave;

                        for (int i = 0; i < 16; i++) {
                            buf_num += g_buffer_B[i];
                        }
                        buf_ave = buf_num / 16;
                        float voltage = buf_ave * 3.3f / 4095.0f;
                        elog_i(TAG_ADCDMA, " Voltage B = [%.2f] ", voltage);
                    }
                        // 发送消息 buffer free
                        xSemaphoreGive(g_Mail_BufferFree);
                }
            }

        }
    }
}

/**
 * @brief  ADC DMA 中断函数
 * 
 * @param [in]  hadc 
 * 
 * @details
 * @note
 * @warning
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    Mailbox_Data_t mail_data = eMAIL_DMA_EOC; 
    /* 在中断中使用中断安全函数 FromISR 时不要忘记 xHigherPriorityTaskWoken ，并使用portYIELD_FROM_ISR */
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueOverwriteFromISR(g_Mailbox, &mail_data, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}
