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
#include "queue.h"
#include "semphr.h"
#include <stdint.h>

#include "KeyLedStateMachine.h"
#include "DebugUtils.h"


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

// QueueHandle_t Queue_Key_Cmd = NULL ;
// QueueHandle_t Queue_Cmd_Led = NULL ;

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

void KeyTask( void  );
void LedTask( void  );
void KeyCmdTask( void );

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

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    // communicate between led and key
    Queue_Key_Cmd = xQueueCreate( (UBaseType_t)10, sizeof(uint8_t) );
    if( NULL == Queue_Key_Cmd )
    {
        printf(" Queue_Key_Cmd queue create FAIL! \n ");
    }

    Queue_Cmd_Led = xQueueCreate( (UBaseType_t)10, sizeof(uint8_t) );
    if( NULL == Queue_Cmd_Led )
    {
        printf(" Queue_Cmd_Led queue create FAIL! \n ");
    }

    Queue_key_Cmd_Irq = xQueueCreate( (UBaseType_t)10, sizeof( IrqStatus_t ) );
    if( NULL == Queue_key_Cmd_Irq )
    {
        printf(" Queue_key_Cmd_Irq queue create FAIL! \n ");
    }

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    // create tasks
    // 按键状�?�任�??
    BaseType_t  key = xTaskCreate( KeyTask,
                                    " Key control LED ",
                                    1000,
                                    NULL,
                                    3,      // 1 
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
                                    3,          // 2
                                    NULL );
    if(  pdPASS == Led )
    {
        printf("LedTask create SUCCESS! \n");
    }
    else{
        printf("LedTask create FAIL! \n");
    }

    // 按键 命令 任务
    BaseType_t KeyCmd = xTaskCreate( KeyCmdTask,
                                    " Key Cmd ",
                                    1000,
                                    NULL,
                                    3, 
                                    NULL );
    if(  pdPASS == KeyCmd )
    {
        printf("KeyCmdTask create SUCCESS! \n");
    }
    else{
        printf("KeyCmdTask create FAIL! \n");
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
    vTaskDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
 * @brief Key check function
 *      按键基本的动作分解，分类出不同的按键动作和命令类型：
 *          短按、长按�?�连按多次等
 * 
 * 以电平作为判断或者以状�?�作为判�??
 */
void KeyTask( void  )
{
    DEBUG_PRINT("KeyTask ! \n ") ;       // 该方式会附带详细路径和行�??
    
    static IrqStatus_t last_irq_status = {0};
    static IrqStatus_t current_irq_status = {0};


    // for(;;)                              // 循环放在外面，局部变量的生命周期，形参的值，每一次都是从前面重新获取， 实参一直没变，所以形参的初值也一直是 0。
    // {                                    // 或者传递指针
        KeyRunable(last_irq_status, current_irq_status);

    // }

}



/**
 * @brief  按键命令类型，不同的按键命令执行不同的任�??
 * 
 */
void KeyCmdTask( void )
{
    DEBUG_PRINT("KeyCmdTask ! \n ") ;       // 该方式会附带详细路径和行�??
    for(;;)
    {
        KeyCmdRunable();
    }
}

/**
 * @brief LED  flash function 
 * 
 */
void LedTask( void  )
{
    DEBUG_PRINT("LedTask ! \n ") ;       // 该方式会附带详细路径和行�??
    for(;;)
    {
        LedRunable();
    }
}


/* USER CODE END Application */

