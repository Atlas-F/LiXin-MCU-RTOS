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
static BspLedStatus_t BspLedDriver_Blink(BspLedDriver_t *const self)
{
    BspLedStatus_t status = LED_STATUS_OK;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDriver_Blink start \r\n");
#endif
    /*  check the pointer and the target */
    if (NULL == self) return LED_STATUS_ERROR_NULL_POINTER;
    if (BSP_LED_NOT_INITIALIZED == self->initialized) return LED_STATUS_ERROR_RESOURCE;


    /* 此处是否有必要使用一层局部变量再解耦 ？ */
    /* 避免竞态，修改数据 */
    /* Options: 此处可以通过局部变量传参来保存使用的数据，可以避免竞态情况下第三者对数据的修改，
                或者使用互斥锁或者信号量来解耦，避免此处执行出现问题 */

    /* calculate the time for each led to toggle */
    uint16_t led_toggle_time = 0;
    switch (self->proportion_on_off)
    {
        case LED_PROPORTION_1_3:
            led_toggle_time = self->cycle_time_ms / 4U;
            break;
        case LED_PROPORTION_1_2:
            led_toggle_time = self->cycle_time_ms / 3U;
            break;
        case LED_PROPORTION_1_1:
            led_toggle_time = self->cycle_time_ms / 2U;
            break;
        default:
#ifdef DEBUG_ENABLED
            DEBUG_OUT("BspLedDriver_Blink LED_STATUS_ERROR_PARAMETER  \r\n");
#endif
            return LED_STATUS_ERROR_PARAMETER;
    }



    /* Do the Blink action */
    for (uint32_t blink_index = 0U; blink_index < self->blink_times; blink_index++)
    {
        for (uint32_t time_index = 0U; time_index < self->cycle_time_ms; time_index++)
        {
            if (time_index < led_toggle_time) /* 这里怎么能确保 J++ 一次的时间就是一毫秒 ？ */
            {
                self->operations->BspLed_On();
            }
            else
            {
                self->operations->BspLed_Off();
            }
        }
    }
    status = LED_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDriver_Blink End \r\n");
#endif

    return status;
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
BspLedStatus_t BspLedDriver_Control(BspLedDriver_t *const self,
                                     uint32_t cycle_time_ms,
                                     uint32_t blink_times,
                                     BspLedProportionOnOff_t proportion_on_off)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDriver_Control start \r\n");
#endif
    /* 1、check the pointer and the parameters */
    if (NULL == self) return LED_STATUS_ERROR_NULL_POINTER;

    if ((cycle_time_ms > 1000U) || (proportion_on_off > LED_PROPORTION_1_1))
    {
        return LED_STATUS_ERROR_PARAMETER;
    }

    /* Options: 检查参数的有效性，这里的有效性指的是和程序功能含义相关的有效数据，可能语法没有问题，但是不符合设计要求 */

    /* 2、check the target of the inited */

    if (BSP_LED_INITIALIZED != self->initialized) return LED_STATUS_ERROR_RESOURCE;

    /* 赋值数据 */
    self->blink_times       = blink_times;
    self->cycle_time_ms     = cycle_time_ms;
    self->proportion_on_off = proportion_on_off;


    /* 3、do the action */
    // TODO:
    BspLedStatus_t status = BspLedDriver_Blink(self);


#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDriver_Control End \r\n");
#endif
    return status;
}





/**
 * @brief  Init the target BspLedDriver_t
 *
 * @param [in]  self pointer of input data
 * @return    BspLedStatus_t the status of running
 *
 * @details
 * @note        这个函数有待优化！
 * @warning
 */
static BspLedStatus_t BspLedDriver_Initialize(BspLedDriver_t *const self)
{
    BspLedStatus_t status = LED_STATUS_OK; /* 初始化结果状态 */
    if (NULL == self) return LED_STATUS_ERROR_PARAMETER;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDriver_Initialize start \r\n");
#endif

    self->operations->BspLed_Off();
    self->os_delay->BspLed_DelayMs(600U);
    uint32_t time_stamp = 0U; /* 当前毫秒时基时间戳 */
    self->time_base_ms->BspLed_GetTimeMs(&time_stamp);

    return status;
}

#if 1

