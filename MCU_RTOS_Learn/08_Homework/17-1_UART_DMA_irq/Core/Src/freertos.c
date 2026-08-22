/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "elog.h"
#include "SEGGER_RTT.h"

#include "BSP_uart_driver.h"
#include "uart_parse_task.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

QueueHandle_t g_queue_sub_app  = NULL ;


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskUartRecA */
osThreadId_t TaskUartRecAHandle;
const osThreadAttr_t TaskUartRecA_attributes = {
  .name = "TaskUartRecA",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for UartDriverFunc */
osThreadId_t UartDriverFuncHandle;
const osThreadAttr_t UartDriverFunc_attributes = {
  .name = "UartDriverFunc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,      // osPriorityLow
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void UartRecFuncA(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TaskUartRecA */
  TaskUartRecAHandle = osThreadNew(UartRecFuncA, NULL, &TaskUartRecA_attributes);

/* creation of UartDriverFunc */
  UartDriverFuncHandle = osThreadNew(UartDriverFunc, NULL, &UartDriverFunc_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

    uint8_t cmd[1] = 0xAB;
    g_queue_sub_app = xQueueCreate(1, 4);
    uint8_t ret = APP_ArraySubcribe( cmd, g_queue_sub_app);

    App_message_t temp_msg = {0};
  /* Infinite loop */
    for(;;)
    {
        xQueueReceive(g_queue_sub_app, &temp_msg, 0 );
        for( int i = 0; i < temp_msg.data_cnt; i++ )
        {
            log_i("sub_app data:[%d] ", temp_msg.data[i]);
        }
        temp_msg.data_cnt = 0 ;

    osDelay(1);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_UartRecFuncA */
// /**
// * @brief Function implementing the TaskUartRecA thread.
// * @param argument: Not used
// * @retval None
// */
// /* USER CODE END Header_UartRecFuncA */
// void UartRecFuncA(void *argument)
// {
//   /* USER CODE BEGIN UartRecFuncA */
//     elog_i(LOG_TAG, " UartRecFuncA Init Success !\r\n");

//   /* Infinite loop */
//   for(;;)
//   {
//     osDelay(1);
//   }
//   /* USER CODE END UartRecFuncA */
// }

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

