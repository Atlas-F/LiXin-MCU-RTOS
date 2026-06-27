/**
 * @file LedState.h
 * @author MAX
 * @brief Header file for the LedState class, which manages the state of an LED.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef _LEDSTATE_H_
#define _LEDSTATE_H_

#include "stm32f4xx_hal.h"


/**
 * @brief   LED Status 
 * 
 */
typedef struct LEDSTATUS
{
    GPIO_TypeDef* GPIO_Port; // GPIO port for the LED
    uint16_t GPIO_Pin;       // GPIO pin for the LED
} LedStatus;


/**
 * @brief   LED  State
 * 
 */
typedef enum
{
    LED_ON = 0,
    LED_OFF = 1
} LedState;


/**
 * @brief   KEY  Status
 * 
 */
typedef struct KEYSTATUS
{
    GPIO_TypeDef* GPIO_Port; // GPIO port for the KEY
    uint16_t GPIO_Pin;       // GPIO pin for the KEY
} KeyStatus ;


/**
 * @brief  KEY State
 * 
 */
typedef enum
{
    KEY_Idle = 0 ,
    KEY_Debounce ,
    KEY_Pressed  ,
    KEY_Released 
} KeyState ;

// KeyState static volatile KeyCurrentState = KEY_Idle ;

typedef enum
{
    KEY_Event_Idle = 0 , 
    KEY_Event_Debounce_Start ,
    KEY_Event_Debounce_End
} KeyEvent ;


/**
 * @brief Get the Led State object
 * @param led 
 * @return LedState 
 */
LedState GetLedState(LedStatus* led);


/**
 * @brief Set the Led State object
 * 
 * @param led 
 * @param state 
 * @return LedState 
 */
LedState SetLedState( LedStatus* led , LedState state);



/**
 * @brief Get the Key State object
 * 
 * @param key 
 * @return KeyState 
 */
KeyState GetKeyState(KeyStatus* key);









#endif /* _LEDSTATE_H_ */
