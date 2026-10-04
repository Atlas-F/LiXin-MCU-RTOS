/**
 * 
 * @file   System_adaption.h
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-10-03
 * @version 1.0
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
#ifndef __SYSTEM_ADAPTION__
#define __SYSTEM_ADAPTION__


/* *********************************** Includes ***************************** */



#include "BSP_Led_Driver.h"
#include "BSP_LED_Handler.h"




/* *********************************** Defines ****************************** */









/* ******************************* Declaration ****************************** */


// static BspLedStatus_t BspLedDemo_On(void);

// static BspLedStatus_t BspLedDemo_Off(void);

// static BspLedStatus_t BspLedDemo_GetTimeMs(uint32_t *const time_stamp);

// static BspLedStatus_t BspLedDemo_DelayMs(uint32_t delay_time);

void SystemAdaption_LedDemoInit(void);

/**
 * @brief  
 * 
 * @param [in]  item_num 
 * @param [in]  item_size 
 * @param [in]  queue_handle 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
// static BspLedHandlerStatus_t BspLedDemo_QueueCreate(uint32_t item_num,
                                                    //  uint32_t item_size,
                                                    //  void **queue_handle);


/**
 * @brief  
 * 
 * @param [in]  queue_handle 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
// static BspLedHandlerStatus_t BspLedDemo_QueueDestroy(void **queue_handle);

/**
 * @brief  
 * 
 * @param [in]  queue_handle 
 * @param [in]  queue_item 
 * @param [in]  timeout 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
// static BspLedHandlerStatus_t BspLedDemo_QueuePut(void *const queue_handle,
//                                                   void *const queue_item,
//                                                   uint32_t timeout);


/**
 * @brief  
 * 
 * @param [in]  queue_handle 
 * @param [in]  queue_item 
 * @param [in]  timeout 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
// static BspLedHandlerStatus_t BspLedDemo_QueuePop(void *const queue_handle,
//                                                   void *const     queue_item,
//                                                   uint32_t   timeout);



/**
 * @brief  LED Handler inst 实例化
 *
 * @param [in]  handler_self
 * @param [in]  time_base_ms
 * @param [in]  os_delay
 * @return
 *
 * @details
 * @note
 * 1、    handler_self->BspLedHandler_Control = BspLedHandler_Control;
 *              这里的控制策略其实是绑定的，那么对于不同的 LED 来说，其实是同样的控制策略
                    但是不同的LED应该有不同的控制策略，所以这里不应该绑定，应该由 各自的LED
                    初始化的时候，绑定不同的控制策略，即将具体的 LED 控制函数指针绑定下放
                    而不是在 Handler 层绑定
                但是如果将控制策略升级到 handler 层中，那么对于不同的控制策略也需要调整，
                如果只是一种闪烁模式，通过调整参数来达到不同的效果，那么其实还是一种模式的抽象？
                应该是包含不同的闪烁模式，例如：呼吸灯、固定间隔闪烁、渐亮、渐灭等，都是不同的控制策略
 *              那么就应该升级到 handler 层进行使用和适配
    2、handler_self->os_queue = os_queue;
            这里的 OS_Queue 在 inst 函数内部绑定具体的接口和在外部绑定效果相同吗？外部已经绑定好接口，内部在进行绑定可以吗？
 * @warning
 */
// BspLedHandlerStatus_t BspLedHandler_Instantiate(BspLedHandler_t *const handler_self,
//                                                  BspLedHandlerTimeBaseMs_t *const time_base_ms,
// #ifdef OS_SUPPORTING
//                                                  BspLedHandlerOsDelay_t *const os_delay,
//                                                  BspLedHandlerOsQueue_t *const os_queue,
//                                                  BspLedHandlerOsThread_t *     os_thread
// #endif /* OS_SUPPORTING */
// );

/**
 * @brief 
 *
 * @return
 *
 * @details
 * @note
 *      这个函数应该放在适配层吗？
 * @warning
 */
// static BspLedHandlerStatus_t BspLedHandler_CriticalExit(void);


/**
 * @brief
 *
 * @return
 *
 * @details
 * @note
 * @warning
 */
// static BspLedHandlerStatus_t BspLedHandler_CriticalEnter(void);


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
// BspLedStatus_t BspLedDriver_Instantiate(BspLedDriver_t *const self,
//                                          BspLedOperations_t *const operations, // const BspLedOperations_t *const operations,
//                                          BspLedTimeBaseMs_t *const time_base_ms, // const BspLedTimeBaseMs_t *const time_base_ms,
// #ifdef OS_SUPPORTING
//                                          BspLedOsDelay_t *const os_delay // const BspLedOsDelay_t *const os_delay
// #endif /* OS_SUPPORTING */
// );

















#endif /* __SYSTEM_ADAPTION__ */

