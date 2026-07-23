/**
 * ********************************************************************************
 * @file KeyLedControl.c
 * @brief 
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-18
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



 /************************** private include  ************************************* */
#include "KeyLedControl.h"




/* Private variables ---------------------------------------------------------*/

extern QueueHandle_t g_Queue_KeyLED ;
extern QueueHandle_t g_Queue_KeyIrq ;




/************************** private function declare  ******************************* */


void LED_BlinkSeveralTimes( uint32_t times);



/************************** private function definition  ******************************* */


/**
 * @brief 判断长按还是短按
 * 
 * @param press_time  按下时间
 * @return KEY_PressStatus_t 
 * 
 * @note  小于 1 秒为短按，1~2为长按
 */
KEY_PressStatus_t KeyJudgeShortLongPress( uint32_t press_time )
{
    if( press_time > 0 && press_time < 1000 )
    {
        return eKEY_SHORT_PRESSED ;
    }
    else if( press_time >= 1000 && press_time < 2000 )
    {
        return eKEY_LONG_PRESSED ;
    }
    return eKEY_PRESSED ;       // 这个垃圾值 有待考虑
}


/**
 * @brief 按键线程任务
 * 
 * @param pvParameters 
 */
void KeyTask( void *pvParameters  )
{
    // 用于计算电平信息的两个变量
    static KeyIrqStatus_t fall_edge_status = {
        .KeyLevel = GPIO_PIN_RESET ,
        .keyTick = 0 
    };
    static KeyIrqStatus_t rise_edge_status = {
        .KeyLevel = GPIO_PIN_SET ,
        .keyTick = 0 
    };

    // 操作类型
    static LED_operation_t key_operate = eLED_INITED_VALUE ;

    for(;;)
    {
        // printf("KeyTask Running ! \r\n");
        // vTaskDelay(500);

        // 接收信息变量
        KeyIrqStatus_t Key_status = {
            .KeyLevel = GPIO_PIN_SET ,
            .keyTick = 0 
        };

        if(pdTRUE == xQueueReceive(g_Queue_KeyIrq, &Key_status, 10))
        {
            vTaskDelay(20);             // 消抖延时  10
            if( Key_status.KeyLevel == HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) )
            {
                // 数据更新赋值
                if( Key_status.KeyLevel == GPIO_PIN_RESET)
                {
                    fall_edge_status = Key_status ;
                    printf(" fall_edge_status level:[%d], tick:[%d] \r\n", fall_edge_status.KeyLevel, 
                                                                fall_edge_status.keyTick );
                }
                else if( Key_status.KeyLevel == GPIO_PIN_SET )
                {
                    rise_edge_status = Key_status ;
                    printf(" rise_edge_status level:[%d], tick:[%d] \r\n", rise_edge_status.KeyLevel, 
                                                                rise_edge_status.keyTick );

                    // 按下 时间长度 计算
                    uint32_t press_ms  = 0 ;
                    press_ms = rise_edge_status.keyTick - fall_edge_status.keyTick ;
                    printf(" Key press_ms [%d]! \r\n", press_ms);
                    
                    // 按照时长阈值判断操作类型
                    if( press_ms > 0 && press_ms < 1000 )
                    {
                        key_operate = eLED_TOGGLE ;
                        printf(" Key press short! \r\n");
                    }
                    else if( press_ms >= 1000 && press_ms < 2000 )
                    {
                        key_operate = eLED_BLINK_1_TIMES ;
                        printf(" Key press more than 1 second ! \r\n");
                    }
                    else if( press_ms >= 2000  )
                    {
                        key_operate = eLED_BLINK_3_TIMES ;
                        printf(" Key press more than 2 second ! long press \r\n");
                    }

                    // 发送指令数据到队列
                    xQueueSendToBack(g_Queue_KeyLED, &key_operate, 10);
                    press_ms = 0 ;

                }

            }
            else
            {
                printf(" current Key debounce level wrong ! \r\n");
            }
            printf(" current Key status level:[%d], tick:[%d] \r\n", Key_status.KeyLevel, 
                                                                Key_status.keyTick );
        }

    }
}


/**
 * @brief LED 线程 任务
 * 
 * @param pvParameters 
 */
void LedTask( void *pvParameters  )
{
    for(;;)
    {
        // printf("LedTask Running ! \r\n");
        // vTaskDelay(500);

        LED_operation_t key_operate = eLED_INITED_VALUE ;
        if( pdTRUE == xQueueReceive( g_Queue_KeyLED, &key_operate, 10 ) )
        {
            switch (key_operate)
            {
            case eLED_ON:
                /* code */
                break;
                
            case eLED_OFF:
                /* code */
                break;

            case eLED_TOGGLE:
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
                break;
            case eLED_BLINK_1_TIMES:
                LED_BlinkSeveralTimes(1);
                break;
            case eLED_BLINK_3_TIMES:
                LED_BlinkSeveralTimes(3);
                break;
            case eLED_BLINK_10_TIMES:
                LED_BlinkSeveralTimes(10);
                break;
            
            default:
                break;
            }
        }

    }
}


/**
 * @brief 按键中断回调函数
 * 
 * @param GPIO_Pin 
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 读取当前数据：电平、tick 计数
    KeyIrqStatus_t press_status  = {
        .KeyLevel = GPIO_PIN_SET ,
        .keyTick = 0
    };

    KeyIrqStatus_t * p_press_status = &press_status;

    press_status.KeyLevel = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
    press_status.keyTick = HAL_GetTick();
    // printf(" Tick_num:[%d], trigger_type:[%d] \n ", press_status.keyTick, press_status.KeyLevel);

    // 发送到队列 g_Queue_KeyIrq
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR( g_Queue_KeyIrq, &press_status, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}


/**
 * @brief LED 闪烁次数
 * 
 *  1 秒为一个周期，亮灭均持续500 ms
 * @param blink_times 
 */
void LED_BlinkSeveralTimes( uint32_t blink_times)
{
    printf(" Led Blink [%d] \r\n", blink_times );
    for( uint8_t times = 0 ; times < blink_times ; times++)
    {
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
        HAL_GPIO_TogglePin( LED_GPIO_Port ,  LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(500)); 
        // printf(" Led Blink [%d] \r\n", times );
    }
}
