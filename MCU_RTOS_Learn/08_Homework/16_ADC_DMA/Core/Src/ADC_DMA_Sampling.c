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

uint32_t g_buffer_A[1] = {0};
uint32_t g_buffer_B[1] = {0};

bool g_buf_use_A = true;

QueueHandle_t g_Mailbox = NULL;
QueueHandle_t g_Mailbox_DataConver = NULL;



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

    /* 启动ADC、DMA 传输*/
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_A, 1);
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
 * @brief  任务：设置 DMA 运输目标
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

        /* 邮箱接收消息*/
        // xQueuePeek(g_Mailbox, &mail_eoc, 0);
        if(pdPASS == xQueuePeek(g_Mailbox, &mail_eoc, 0))
        {
            /* 通过添加 xQueueReceive 来确保严格同步的阻塞状态*/
            if(pdPASS == xQueueReceive(g_Mailbox, &mail_eoc, 0))
            {
                if (eMAIL_DMA_EOC == mail_eoc)
                {
                    if (true == g_buf_use_A)    // 使用 buf_A 切换为使用 buf_B
                    {
                        mail_data = eMAIL_BUF_A_RECEIVE;
                        xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
                        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_B, 1);
                        g_buf_use_A = false;
                    }
                    else
                    {
                        mail_data = eMAIL_BUF_B_RECEIVE;
                        xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
                        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_A, 1);
                        g_buf_use_A = true;
                    }
                }

            }
        }
        // if (eMAIL_DMA_EOC == mail_eoc)
        // {
        //     if (true == g_buf_use_A)    // 使用 buf_A 切换为使用 buf_B
        //     {
        //         mail_data = eMAIL_BUF_A_RECEIVE;
        //         xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
        //         HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_B, 1);
        //         g_buf_use_A = false;
        //     }
        //     else
        //     {
        //         mail_data = eMAIL_BUF_B_RECEIVE;
        //         xQueueOverwrite(g_Mailbox_DataConver, &mail_data);
        //         HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_buffer_A, 1);
        //         g_buf_use_A = true;
        //     }
        // }
    }
}

/**
 * @brief  ADC  DMA 转运数据处理任务
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

        xQueuePeek(g_Mailbox_DataConver, &mail_data, 0);
        if (eMAIL_BUF_A_RECEIVE == mail_data)
        {
            /* 处理数据并log 输出*/
            float voltage = g_buffer_A[0] * 3.3f / 4095.0f;
            elog_i(TAG_ADCDMA, " Voltage A = [%.2f] ", voltage);
        }
        else
        {
            float voltage = g_buffer_B[0] * 3.3f / 4095.0f;
            elog_i(TAG_ADCDMA, " Voltage B = [%.2f] ", voltage);
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

    // float voltage = g_buffer_A[0] * 3.3f / 4095.0f;
    // elog_i(TAG_ADCDMA, " Voltage A = [%.2f] ", voltage);
}
