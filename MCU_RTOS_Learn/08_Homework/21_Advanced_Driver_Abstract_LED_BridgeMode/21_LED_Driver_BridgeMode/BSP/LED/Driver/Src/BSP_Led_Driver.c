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
 * @brief  Do the action of LED blink
 * 
 * @param [in]  self 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
Led_Status_t LED_Blink( BSP_LedDriver_t *const self)
{
#ifdef DEBUG
    DEBUG_OUT("LED_Blink start \r\n");
#endif
    /*  check the pointer and the target */
    if( NULL == self )  return LED_ERROR_NULL_POINTER;
    if( NOT_INITED == self->is_inited) return  LED_ERROR_RESOURCE;


    /* 此处是否有必要使用一层局部变量再解耦 ？ */
    /* calculate the time for each led to toggle */
    uint16_t led_toggle_time = 0 ;
    switch ( self->proportion_on_off)
    {
        case ePROPORTION_1_3 :
            led_toggle_time = (self->cycle_time_ms) / 4;
            break;
        case ePROPORTION_1_2 :
            led_toggle_time = (self->cycle_time_ms) / 3;
            break;
        case ePROPORTION_1_1 :
            led_toggle_time = (self->cycle_time_ms) / 2;
            break;                
        default:
            break;
    }



    /* Do the action */
    for (int i = 0; i < self->blink_times; i++)
    {
        for (int j = 0; j < self->cycle_time_ms; j++)
        {
            if (j < led_toggle_time)        /* 这里怎么能确保 J++ 一次的时间就是一毫秒 ？ */
            {
                self->p_Led_OperaInst->pf_Led_On();
            }
            else
            {
                self->p_Led_OperaInst->pf_Led_Off();
            }
        }
    }

#ifdef DEBUG
    DEBUG_OUT("LED_Blink End \r\n");
#endif
    
}



/**
 * @brief   Control the LED , and do the action
 * 
 * @param [in]  self 
 * @param [in]  cycle_time_ms 
 * @param [in]  blink_times 
 * @param [in]  proportion_on_off 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
Led_Status_t LED_Control(   BSP_LedDriver_t *const                   self,
                            uint32_t                        cycle_time_ms,    
                            uint32_t                          blink_times,  
                            Proportion_OnOff_t          proportion_on_off )
{
#ifdef DEBUG
    DEBUG_OUT("LED_Control start \r\n");
#endif
    /* 1、check the pointer and the parameters */
    if( NULL == self)   return LED_ERROR_NULL_POINTER;

    if( (cycle_time_ms < 0 || cycle_time_ms > 1000 ) || 
        ( proportion_on_off < 0 || proportion_on_off > 3))
    {
        return LED_ERROR_PARAMETER;
    }


    /* 2、check the target of the inited */

    if( INITED != self->is_inited)  return LED_ERROR_RESOURCE;

    /* 赋值数据 */
    self->blink_times       = blink_times;
    self->cycle_time_ms     = cycle_time_ms;
    self->proportion_on_off = proportion_on_off ;


    /* do the action */
    // TODO:
    LED_Blink(self);


#ifdef DEBUG
    DEBUG_OUT("LED_Control End \r\n");
#endif
}





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
Led_Status_t Led_Driver_Init(BSP_LedDriver_t *const self)
{
    Led_Status_t ret = LED_OK; /* 初始化结果状态 */
    if ((NULL == self)) return LED_ERROR_PARAMETER;
#ifdef DEBUG
    DEBUG_OUT("Led_Driver_Init start \r\n");
#endif

    self->p_Led_OperaInst->pf_Led_Off();
    self->p_OS_TimeDelay->pf_OS_Delay_ms(600);
    uint32_t time_stamp = 0; /* 当前毫秒时基时间戳 */
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
 * @param[in,out] self      驱动对象实例；输入时不能为 NULL，函数会写入其接口指针、
 *                          初始闪烁参数和初始化状态。
 * @param[in]     led_Ops   LED 核心操作接口，必须指向有效的 Led_Operations_t 对象，
 *                          并提供 pf_Led_On 与 pf_Led_Off 回调函数。
 * @param[in]     time_base 毫秒时基接口，必须指向有效的 Time_Base_ms_t 对象，
 *                          并提供用于读取当前时间的 pf_Get_Time_ms 回调函数。
 * @param[in]     OS_delay  操作系统    延时接口，必须指向有效的 OS_Delay_t 对象，
 *                          并提供毫秒延时的 pf_OS_Delay_ms 回调函数；仅在定义
 *                          OS_SUPPORTING 时生效。
 * @return      LED_OK             实例化成功。
 * @return      LED_ERROR_PARAMETER self、led_Ops、time_base 或 OS_delay 为 NULL。
 * @return      LED_ERROR_RESOURCE  self 对象已经完成实例化。
 * @return      其他 Led_Status_t  驱动初始化流程返回的错误状态。
 *
 * @details
 *  函数成功后，@p self 会持有上述接口对象的地址，因此这些对象在驱动使用期间
 *  必须保持有效；函数不会复制接口对象本身。
 * @note      调用前应将 self 指向的 BSP_LedDriver_t 对象清零或设置为 NOT_INITED。
 * @warning   传入的接口对象及其函数指针不得为 NULL，且生命周期必须覆盖驱动实例的使用期。
 */
Led_Status_t LedDriver_inst(BSP_LedDriver_t *const  self,
                            Led_Operations_t *const led_Ops,    // const Led_Operations_t  * const led_Ops,
                            Time_Base_ms_t *const   time_base,  // const Time_Base_ms_t    * const time_base,
#ifdef OS_SUPPORTING
                            OS_Delay_t *const OS_delay          // const OS_Delay_t        * const OS_delay
#endif                                                          /* OS_SUPPORTING */
)
{
    DEBUG_OUT("led inst kick-off \r\n");
    Led_Status_t ret; /* 驱动初始化结果状态 */
    /**************** 1.Checing the input parameters **************** */

    if ((NULL == self) || (NULL == led_Ops) || (NULL == time_base)) return LED_ERROR_PARAMETER;
#ifdef OS_SUPPORTING
    if (NULL == OS_delay) return LED_ERROR_PARAMETER;
#endif /* OS_SUPPORTING */

    /**************** 1.Checing the Resources **************** */
    if (INITED == self->is_inited)
    {
#ifdef DEBUG
        DEBUG_OUT("LED_ERROR_RESOURCE");
        return LED_ERROR_RESOURCE;
#endif
    }

#ifdef DEBUG
    DEBUG_OUT("led  inst start \r\n");
#endif

    /**************** 1.Adding the interface  **************** */
    /* mount the external interface  */
    self->p_Led_OperaInst = led_Ops;
    self->p_OS_TimeDelay  = OS_delay;
    self->p_TimeBase_ms   = time_base; /* 这里可以内部做参数的校验 */

    /* mount the internal interface  */
    self->pf_LedController = LED_Control;   /*  这里赋值可以吗？因为不是从参数传递的 */

    /**************** 1. Init the target  **************** */
    self->blink_times       = 0;
    self->cycle_time_ms     = 0;
    self->proportion_on_off = ePROPORTION_X_X;

    ret = Led_Driver_Init(self);
    if (LED_OK != ret)
    {
#ifdef DEBUG
        DEBUG_OUT("Init failed \r\n");
#endif
        self->p_Led_OperaInst = NULL;
        self->p_OS_TimeDelay  = NULL;
        self->p_TimeBase_ms   = NULL;

        return ret;
    }

    self->is_inited = INITED;
    DEBUG_OUT("init finished  \r\n");
}
