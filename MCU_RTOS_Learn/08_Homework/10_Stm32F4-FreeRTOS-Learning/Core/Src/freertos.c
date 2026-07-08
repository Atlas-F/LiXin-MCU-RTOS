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

QueueHandle_t Queue_Key_Cmd = NULL ;
QueueHandle_t Queue_Cmd_Led = NULL ;

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

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    // create tasks
    // 按键状态任务
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

    // LED 亮灭任务
    BaseType_t Led = xTaskCreate( LedTask,
                                    " LED flash ",
                                    1000,
                                    NULL,
                                    2, 
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
 *          短按、长按、连按多次等
 * 
 * 以电平作为判断或者以状态作为判断
 */
void KeyTask( void  )
{
    DEBUG_PRINT("KeyTask ! \n ") ;       // 该方式会附带详细路径和行号
    for(;;)
    {
        // 时长和次数，基本单元开始
        KeyCMD_t key_cmd_status = eKEY_CMD_IDLE; 

        static GPIO_PinState Key_last_level = GPIO_PIN_SET;    /* Key_last_level 必须加 static（或者提升为全局/模块级静态变量），
                                                                否则它在每次进入 for(;;) 循环时都会被重新初始化为 1，
                                                                永远记不住“上一个周期的电平”，按键检测就废了。 */
        static uint8_t key_press_times = 0 ;        // 按键按下次数
        static uint8_t key_gap_ms = 0 ;             // 按键间隔时长
        keyState_t key_last_state = eKEY_IDLE ;
        static uint32_t press_ms = 0 ;


        GPIO_PinState Key_current_level = HAL_GPIO_ReadPin(KEY_GPIO_Port,  KEY_Pin );

        if( GPIO_PIN_SET == (Key_last_level) )      // 上一帧为高电平
        {
            if( Key_current_level == GPIO_PIN_RESET )   // 当前帧为低电平
            {
                // 按键按下
                press_ms = press_ms + 10 ;
                printf(" KEY short press down! \n ");
            }
            else
            {
                // 按键未按下， 或者抖动中
            }
        }
        else                                            // 上一帧为低电平              
        {
            vTaskDelay(10);
            if( Key_current_level == GPIO_PIN_RESET )
            {
                // 按键按下中
                press_ms = press_ms + 10 ;
            }
            else                                        // 当前帧为高电平
            {
                vTaskDelay(10);     // 消抖后需要重读电平
                if( GPIO_PIN_SET != HAL_GPIO_ReadPin(KEY_GPIO_Port,  KEY_Pin ) )
                {
                    break;
                }
                // 按键弹起
                press_ms = press_ms + 10 ;
                if(  press_ms >= 0  && press_ms < 1000 )
                {
                    // 短按
                    // printf(" KEY short press! \n ");
                    DEBUG_PRINT("KEY short press! \n ") ;       // 该方式会附带详细路径和行号
                    key_cmd_status = eKEY_CMD_PRESS ;       // 单击
                    xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
                    press_ms = 0 ;

                    key_press_times++;
                    key_gap_ms++;       //  这里如果要精确的计时，需要使用定时器或者是知道时间片长度，否则无法保证 自增一次就是1ms 
                    
                    // TODO ... if 判断按键计数次数和间隔时间，得出连按几次的结果

                }else if (press_ms >= 1000  && press_ms < 2000)
                {
                    /* 长按一秒 */
                    key_cmd_status = eKEY_CMD_LONGPRESS ;   // 长按
                    xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
                    printf(" KEY long press 1 second! \n ");
                    press_ms = 0 ;
                    
                }else if(press_ms >= 2000  && press_ms < 3000)
                {
                    /* 长按2 秒 */
                    // TODO ...
                    printf(" KEY long press 2 second! \n ");
                    press_ms = 0 ;
                }else if(press_ms >= 3000 )
                {
                    /* 长按 超过3 秒 */
                    // TODO ...
                    printf(" KEY long press more than 3 second! \n ");
                    press_ms = 0 ;
                }
                // key_press_times++;
                
            }
        }
        /* 状态更新 */
        Key_last_level =  Key_current_level ;


        // 获取
        // KeyCMD_t key_cmd_status = eKEY_CMD_IDLE;    // 传输命令信息
        // GPIO_PinState Key_level = HAL_GPIO_ReadPin(KEY_GPIO_Port,  KEY_Pin );

        // static uint32_t press_ms = 0 ;

        // switch ( Key_level )        // 按键电平
        // {
        //     case GPIO_PIN_RESET:            // 这个分支的状态机应该可以再优化，否则高电平有毛用？
        //         vTaskDelay(5);
        //         press_ms = press_ms + 5 ;
        //         if( HAL_GPIO_ReadPin(KEY_GPIO_Port,  KEY_Pin ) )
        //         {
        //             if( press_ms < 1000 )
        //             {
        //                 key_cmd_status = eKEY_CMD_PRESS ;       // 单击
        //                 xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
        //             }
        //             else
        //             {
        //                 key_cmd_status = eKEY_CMD_LONGPRESS ;   // 长按
        //                 xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
        //             }
        //             press_ms = 0;
        //         }
        //         break;

        //     case GPIO_PIN_SET:
        //         // if( press_ms < 1000 )
        //         // {
        //         //     key_cmd_status = eKEY_CMD_PRESS ;
        //         //     xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
        //         // }
        //         // else
        //         // {
        //         //     key_cmd_status = eKEY_CMD_LONGPRESS ;
        //         //     xQueueSendToBack( Queue_Key_Cmd, &key_cmd_status , 100 ) ;
        //         // }
        //         // press_ms = 0;
        //         break;
            
        //     default:
        //         break;
        // }

    }
}



/**
 * @brief  按键命令类型，不同的按键命令执行不同的任务
 * 
 */
void KeyCmdTask( void )
{
    DEBUG_PRINT("KeyCmdTask ! \n ") ;       // 该方式会附带详细路径和行号
    for(;;)
    {
        BaseType_t key_status  =  0 ;
        KeyCMD_t key_cmd_status = eKEY_IDLE ;     // 这里的指针似乎有问题

        key_status = xQueueReceive( Queue_Key_Cmd, &key_cmd_status, 100 );
        if( pdPASS != key_status )
        {
            // printf(" Could not Receive data from KEY_CMD !\n");
        }

        xQueueSendToBack( Queue_Cmd_Led, (uint8_t *)&key_cmd_status, 10 );

        // switch ( key_cmd_status )       // 这个状态机似乎没有用啊，一样的操作，有用，健壮性，idle 状态
        // {
        //     case eKEY_CMD_PRESS:
        //         xQueueSendToBack( Queue_Cmd_Led, (uint8_t *)&key_cmd_status, 10 );
        //         break;

        //     case eKEY_CMD_LONGPRESS:
        //         xQueueSendToBack( Queue_Cmd_Led, (uint8_t *)&key_cmd_status, 10 );
        //         break;
            
        //     default:
        //         break;
        // }
    }
}

/**
 * @brief LED  flash function 
 * 
 */
void LedTask( void  )
{
    DEBUG_PRINT("LedTask ! \n ") ;       // 该方式会附带详细路径和行号
    for(;;)
    {
        BaseType_t led_status  =  0 ;
        KeyCMD_t key_cmd_status = eKEY_CMD_IDLE ;     // 这里的指针似乎有问题
        led_status = xQueueReceive( Queue_Cmd_Led, &key_cmd_status, 100 );
        if( led_status != pdPASS )
        {
            // printf(" Could not Receive data from KEY !\n");
        }
        switch( key_cmd_status )
        {

            case eKEY_CMD_PRESS :
                // printf("LedTask is running! \n");       // 为什么不打印？
                HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
                break;

            case eKEY_CMD_LONGPRESS :
                LedToggleSeveralTimes(3);
                break ;

            default:
                break;
        }
    }
}


/* USER CODE END Application */

