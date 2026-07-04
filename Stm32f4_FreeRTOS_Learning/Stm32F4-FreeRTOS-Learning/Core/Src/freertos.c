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
QueueHandle_t queue_key_led = NULL ;
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
    queue_key_led = xQueueCreate( (UBaseType_t)10, sizeof(uint8_t) );
    if( NULL == queue_key_led )
    {
        printf(" queue_key_led queue create FAIL! \n ");
    }

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    // create tasks
    BaseType_t  key = xTaskCreate( KeyTask,
                                    " Key control LED ",
                                    1000,
                                    NULL,
                                    1, 
                                    NULL );
    if(  pdPASS == key )
    {
        printf("KeyTask create SUCCESS! \n");
    }
    else{
        printf("KeyTask create FAIL! \n");
    }

    BaseType_t Led = xTaskCreate( LedTask,
                                    " LED flash ",
                                    1000,
                                    NULL,
                                    1, 
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
		
        vTaskDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
 * @brief Key check function
 * 
 */
void KeyTask( void  )
{
    for(;;)
    {
        // 获取
        keyState_t key_status = eKEY_IDLE;
        GPIO_PinState key_state = HAL_GPIO_ReadPin(KEY_GPIO_Port,  KEY_Pin );
        // printf("KEYTASK is running! \n");
        switch ( key_state )
        {
            case GPIO_PIN_RESET:
                key_status = eKEY_DOWN ;
                vTaskDelay(10);
                if( HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET )
                {
                    break;      // 这里还不太明白，似乎是因为偶发性重合，延时后不一定高电平，
                                // 机械抖动或者是按的时间长强制等到高电平，需要回看第一节
                }
                xQueueSendToBack(queue_key_led, (uint8_t *)&key_status, 10  );
                printf("KeyTask is running! \n");
                break;
            case GPIO_PIN_SET:
                key_status = eKEY_UP ;
                xQueueSendToBack(queue_key_led, (uint8_t *)&key_status, 10  );
                break;
            
            default:
                break;
        }

        // printf("KeyTask is running! \n");
        // vTaskDelay(500);
    }
}


/**
 * @brief LED  flash function 
 * 
 */
void LedTask( void  )
{
    for(;;)
    {
        BaseType_t led_status  =  0 ;
        keyState_t key_status = eKEY_IDLE ;     // 这里的指针似乎有问题
        led_status = xQueueReceive( queue_key_led, &key_status, 100 );
        if( led_status != pdPASS )
        {
            printf(" Could not Receive data from KEY !\n");
            // 在没有收到的时候会一直打印信息
        }
        // printf("LEDTASK is running! \n");
        switch( key_status )
        {
            case eKEY_DOWN:
                vTaskDelay(pdMS_TO_TICKS(50));          //  test bug
                printf("LedTask is running! \n");       // 为什么不打印？
                HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
                break;
            case eKEY_UP:

                break;
            default:
                break;

        }

    }
}

/* USER CODE END Application */

