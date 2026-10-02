/**
 *
 * @file   BSP_LED_Handler.c
 * @brief  BSP 层，关于 LED 的抽象
 *
 * @author AtlasFg
 * @date   2026-09-26
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


/*---------- Private Includes -------------------------------------------------- */
#include "main.h"

#include "BSP_LED_Handler.h"

#include "FreeRTOSConfig.h"

/*----------- Private typedef ------------------------------------------------*/

/*----------- Private define -------------------------------------------------*/

/*--------- Private variables ------------------------------------------------*/

/*--------- Private function prototypes --------------------------------------*/


/**
 * @brief 
 *
 * @return
 *
 * @details
 * @note
 * @warning
 */
static BspLedHandlerStatus_t BspLedHandler_CriticalExit(void)
{
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_CriticalExit \r\n");
#endif



    return status;
}

/**
 * @brief
 *
 * @return
 *
 * @details
 * @note
 * @warning
 */
static BspLedHandlerStatus_t BspLedHandler_CriticalEnter(void)
{
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_CriticalEnter \r\n");
#endif




    return status;
}

/**
 * @brief
 *
 * @param [in]  instance_group
 * @param [in]  instance_num
 * @return
 *
 * @details
 * @note    这个函数有待进行逻辑检查，初始化方式
 * @warning
 */
static BspLedHandlerStatus_t BspLedHandler_InitializeArray(BspLedDriver_t **const instance_group,
                                                           uint32_t instance_num)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_InitializeArray \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

    /**************** 1.Checing the input parameters **************** */
    if (NULL == instance_group) return LED_HANDLER_STATUS_ERROR_NULL_POINTER;


    /**************** 2.Checing the Resources *********************** */
    if (instance_num > BSP_LED_HANDLER_MAX_INSTANCE_NUM) return LED_HANDLER_STATUS_ERROR_PARAMETER;
        /* 这里的判断逻辑不对 */

    /**************** 3、do the action ****************************** */

    for (uint32_t index = 0U; index < instance_num; index++)
    {
        instance_group[index] = NULL;

    }

    return status;
}

/**
 * @brief
 *
 * @param [in]  self
 * @param [in]  led_driver
 * @return
 *
 * @details
 * @note
 * @warning
 */
static BspLedHandlerStatus_t BspLedHandler_Register(BspLedHandler_t *const self,
                                                    BspLedDriver_t *const led_driver,
                                                    BspLedHandlerIndex_t *const led_index)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_Register \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

    /**************** 1.Checing the input parameters **************** */
    if ((NULL == self) || (NULL == led_driver) || (NULL == led_index) ||
        (BSP_LED_NOT_INITIALIZED == self->initialized))
        return LED_HANDLER_STATUS_ERROR_NULL_POINTER;




    /**************** 2.Checing the Resources *********************** */
    // if( (NULL == self->instance_group->instance_group ) ||
    //     (  self->instance_group->instance_num > BSP_LED_HANDLER_MAX_INSTANCE_NUM))
    //     return LED_HANDLER_STATUS_ERROR_NULL_POINTER;
    if (BSP_LED_NOT_INITIALIZED == led_driver->initialized)
    {
        /* Options: mutex to upgrade low priority task to init target ASAP  */
#ifdef DEBUG_ENABLED
        DEBUG_OUT("led handler LED_HANDLER_STATUS_ERROR_RESOURCE \r\n");
#endif
        return LED_HANDLER_STATUS_ERROR_RESOURCE;
    }



    /**************** 3、Add the instance to the target array ******* */
    if ((BSP_LED_HANDLER_MAX_INSTANCE_NUM - self->instance_group->instance_num) == 0U)
    {
        status = LED_HANDLER_STATUS_ERROR_RESOURCE;
        return status;
    }
//TODO:/* 进入临界区，避免竞态 */
BspLedHandler_CriticalEnter();
    if ((BSP_LED_HANDLER_MAX_INSTANCE_NUM - self->instance_group->instance_num) > 0U)
    {
        self->instance_group->instance_group[self->instance_group->instance_num] =
                                led_driver;
        *led_index = (BspLedHandlerIndex_t)self->instance_group->instance_num;
        self->led_index = *led_index;
        self->instance_group->instance_num++;
        // return LED_HANDLER_STATUS_OK;
    }
//TODO:/* 退出临界区 */
BspLedHandler_CriticalExit();
    return LED_HANDLER_STATUS_OK;


    return status;

}


