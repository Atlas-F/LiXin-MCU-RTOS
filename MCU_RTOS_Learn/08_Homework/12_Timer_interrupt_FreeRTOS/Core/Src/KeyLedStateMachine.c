/**
 * 
 * @file KeyLedStateMachine.c
 * @brief  按键控制LED 状态机实现
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-05
 * @copyright Copyright (c) 2026
 * @version 0.1
 * 
 * @note 
 * @attention 
 * ********************************************************************************
 *  项目命名规范(v1.0) 
 *  局部变量                       : snake_case 
 *  全局变量                       : g_ + snake_case 
 *  静态变量                       : s_ + snake_case 
 *  指针                           : p_ 开头  
 *  函数                           : 模块前缀_PascalCase  (模块前缀大写_动词 + 名词)
 *  类型                           : PascalCase + _t 
 *  宏                             : UPPER_CASE 
 *  结构体/枚举/typedef 类型名      :PascalCase + _t 
 *  枚举值                         : eTypeName_VALUE 或 ePascalCase 
 * ********************************************************************************
 */

#include "KeyLedStateMachine.h"

#include "queue.h"

#include <stdint.h>
#include "DebugUtils.h"
#include "tim.h"


/*  -------------------------------- 全局变量前置声明 ------------------------------ */
QueueHandle_t Queue_Key_Cmd = NULL ;
QueueHandle_t Queue_Cmd_Led = NULL ;

QueueHandle_t Queue_key_Cmd_Irq = NULL ;



/*  -------------------------------- 函数前置声明 ---------------------------------- */

void KeyRunable( IrqStatus_t last_irq_status, IrqStatus_t current_irq_status  );
void LedRunable( void  );
void KeyCmdRunable( void );






/*  -------------------------------- 函数定义 -------------------------------------- */

/**
 * @brief LED  翻转多次
 * 
 * @param toggle_times  翻转次数
 */
void LedToggleSeveralTimes( uint8_t toggle_times ) 
{
    for( uint8_t times = 0 ; times < toggle_times ; times++)
    {
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}


/**
 * @brief  按键线程
 * 
 */
void KeyRunable( IrqStatus_t last_irq_status, IrqStatus_t current_irq_status  )
{

    for(;;)
    {
        KeyCMD_t key_cmd_status = eKEY_CMD_IDLE; 

        static TickType_t press_ms = 0 ;

            // IrqStatus_t peek_irq_status = {0};
            // BaseType_t peek_rece_status = xQueuePeek( Queue_key_Cmd_Irq,  &peek_irq_status,  100);
            // if( peek_rece_status != pdPASS)
            // {

            // }
            // else if ( peek_rece_status == pdPASS)
            // {
            //     printf(" PEEK key_Cmd_Irq trigger_type:[%d] , tick_num:[%d]! \n ",
            //                                                                     peek_irq_status.trigger_type , 
            //                                                                     peek_irq_status.tick_num);
            // }

        BaseType_t rece_status = xQueueReceive( Queue_key_Cmd_Irq, &current_irq_status, 100);
        press_ms = current_irq_status.tick_num - last_irq_status.tick_num ;
        if (rece_status != pdPASS)
        {
            // printf(" Queue_key_Cmd_Irq rece FAIL ! \n ");
        }
        else if(rece_status == pdPASS)
        {
            // printf(" Queue_key_Cmd_Irq rece SUCCESS ! \n ");
            printf(" KEY trigger_type:[%d] , tick_num:[%d] , press_ms:[%d]! \n ",
                                                                            current_irq_status.trigger_type ,
                                                                            current_irq_status.tick_num , 
                                                                            press_ms);
        }

        // 消抖
        vTaskDelay(15);
        if( HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) != current_irq_status.trigger_type )
        {
            continue;
        }



        if( last_irq_status.trigger_type == eEDGE_FALLING )
        {

            if( current_irq_status.trigger_type == eEDGE_RISING )
            {
                printf(" KEY  press! \n ");
                if(press_ms < 1000 && press_ms > 10)
                {
                    DEBUG_PRINT("KEY short  press! ! \n  ! ! \n ") ;
                    key_cmd_status = eKEY_CMD_PRESS ;       // 单击
                    // xQueueSendToBack( Queue_Cmd_Led, &key_cmd_status , 100 ) ;
                }
                else if( press_ms < 2000 && press_ms >= 1000 )
                {
                    DEBUG_PRINT("KEY long press 1 second! ! \n ") ;
                    key_cmd_status = eKEY_CMD_LONGPRESS ;       // 长按
                    // xQueueSendToBack( Queue_Cmd_Led, &key_cmd_status , 100 ) ;
                }
                else if( press_ms < 3000 && press_ms >= 2000 )
                {
                    DEBUG_PRINT("KEY long press more than 3 second!! ! \n ") ;
                    HAL_TIM_Base_Start_IT(&htim9);              // 特殊闪烁 
                                                                // 通过 开启或或者关闭定时器来控制闪烁次数
																																// 可以通过参数传入闪烁次数
                }
                xQueueSendToBack( Queue_Cmd_Led, &key_cmd_status , 100 ) ;

            }
            else
            {
                // TODO ...
            }
                        
        }	
        else
        {
            // TODO ...
        }
        last_irq_status = current_irq_status ;                  
    }
    
}


/**
 * @brief   KEY 命令类型 线程
 * 
 * 该任务多余，作为中转，直接发送到LED即可
 */
void KeyCmdRunable( void )
{
    BaseType_t key_status  =  0 ;
    KeyCMD_t key_cmd_status = eKEY_IDLE ;     
}

/**
 * @brief   LED  线程
 * 
 */
void LedRunable( void  )
{
    BaseType_t led_status  =  0 ;
    KeyCMD_t key_cmd_status = eKEY_CMD_IDLE ;    
    led_status = xQueueReceive( Queue_Cmd_Led, &key_cmd_status, 100 );
    if( led_status != pdPASS )
    {
        // printf(" Could not Receive data from KEY !\n");
    }
    switch( key_cmd_status )
    {

        case eKEY_CMD_PRESS :
            HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
            break;

        case eKEY_CMD_LONGPRESS :
            LedToggleSeveralTimes(3);
            break ;

        default:
            break;
    }

}


