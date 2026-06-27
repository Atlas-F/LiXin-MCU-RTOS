/**
 * @file LedState.c
 * @author MAX
 * @brief  LED State  Source file
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "LedState.h"
#include "main.h"


/**
 * @brief Get the Led State object
 * 
 * @param led 
 * @return LedState 
 */
LedState GetLedState(LedStatus* led)
{
    // led->GPIO_Port = LED_GPIO_Port ;
    // led->GPIO_Pin = LED_Pin ;

    if( HAL_GPIO_ReadPin(led->GPIO_Port, led->GPIO_Pin) == GPIO_PIN_RESET )
    {
        return LED_ON ;
    }
    return LED_OFF;

}

/**
 * @brief Set the Led State object
 * 
 * @param led 
 * @param state 
 * @return LedState 
 */
LedState SetLedState( LedStatus* led , LedState state)
{
    switch( state )
    {
        case LED_ON:  
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_RESET) ;
            return LED_ON;
        case LED_OFF:
            HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_SET) ;
            return LED_OFF;
    }

}

/**
 * @brief Get the Key State object
 * 
 * @param key 
 * @return KeyState 
 */
KeyState GetKeyState(KeyStatus* key)
{
    if( HAL_GPIO_ReadPin(key->GPIO_Port, key->GPIO_Pin) == GPIO_PIN_RESET )
    {
        return KEY_Pressed ;
    }
    return KEY_Released ;
}



