/**
 * 
 * @file   BSP_Led_Driver.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-09-23
 * @version 1.0
 * 
 * @par 说明
 * * 
 * @note 注意事项
 *              该代码方案起始还有很大的参考空间
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */

#ifndef     __BSP_LED_DRIVER__
#define     __BSP_LED_DRIVER__

/*---------- Private Includes -------------------------------------------------- */
#include <stdint.h>
#include <stdio.h>


/*----------- Private define -------------------------------------------------*/


#define INITED              1/* led is Inited */
#define NOT_INITED          0

#define OS_SUPPORTING       /* OS_SUPPORTING depending on OS avaliable */
#define DEBUG

#define DEBUG_OUT(X)        printf(X)   /* 自定义 debug 输出方式 */

/*----------- Private typedef ------------------------------------------------*/

typedef enum
{
    LED_OK                   = 0 ,
    LED_ERROR                = 1 ,
    LED_ERRORTIMEOUT         = 2 ,
    LED_ERRORRESOURCE        = 3 ,
    LED_ERRORPARAMETER       = 4 ,
    LED_ERFORNOMEMORY        = 5 ,
    LED_ERRORISR             = 6 ,
    LED_RESERVED             = 0xFF ,

} Led_Status_t ;


typedef enum
{
    ePROPORTION_1_3 = 0 ,
    ePROPORTION_1_2 = 1 ,
    ePROPORTION_1_1 = 2 ,
    ePROPORTION_X_X = 0xFF
} Proportion_OnOff_t ;

typedef struct
{
    Led_Status_t (* pf_Led_On) (void);
    Led_Status_t (* pf_Led_Off) (void);
} Led_Operations_t ;

typedef struct 
{
    Led_Status_t (* pf_Get_Time_ms) (uint32_t * const);

} Time_Base_ms_t ;

#ifdef OS_SUPPORTING
typedef struct 
{
    Led_Status_t (* pf_OS_Delay_ms) (const uint32_t );

} OS_Delay_t ;
#endif /* OS_SUPPORTING */

typedef Led_Status_t (* pf_Led_Control_t)( uint32_t,            // cycle time
                                           uint32_t,            // blink times
                                        Proportion_OnOff_t     // light On or Off
                                        );


typedef struct 
{
    /**************** Target of internal status **** */
     uint8_t 													is_inited;


    /**************** Target of Features **** */
    /* blink cycle time*/
    uint32_t                      cycle_time_ms;
    /* LED  blink times */
    uint32_t                        blink_times;
    /* light On or Off */
    Proportion_OnOff_t        proportion_on_off;  

    /**************** Target of IOs needs **** */
    /* ******* the interface of  Core Layer    */
    Led_Operations_t            * p_Led_OperaInst;
    Time_Base_ms_t               * p_TimeBase_ms;
    
    /* ******* the interface of  RTOS  Layer    */
#ifdef OS_SUPPORTING
    OS_Delay_t                  * p_OS_TimeDelay;
#endif /* OS_SUPPORTING */
    /**************** Target of APIs ********* */
    pf_Led_Control_t              pf_LedController;


} BSP_LedDriver_t;


/*--------- Private declaring  ------------------------------------------------*/

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
Led_Status_t LedDriver_inst(  
                            BSP_LedDriver_t * const self,
                             Led_Operations_t  * const led_Ops,
                             Time_Base_ms_t    * const time_base, 
#ifdef OS_SUPPORTING
                             OS_Delay_t        * const OS_delay
#endif /* OS_SUPPORTING */
) ;  



/*--------- Private variables ------------------------------------------------*/


/*--------- Private function prototypes --------------------------------------*/













#endif      /* __BSP_LED_DRIVER__ */

