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

#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// #include "BSP_Led_Driver.h"
// #include "BSP_LED_Handler.h"

#include "System_adaption.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* ---------------------- Unity Test ---------------------------------------*/


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

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
  /* Infinite loop */
 
//   BspLedDriver_t led_1;
    SystemAdaption_LedInit();

#ifdef BSP_LED_DEBUG_UNITY_TEST

    // // static Bsp_Led_Handler_t bsp_led_handler_instance = {0};
    // Bsp_Led_Handler_Instantiate(&bsp_led_handler_instance,
    //                           &g_bsp_led_handler_demo_time_base_ms,
    //                           &g_bsp_led_handler_demo_os_delay,
    //                           &g_bsp_led_handler_demo_os_queue,
    //                           &g_bsp_led_handler_demo_os_thread);

    // // BspLedDriver_t led_1 = {0};
    // BspLedDriver_Instantiate(&led_1,
    //                          &g_bsp_led_demo_operations,
    //                          &g_bsp_led_demo_time_base_ms,
    //                          &g_bsp_led_demo_os_delay);
    
    // BspLedHandlerEvent_t led_event = {
    //     .cycle_time_ms     = 6,
    //     .blink_times       = 4,
    //     .proportion_on_off = LED_PROPORTION_1_2,
    //     .led_index         = LED_INDEX_1,
    // } ;                        

    // bsp_led_handler_instance.Bsp_Led_Handler_RegisterDriver(&bsp_led_handler_instance,
    //                                                        &led_1,
    //                                                        &bsp_led_handler_instance.led_index);
    // bsp_led_handler_instance.Bsp_Led_Handler_Control(&bsp_led_handler_instance, 
    //                                                 led_event.cycle_time_ms, 
    //                                                 led_event.blink_times, 
    //                                                 led_event.proportion_on_off,
    //                                                 led_event.led_index);


#endif /* BSP_LED_DEBUG_UNITY_TEST */


    // led_1.BspLedDriver_Control(&led_1, 4U, 6U, LED_PROPORTION_1_1);


  for(;;)
  {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

