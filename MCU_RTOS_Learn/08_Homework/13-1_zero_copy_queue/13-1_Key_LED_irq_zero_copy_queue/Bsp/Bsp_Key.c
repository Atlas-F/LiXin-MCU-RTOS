/**
 * ********************************************************************************
 * @file Bsp_Key.c
 * @brief 
 * 
 * @author AtlasFg (phoenixfg@foxmail.com)
 * @date 2026-07-16
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

#include "Bsp_Key.h"





/************************** private function definition  ******************************* */

/**
 * @brief 按键扫描，判断是否按下，读取按键值，并通过形参将数据 传入和传出
 * 
 * @param key_value 
 * @return KEY_Status_t 返回操作状态 结果，成功或者超时
 * 
 * @note 是否有冗余设计的嫌疑？
 */
KEY_Status_t keyScan( KEY_PressStatus_t * key_value)
{
    if( NULL == key_value)
        return eKEY_ERRORPARAMETER ;

    KEY_PressStatus_t key_press_value  = eKEY_NOT_PRESSED ;
    if( HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if( HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_RESET)
        {
            *key_value = eKEY_PRESSED ;
            return eKEY_OK;
        }
        *key_value = eKEY_NOT_PRESSED ;
        return eKEY_ERRORTIMEOUT;
    }

    *key_value = key_press_value ;
    return eKEY_ERRORTIMEOUT ;          
}