/**
 * @brief
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
static BspLedHandlerStatus_t BspLedHandler_Control(BspLedHandler_t *const self,
                                                   uint32_t cycle_time_ms,
                                                   uint32_t blink_times,
                                                   BspLedProportionOnOff_t proportion_on_off)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_Control \r\n");
#endif
    BspLedHandlerStatus_t status; /* 驱动初始化结果状态 */
    /**************** 1.Checing the input parameters **************** */
    if ( (NULL == self) ) return LED_HANDLER_STATUS_ERROR_NULL_POINTER;

    /* 这里的参数检查是基于业务逻辑的限幅检查，实际项目中可灵活采用 */
    if ((cycle_time_ms > 1000U) || 
        (proportion_on_off > LED_PROPORTION_1_1) ||
        (blink_times > 1000U))
    {
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }



    /**************** 2.Checing the Resources *********************** */
    if ((BSP_LED_INITIALIZED != self->initialized) ||
                       (NULL == self->queue_handle)) 
        return LED_HANDLER_STATUS_ERROR_PARAMETER;






    /**************** 3、do the action ****************************** */

    //TODO:
    // BspLedStatus_t driver_status = self->BspLedDriver_Control(self,
    //                                                            cycle_time_ms,
    //                                                            blink_times,
    //                                                            proportion_on_off);
    // status = (BspLedHandlerStatus_t)driver_status;

    // TODO：异步IO操作，发送队列到handler中，然后运行到 handler 中的时候，处理接收到的事件
    /* 这里的 led_event 是否还应该有具体的 led 设备？ */
    BspLedHandlerEvent_t led_event = {
        .cycle_time_ms     = cycle_time_ms,
        .blink_times       = blink_times,
        .proportion_on_off = proportion_on_off
    };
    /* 将操作指令入队 */
    status = self->os_queue->BspLedHandler_PutQueue(self->queue_handle, &led_event, 0);
    if( LED_HANDLER_STATUS_OK != status )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("BspLedHandler_PutQueue failed \r\n");
#endif
    }


    return status;

}




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
BspLedHandlerStatus_t BspLedHandler_Instantiate(BspLedHandler_t *const handler_self,
                                                 BspLedHandlerTimeBaseMs_t *const time_base_ms,
#ifdef OS_SUPPORTING
                                                 BspLedHandlerOsDelay_t *const os_delay,
                                                 BspLedHandlerOsQueue_t *const os_queue,
                                                 BspLedHandlerOsThread_t *     os_thread
#endif /* OS_SUPPORTING */
)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("led handler inst kick-off \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; /* 驱动初始化结果状态 */
    /**************** 1.Checing the input parameters **************** */

    if ((NULL == handler_self) || (NULL == time_base_ms)) return LED_HANDLER_STATUS_ERROR_PARAMETER;
#ifdef OS_SUPPORTING
    if ((NULL == os_delay) || (NULL == os_queue) ||( NULL == os_thread)) 
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
#endif /* OS_SUPPORTING */

    /**************** 2.Checing the Resources *********************** */
    if (BSP_LED_INITIALIZED == handler_self->initialized)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("LED_HANDLER_STATUS_ERROR_RESOURCE");
#endif
        return LED_HANDLER_STATUS_ERROR_RESOURCE;
    }

#ifdef DEBUG_ENABLED
    DEBUG_OUT("led handler inst start \r\n");
#endif

    /**************** 3.Adding the interface  *********************** */
        /* mount the external interface  */
        handler_self->time_base_ms = time_base_ms;
        handler_self->os_delay     = os_delay;
        handler_self->os_queue     = os_queue;
        handler_self->os_thread    = os_thread;

        /* mount the internal interface  */
        handler_self->BspLedHandler_Control                       = BspLedHandler_Control;
        handler_self->BspLedHandler_RegisterDriver                = BspLedHandler_Register;
        handler_self->critical_region.BspLedHandler_CriticalEnter = BspLedHandler_CriticalEnter; /* 这里的临界区操作应该只是 对于该模块handler 来说，需要应用到其他模块吗？ */
        handler_self->critical_region.BspLedHandler_CriticalExit  = BspLedHandler_CriticalExit;

            // handler_self->instance_group->instance_num = 0;
            // handler_self->instance_group->instance_group = NULL ;
            /*  这里 是否要进行赋值？ */
    /**************** 4. Init the target  *************************** */
    /* 初始化创建对象，避免数组溢出越界，但是看案例没有这一步啊
        这里的静态对象传递给非静态，是否会有问题？ */
    static BspLedHandlerInstanceGroup_t instance_group = {0};
    handler_self->instance_group = &instance_group;

    // TODO :
        /* Init the variables will be used */
        /* 这里的初始化要不要分配空间？还是register 的时候再分配空间？ */
        handler_self->instance_group->instance_num = 0U;
        status = BspLedHandler_InitializeArray(handler_self->instance_group->instance_group,
                                               BSP_LED_HANDLER_MAX_INSTANCE_NUM);
    if (LED_HANDLER_STATUS_OK != status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" BspLedHandler_InitializeArray Init failed \r\n");
