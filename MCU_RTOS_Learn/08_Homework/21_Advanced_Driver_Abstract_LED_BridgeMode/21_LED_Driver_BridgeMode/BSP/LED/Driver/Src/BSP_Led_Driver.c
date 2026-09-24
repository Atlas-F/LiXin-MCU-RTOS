/**
 * 
 * @file   BSP_Led_Driver.c
 * @brief   高级驱动抽象入门，LED 设备的 BSP 层，驱动源文件
 * 
 * @author AtlasFg
 * @date   2026-09-23
 * @version 1.0
 * 
 * @dependencies
 *  - stdio
 *  - stdint
 *  - BSP_Led_Driver.h
 * 
 * @par 说明
 * * 
 * @note 注意事项
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */

/*---------- Private Includes -------------------------------------------------- */
#include "main.h"  

#include "BSP_Led_Driver.h"


/*----------- Private typedef ------------------------------------------------*/


/*----------- Private define -------------------------------------------------*/


/*--------- Private variables ------------------------------------------------*/


/*--------- Private function prototypes --------------------------------------*/

/**
 * @brief  Init the target BSP_LedDriver_t
 * 
 * @param [in]  self pointer of input data
 * @return    Led_Status_t the status of running   
 * 
 * @details
 * @note
 * @warning
 */
Led_Status_t Led_Driver_Init( BSP_LedDriver_t * const self )
{
    Led_Status_t ret  = LED_OK ;
    if( (NULL == self)  )
        return LED_ERRORPARAMETER ;
#ifdef DEBUG
        DEBUG_OUT("Led_Driver_Init start \r\n");
#endif 

    self->p_Led_OperaInst->pf_Led_Off();
    self->p_OS_TimeDelay->pf_OS_Delay_ms(600);
    uint32_t time_stamp = 0 ;
    self->p_TimeBase_ms->pf_Get_Time_ms(&time_stamp);

    return ret;

}












/**
 * @brief   Instiantiate the target of BSP_Led_Driver  实例化 
 *  
 * Steps:
    1, Adding the Core interfaces into target of bsp_led_driver instance.
    2, Adding the OS interfaces into target of bsp_ledd_driver instance.
    3, Adding the timebase interfaces into targetof bsp_led_driver instance.
 * @param [in]  self 
 * @param [in]  led_Ops 
 * @param [in]  time_base 
 * @param [in]  OS_delay 
 * @return      Led_Status_t  the status of running
 * 
 * @details
 * @note
 * @warning
 */
Led_Status_t LedDriver_inst(  BSP_LedDriver_t * const self,
                               Led_Operations_t  * const led_Ops,      // const Led_Operations_t  * const led_Ops,
                               Time_Base_ms_t    * const time_base,    // const Time_Base_ms_t    * const time_base, 
#ifdef OS_SUPPORTING
                               OS_Delay_t        * const OS_delay      // const OS_Delay_t        * const OS_delay
#endif /* OS_SUPPORTING */                           
) 
{
    DEBUG_OUT("led inst kick-off \r\n");
    Led_Status_t ret ;
/**************** 1.Checing the input parameters **************** */

    if( (NULL == self) || (NULL == led_Ops) || ( NULL == time_base) )
        return LED_ERRORPARAMETER ;
#ifdef OS_SUPPORTING
    if( NULL == OS_delay)   
        return LED_ERRORPARAMETER ;
#endif /* OS_SUPPORTING */  


/**************** 1.Checing the Resources **************** */
    if( INITED == self->is_inited)
    {
#ifdef DEBUG
        DEBUG_OUT("LED_ERRORRESOURCE");
        return  LED_ERRORRESOURCE    ;
#endif 
    }

#ifdef DEBUG
        DEBUG_OUT("led  inst start \r\n");
#endif 


/**************** 1.Adding the interface  **************** */
    self->p_Led_OperaInst = led_Ops ;
    self->p_OS_TimeDelay = OS_delay ;
    self->p_TimeBase_ms = time_base ;       /* 这里可以内部做参数的校验 */

/**************** 1. Init the target  **************** */
    self->blink_times = 0 ;
    self->cycle_time_ms = 0 ;  
    self->proportion_on_off =     ePROPORTION_X_X ;

    ret = Led_Driver_Init(self);
    if( LED_OK != ret )
    {
#ifdef DEBUG
        DEBUG_OUT("Init failed \r\n");
#endif 
        self->p_Led_OperaInst = NULL ;
        self->p_OS_TimeDelay = NULL ;
        self->p_TimeBase_ms = NULL ;       

        return ret;
    }

    self->is_inited = INITED ;
    DEBUG_OUT("init finished  \r\n");





}
    
