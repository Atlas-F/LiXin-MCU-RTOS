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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "semphr.h"

#include "../../Bsp/Bsp_Key.h"
#include "../../Bsp/Bsp_Led.h"
#include "KeyLedControl.h"

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

QueueHandle_t g_Queue_KeyLED = NULL ;
QueueHandle_t g_Queue_KeyIrq = NULL ;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTimer01 */
osTimerId_t myTimer01Handle;
const osTimerAttr_t myTimer01_attributes = {
  .name = "myTimer01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void CallbackTimer01(void *argument);

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

  /* Create the timer(s) */
  /* creation of myTimer01 */
  myTimer01Handle = osTimerNew(CallbackTimer01, osTimerPeriodic, NULL, &myTimer01_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
    g_Queue_KeyLED = xQueueCreate( (UBaseType_t)10, sizeof(uint8_t) );
    if( NULL == g_Queue_KeyLED )
    {
        printf(" g_Queue_KeyLED queue create FAIL! \n ");
    } 

    g_Queue_KeyIrq = xQueueCreate( (UBaseType_t)10, sizeof(KeyIrqStatus_t) );
    if( NULL == g_Queue_KeyIrq )
    {
        printf(" g_Queue_KeyIrq queue create FAIL! \n ");
    } 




  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    BaseType_t  key = xTaskCreate( KeyTask,
                                    " Key control LED ",
                                    1000,
                                    NULL,
                                    25,      // 1 
                                    NULL );
    if(  pdPASS == key )
    {
        printf("KeyTask create SUCCESS! \n");
    }
    else{
        printf("KeyTask create FAIL! \n");
    }

        // LED 亮灭任务
    BaseType_t Led = xTaskCreate( LedTask,
                                    " LED flash ",
                                    1000,
                                    NULL,
                                    25,          // 2
                                    NULL );
    if(  pdPASS == Led )
    {
        printf("LedTask create SUCCESS! \n");
    }
    else{
        printf("LedTask create FAIL! \n");
    }

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
  /* Infinite loop */
  for(;;)
  {

    // BSP_LED_On_Off(eLED_OK);
    // // printf(" LED on ! \r\n");
    // vTaskDelay(500);

    // BSP_LED_On_Off(eLED_OFF);
    // // printf(" LED off ! \r\n");
    // vTaskDelay(500);

  }
  /* USER CODE END StartDefaultTask */
}

/* CallbackTimer01 function */
void CallbackTimer01(void *argument)
{
  /* USER CODE BEGIN CallbackTimer01 */

  

  /* USER CODE END CallbackTimer01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