#endif
        handler_self->os_delay     = NULL;
        handler_self->time_base_ms = NULL;
        return status;
    }

        /* Init the variables will be used */
    status = handler_self->os_queue->BspLedHandler_CreateQueue(5, sizeof(BspLedHandlerEvent_t), &handler_self->queue_handle);
    if( LED_HANDLER_STATUS_OK != status )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" BspLedHandler_CreateQueue failed \r\n");
#endif
        handler_self->os_queue->BspLedHandler_DestroyQueue(&handler_self->queue_handle);
    }

    status = handler_self->os_thread->BspLedHandler_CreateThread(BspLedHandler_ThreadController,      /* 这个函数指针应该怎么传？ 这个算是线程函数，应该和 handler 在同一层吗？或者是driver？*/
                                                                "BspLedHandler_ThreadController",
                                                                128U,
                                                                handler_self,
                                                                NULL,
                                                                &handler_self->thread_handle );
    if( 1  != status )  /* pdPASS != status  */
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" BspLedHandler_CreateThread failed \r\n");
#endif
        handler_self->os_thread->BspLedHandler_DestroyThread(handler_self->thread_handle);
    }



    /**************************************************************** */


    handler_self->initialized = BSP_LED_INITIALIZED;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_Instantiate finished  \r\n");
#endif

    return status;

}


/**
 * @brief  BspLedHandler_ThreadController 执行 LED 控制操作的专有线程
 * 
 * @return      
 * 
 * @details
 * @note
 *      这里不传参数，如何获取 handler 句柄
 * @warning
 */
static void BspLedHandler_ThreadController(void *argument)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_ThreadController \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 

    /**************** 2.Checing the Resources *********************** */
    BspLedHandler_t *handler_self = (BspLedHandler_t *)argument ;
    if( (NULL == handler_self) || 
        (NULL == handler_self->thread_handle) || 
        (NULL == handler_self->queue_handle) )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" ThreadController NULL pointer \r\n");
        if( NULL == handler_self )
        {
            DEBUG_OUT(" handler_self NULL pointer \r\n");
        }
        if( NULL == handler_self->thread_handle )
        {
            DEBUG_OUT(" handler_self->thread_handle NULL pointer \r\n");
        }
        if( NULL == handler_self->queue_handle )
        {
            DEBUG_OUT(" handler_self->queue_handle NULL pointer \r\n");
        }

#endif
        // return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }

    BspLedHandlerEvent_t led_event = {
        .cycle_time_ms     = 0,
        .blink_times       = 0,
        .proportion_on_off = 0
    };

    /**************** 3、do the action ****************************** */
    for(;;)
    {
        /******************** receive the data of led event ******************************* */
        status = handler_self->os_queue->BspLedHandler_PopQueue(handler_self->queue_handle, &led_event, 0);
        if( LED_HANDLER_STATUS_OK != status )
        {
#ifdef DEBUG_ENABLED
            DEBUG_OUT("BspLedHandler_PopQueue failed \r\n");
#endif
            continue;  /* 本轮没有新事件，不能处理旧 led_event */
        }

        /** 
         * @note
         *      这里的逻辑不对
         *      1、为什么在一直出队？
         *      2、为什么会一直执行 队列 0 号的 control 函数 ？
         *          0 号应该是没有数据的，因为是从队尾发送的
         *      3、一直在循环入队出队？但是为什么出队失败？
         */
        // handler_self->BspLedHandler_Control(&handler_self,led_event.cycle_time_ms, led_event.blink_times,led_event.proportion_on_off);
        /* 这里应该执行具体的 LED 逻辑，而不再是控制函数，因为已经将数据发送到队列中了 */
        handler_self->instance_group->instance_group[0]->BspLedDriver_Control(  handler_self->instance_group->instance_group[0],
                                                                                led_event.cycle_time_ms,
                                                                                led_event.blink_times,
                                                                                led_event.proportion_on_off);
        /******************** exe the event of the led  *********************************** */






        /* TBD : delay 延时函数 */
    }

}