/**
 * @brief   Instiantiate the target of BSP_Led_Driver  实例化
 *
 * Steps:
    1, Adding the Core interfaces into target of bsp_led_driver instance.
    2, Adding the OS interfaces into target of bsp_ledd_driver instance.
    3, Adding the timebase interfaces into targetof bsp_led_driver instance.
 * @param[in,out] self         驱动对象实例；输入时不能为 NULL，函数会写入其接口指针、
 *                              初始闪烁参数和初始化状态。
 * @param[in]     operations   LED 核心操作接口，必须指向有效的 BspLedOperations_t 对象，
 *                              并提供 BspLed_On 与 BspLed_Off 回调函数。
 * @param[in]     time_base_ms 毫秒时基接口，必须指向有效的 BspLedTimeBaseMs_t 对象，
 *                              并提供用于读取当前时间的 BspLed_GetTimeMs 回调函数。
 * @param[in]     os_delay     操作系统    延时接口，必须指向有效的 BspLedOsDelay_t 对象，
 *                              并提供毫秒延时的 BspLed_DelayMs 回调函数；仅在定义
 *                              OS_SUPPORTING 时生效。
 * @return      LED_STATUS_OK              实例化成功。
 * @return      LED_STATUS_ERROR_PARAMETER self、operations、time_base_ms 或 os_delay 为 NULL。
 * @return      LED_STATUS_ERROR_RESOURCE  self 对象已经完成实例化。
 * @return      其他 BspLedStatus_t        驱动初始化流程返回的错误状态。
 *
 * @details
 *  函数成功后，@p self 会持有上述接口对象的地址，因此这些对象在驱动使用期间
 *  必须保持有效；函数不会复制接口对象本身。
 * @note      调用前应将 self 指向的 BspLedDriver_t 对象清零或设置为 BSP_LED_NOT_INITIALIZED。
 * @warning   传入的接口对象及其函数指针不得为 NULL，且生命周期必须覆盖驱动实例的使用期。
 */
BspLedStatus_t BspLedDriver_Instantiate(BspLedDriver_t *const self,
                                         BspLedOperations_t *const operations, // const BspLedOperations_t *const operations,
                                         BspLedTimeBaseMs_t *const time_base_ms, // const BspLedTimeBaseMs_t *const time_base_ms,
#ifdef OS_SUPPORTING
                                         BspLedOsDelay_t *const os_delay // const BspLedOsDelay_t *const os_delay
#endif /* OS_SUPPORTING */
)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("led inst kick-off \r\n");
#endif
    BspLedStatus_t status; /* 驱动初始化结果状态 */
    /**************** 1.Checing the input parameters **************** */

    if ((NULL == self) || (NULL == operations) || (NULL == time_base_ms)) return LED_STATUS_ERROR_PARAMETER;
#ifdef OS_SUPPORTING
    if (NULL == os_delay) return LED_STATUS_ERROR_PARAMETER;
#endif /* OS_SUPPORTING */

    /**************** 1.Checing the Resources **************** */
    if (BSP_LED_INITIALIZED == self->initialized)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("LED_STATUS_ERROR_RESOURCE");
#endif
        return LED_STATUS_ERROR_RESOURCE;
    }

#ifdef DEBUG_ENABLED
    DEBUG_OUT("led inst start \r\n");
#endif

    /**************** 1.Adding the interface  **************** */
    /* mount the external interface  */
    self->operations   = operations;
    self->os_delay     = os_delay;
    self->time_base_ms = time_base_ms; /* 这里可以内部做参数的校验 */

    /* mount the internal interface  */
    self->BspLedDriver_Control = BspLedDriver_Control; /*  这里赋值可以吗？因为不是从参数传递的 */

    /**************** 1. Init the target  **************** */
    self->blink_times       = 0U;
    self->cycle_time_ms     = 0U;
    self->proportion_on_off = LED_PROPORTION_INVALID;

    status = BspLedDriver_Initialize(self);
    if (LED_STATUS_OK != status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" BspLedDriver_Initialize Init failed \r\n");
#endif
        self->operations   = NULL;
        self->os_delay     = NULL;
        self->time_base_ms = NULL;

        return status;
    }

    self->initialized = BSP_LED_INITIALIZED;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("init finished  \r\n");
#endif
    return status;
}


#endif
