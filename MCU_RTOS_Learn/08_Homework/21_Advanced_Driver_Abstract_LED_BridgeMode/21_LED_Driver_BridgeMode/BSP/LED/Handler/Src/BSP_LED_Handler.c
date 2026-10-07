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

#include "FreeRTOS.h"
#include "task.h"

#include "BSP_LED_Handler.h"

/*----------- Private typedef ------------------------------------------------*/

/*----------- Private define -------------------------------------------------*/

/*--------- Private variables ------------------------------------------------*/



/*--------- Private declarations ------------------------------------------------*/
static void Bsp_Led_Handler_ThreadController(void *argument);

// static BspLedHandlerStatus_t Bsp_Led_Handler_CriticalEnter(void);
// static BspLedHandlerStatus_t Bsp_Led_Handler_CriticalExit(void);

/*--------- Private function prototypes --------------------------------------*/



/**
 * @brief BSP 层 LED 模块 Handler 下 LED 闪烁策略控制方法
 * 
 * @param [in]  self    LED 设备驱动
 * @return      模块调用结果或错误状态。      
 * @details
 * @note    这里的led 控制仍然属于操作策略层面，所以不下放到 driver 层进行操作
 *          
 *              但是实际上可以使用更加贴合实际的策略模型，即动画模型，K 帧，按照动作起始点、结束点、动作内容
 *              来精细分段控制，以实现更加丰富的效果。可以通过亮灭、间隔、亮度等的调整来实现丰富效果
 * 
 *          这个函数的参数命名和职责仍然混乱，位于handler层，但是通过driver 对象保存和读取blink参数，
 *          策略数据的所有权还没有理顺
 * 更合理的形式是：
 *          static BspLedHandlerStatus_t Bsp_Led_Handler_BlinkProcess(
                Bsp_Led_Handler_t *handler,
                BspLedDriver_t *driver,
                const BspLedHandlerEvent_t *event);
            handler：提供 Handler 的策略执行环境，例如时间/延时接口
            driver ：表示目标 LED
            event  ：保存本次 Blink 的参数
 * @warning
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_BlinkProcess(BspLedDriver_t *const self)
{
    BspLedHandlerStatus_t status = LED_STATUS_OK;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_BlinkProcess start \r\n");
#endif
    /**************** 1.Checing the input parameters **************** */
    if (NULL == self || BSP_LED_NOT_INITIALIZED == self->initialized) 
        return LED_STATUS_ERROR_NULL_POINTER;
    if (BSP_LED_NOT_INITIALIZED == self->initialized) 
        return LED_STATUS_ERROR_RESOURCE;


    /**************** 2.Checing the Resources *********************** */
    if ((self->cycle_time_ms > 1000U) || 
        ((self->proportion_on_off > LED_PROPORTION_1_1) || 
         (self->proportion_on_off < LED_PROPORTION_1_3)) ||
        (self->blink_times > 1000U))
    {
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }


    /**************** 3、do the action ****************************** */

        /* 此处是否有必要使用一层局部变量再解耦 ？ */
        /* 避免竞态，修改数据 */
        /* Options: 此处可以通过一层局部变量赋值来保存使用的数据，可以避免竞态情况下第三者对数据的修改，
                    或者使用互斥锁或者信号量来解耦，避免此处执行出现问题 */
    uint32_t                    cycle_time_local;
    uint32_t                    blink_time_local;
    BspLedProportionOnOff_t     proportion_local;
    uint32_t                    led_toggle_times;

    cycle_time_local = self->cycle_time_ms;
    blink_time_local = self->blink_times;
    proportion_local = self->proportion_on_off;

        /* calculate the time for each led to toggle times */
    led_toggle_times = 0;
    switch (proportion_local)
    {
        case LED_PROPORTION_1_3:
            led_toggle_times = cycle_time_local / 4U;
            break;
        case LED_PROPORTION_1_2:
            led_toggle_times = cycle_time_local / 3U;
            break;
        case LED_PROPORTION_1_1:
            led_toggle_times = cycle_time_local / 2U;
            break;
        default:
#ifdef DEBUG_ENABLED
            DEBUG_OUT("BspLedDriver_Blink LED_STATUS_ERROR_PARAMETER  \r\n");
#endif
            return LED_STATUS_ERROR_PARAMETER;
    }

        /* Do the Blink action */
    for (uint32_t blink_index = 0U; blink_index < blink_time_local; blink_index++)
    {
        for (uint32_t time_index = 0U; time_index < cycle_time_local; time_index++)
        {
            if (time_index < led_toggle_times) /* 这里怎么能确保 J++ 一次的时间就是一毫秒 ？ */
            {
                /* 违反职责边界！！，Handler 直接操作 driver 内部成员！！！ */
                // self->operations->BspLed_On();
                BspLedDriver_On(self);
            }
            else
            {
                /* 违反职责边界！！，Handler 直接操作 driver 内部成员！！！ */
                // self->operations->BspLed_Off();
                BspLedDriver_Off(self);
            }
        }
    }
    status = LED_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_BlinkProcess End \r\n");
#endif

    return status;
}



/**
 * @brief BSP 层 LED 模块 Handler 下 LED 闪烁策略 事件具体的参数信息控制
 * 
 * @param [in]  self                BSP LED  Handler 句柄
 * @param [in]  cycle_time_ms       周期时间
 * @param [in]  blink_times         闪烁时间
 * @param [in]  proportion_on_off   亮灭时间占比
 * @return      模块调用结果或错误状态。      
 * @details     将 LED 闪烁策略 事件信息进行检验，检验通过后调用具体的控制策略函数进行具体执行caozuo
 * @note
 *      什么时候应该使用这种前置双下划线的方式？什么程度算是内核
 * @warning
 *          待办补充：检查LED驱动设备是否注册，通过比较 pattern 值
 */
static BspLedHandlerStatus_t __Led_EventProcess(Bsp_Led_Handler_t *const   self,
                                                BspLedHandlerEvent_t led_msg )/* LED 亮灭时间占比 */
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("__Led_EventProcess \r\n");
#endif

    /**************** 1.Checing the input parameters **************** */
    if ( (NULL == self) || (BSP_LED_INITIALIZED != self->initialized)) 
        return LED_HANDLER_STATUS_ERROR_PARAMETER;


    /**************** 2.Checing the Resources *********************** */

        /* 这里的参数检查是基于业务逻辑的限幅检查，实际项目中可灵活采用 */
    if ((led_msg.cycle_time_ms > 1000U) || 
        ((led_msg.proportion_on_off > LED_PROPORTION_1_1) || (led_msg.proportion_on_off < LED_PROPORTION_1_3)) ||
        (led_msg.blink_times > 1000U) )
    {
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }
    /* 这里的校验有疏漏 */
    if ((self->instance_group == NULL) ||
        (led_msg.led_index >= self->instance_group->instance_num) ||
        (self->instance_group->instance_group[led_msg.led_index] == NULL))
    {
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }

    if( !(( LED_NOT_INITIALIZED == led_msg.led_index) || 
         ( MAX_INDEX_NUMBER > led_msg.led_index)) )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("led_msg.led_index error parameter \r\n");
#endif
        return  LED_HANDLER_STATUS_ERROR_PARAMETER;
    }

    /*  check the led driver target if registerted : compare the pattern data */
        /* TODO: */


    /*  check the led driver target if registerted  */
    
    /* 数据拷贝 */
    /* 这仍然是 Handler 修改 Driver 内部状态：如果 Blink 已经属于 Handler 策略，那么这些字段不应放在 BspLedDriver_t 中。它们应属于：
        - 队列中的 BspLedHandlerEvent_t；
        - 或 Handler 的策略状态；
        - 或一个独立的 BspLedBlinkState_t。
        Driver 只需要保存和管理设备相关状态 */
    self->instance_group->instance_group[led_msg.led_index]->cycle_time_ms     = led_msg.cycle_time_ms;
    self->instance_group->instance_group[led_msg.led_index]->blink_times       = led_msg.blink_times;
    self->instance_group->instance_group[led_msg.led_index]->proportion_on_off = led_msg.proportion_on_off;
    
    /**************** 3、do the action ****************************** */
    BspLedStatus_t retval = LED_STATUS_OK;
    retval = Bsp_Led_Handler_BlinkProcess(self->instance_group->instance_group[led_msg.led_index]);
    if( LED_STATUS_OK != retval )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("BspLedDriver_Control failed \r\n");
#endif
        return  LED_STATUS_ERROR;
    }

    return  LED_STATUS_OK;

}
#if 0

/**
 * @brief LED 模块函数说明。
 *
 * @return      模块调用结果或错误状态。模块调用结果或错误状态。`r`n *
 * @details
 * @note
 *      这个函数应该放在适配层吗？
 * @warning
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_CriticalExit(void)
{
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_CriticalExit \r\n");
#endif
    /* TODO: 真实逻辑填充*/
    taskEXIT_CRITICAL();

    return status;
}

/**
 * @brief LED 模块函数说明。
 *
 * @return      模块调用结果或错误状态。模块调用结果或错误状态。`r`n *
 * @details
 * @note
 * @warning
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_CriticalEnter(void)
{
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_CriticalEnter \r\n");
#endif
    /* TODO: 真是逻辑填充*/
    taskENTER_CRITICAL();



    return status;
}

#endif

/**
 * @brief BSP LED Handler对象内部 LED 设备驱动数组初始化
 *
 * @param [in]  instance_group  LED 设备驱动数组
 * @param [in]  instance_num    LED 设备驱动标识号
 * @return      初始化操作状态
 * @details
 * @note    这个函数有待进行逻辑检查，初始化方式
 * @warning
 *          初始化方式有缺陷，有待优化
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_InitializeArray(BspLedDriver_t **const instance_group,
                                                           uint32_t instance_num)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_InitializeArray \r\n");
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
 * @brief   BSP LED Handler对象 将 LED 设备驱动注册至 Handler 内部 LED 设备驱动数组中         
 *
 * @param [in]  self        LED Handler对象
 * @param [in]  led_driver  LED 驱动对象
 * @return      注册结果状态
 * @details
 * @note
 * @warning
 *          适配的临界区函数没有进行检查是否为空指针
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_Register(Bsp_Led_Handler_t *const self,
                                                    BspLedDriver_t *const led_driver,
                                                    BspLedHandlerIndex_t *const led_index)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_Register \r\n");
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
    // TODO：临界区函数没有检查
self->critical_region.Bsp_Led_Handler_CriticalEnter();      // Bsp_Led_Handler_CriticalEnter();

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
self->critical_region.Bsp_Led_Handler_CriticalExit();       // Bsp_Led_Handler_CriticalExit();

    return LED_HANDLER_STATUS_OK;

}


/**
 * @brief BSP LED Handler对象 运行操作指令，将操作指令入队，由具体的 LED 执行线程进行非阻塞执行
 *
 * @param [in]  self                LED Handler对象
 * @param [in]  cycle_time_ms       LED 闪烁周期时间
 * @param [in]  blink_times         LED 闪烁时间
 * @param [in]  proportion_on_off   LED 闪烁亮灭时间占比
 * @return      
 * @details     将操作指令入队
 * @note        这一层函数其实还可以抽象或者合并
 * @warning
 */
static BspLedHandlerStatus_t Bsp_Led_Handler_Control(Bsp_Led_Handler_t *const self,
                                                   uint32_t cycle_time_ms,
                                                   uint32_t blink_times,
                                                   BspLedProportionOnOff_t proportion_on_off,
                                                   BspLedHandlerIndex_t   led_index)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_Control \r\n");
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
        .proportion_on_off = proportion_on_off,
        .led_index         = led_index          /* 这里的 设备驱动序列如何传进来？难道要修改函数定义吗？ */
    };
    /* 将操作指令入队 */
    status = self->os_queue->Bsp_Led_Handler_PutQueue(self->queue_handle, &led_event, 0);
    if( LED_HANDLER_STATUS_OK != status )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("Bsp_Led_Handler_PutQueue failed \r\n");
#endif
    }


    return status;

}

#if 1



/**
 * @brief  LED Handler inst 实例化
 * 
 * @param [in]  handler_self    LED Handler 对象
 * @param [in]  time_base_ms    时基函数接口
 * @param [in]  os_delay        延时函数接口
 * @param [in]  os_queue        队列函数接口
 * @param [in]  os_thread       线程函数接口
 * @return      模块调用结果或错误状态。模块调用结果或错误状态。
 * 
 * @details
 * @note
 * 1、    handler_self->Bsp_Led_Handler_Control = Bsp_Led_Handler_Control;
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
            有潜在竞态风险，创建完线程之后，可能不会继续向下执行，会被任务抢占，导致标志位没有写入
 */
BspLedHandlerStatus_t Bsp_Led_Handler_Instantiate(Bsp_Led_Handler_t *const handler_self,
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
        handler_self->Bsp_Led_Handler_Control                       = Bsp_Led_Handler_Control;
        handler_self->Bsp_Led_Handler_RegisterDriver                = Bsp_Led_Handler_Register;

        /* 这些接口是否应该由适配层实现？这些是需要接口适配的，但是放在 handler 的init 函数内部似乎有点职责不清晰 */
        // handler_self->critical_region.Bsp_Led_Handler_CriticalEnter = Bsp_Led_Handler_CriticalEnter; /* 这里的临界区操作应该只是 对于该模块handler 来说，需要应用到其他模块吗？ */
        // handler_self->critical_region.Bsp_Led_Handler_CriticalExit  = Bsp_Led_Handler_CriticalExit;

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
        status = Bsp_Led_Handler_InitializeArray(handler_self->instance_group->instance_group,
                                               BSP_LED_HANDLER_MAX_INSTANCE_NUM);
    if (LED_HANDLER_STATUS_OK != status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" Bsp_Led_Handler_InitializeArray Init failed \r\n");
#endif
        handler_self->os_delay     = NULL;
        handler_self->time_base_ms = NULL;
        return status;
    }

        /* ******* Init the variables will be used ******* */
    status = handler_self->os_queue->Bsp_Led_Handler_CreateQueue(5, sizeof(BspLedHandlerEvent_t), &handler_self->queue_handle);
    if( LED_HANDLER_STATUS_OK != status )
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" Bsp_Led_Handler_CreateQueue failed \r\n");
#endif
        handler_self->os_queue->Bsp_Led_Handler_DestroyQueue(&handler_self->queue_handle);
        return LED_HANDLER_STATUS_ERROR;
    }

    status = handler_self->os_thread->Bsp_Led_Handler_CreateThread(Bsp_Led_Handler_ThreadController,      /* 这个函数指针应该怎么传？ 这个算是线程函数，应该和 handler 在同一层吗？或者是driver？*/
                                                                "Bsp_Led_Handler_ThreadController",
                                                                128U,
                                                                handler_self,
                                                                NULL,
                                                                &handler_self->thread_handle );
    if( LED_HANDLER_STATUS_OK   != status )  /* pdPASS != status  */
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT(" Bsp_Led_Handler_CreateThread failed \r\n");
#endif
        handler_self->os_thread->Bsp_Led_Handler_DestroyThread(handler_self->thread_handle);
        return LED_HANDLER_STATUS_ERROR;
    }



    /**************************************************************** */

    /* 实例化确定成功后，设置标志位，这里有潜在竞态风险 */
    handler_self->initialized = BSP_LED_INITIALIZED;
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_Instantiate finished  \r\n");
#endif

    return LED_HANDLER_STATUS_OK;

}


#endif


/**
 * @brief  BSP LED Handler 对象具体的 led 控制操作执行线程
 * 
 * @param [in]  argument 相关要使用的参数传入
 * 
 * @details
 * @note
 * @warning
 */
static void Bsp_Led_Handler_ThreadController(void *argument)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("Bsp_Led_Handler_ThreadController \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 

    /**************** 2.Checing the Resources *********************** */
    Bsp_Led_Handler_t *handler_self = (Bsp_Led_Handler_t *)argument ;
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
        .proportion_on_off = LED_PROPORTION_INVALID,
        .led_index         = LED_NOT_INITIALIZED
    };

    /**************** 3、do the action ****************************** */
    for(;;)
    {
        /******************** receive the data of led event ******************************* */
        status = handler_self->os_queue->Bsp_Led_Handler_PopQueue(handler_self->queue_handle, &led_event, portMAX_DELAY);
        if( LED_HANDLER_STATUS_OK != status )
        {
#ifdef DEBUG_ENABLED
            DEBUG_OUT("Bsp_Led_Handler_PopQueue failed \r\n");
#endif
            continue;  /* 本轮没有新事件，不能处理旧 led_event */
        }

        /** 
         * @note
         *      这里的逻辑不对
         *      1、为什么在一直接收失败？
         *      3、一直在循环入队出队？但是为什么出队失败？
         * 
         *      因为发送只调用了一次，但是在一直接收，接收不到数据就会报错
         * 
         * 这里使用了原有的执行逻辑，但是实际的执行逻辑可能识别的业务
         */
        /* 这里应该执行具体的 LED 逻辑，而不再是控制函数，因为已经将数据发送到队列中了 */
        // handler_self->instance_group->instance_group[led_event.led_index]->BspLedDriver_Control(  
        //                                                                         handler_self->instance_group->instance_group[led_event.led_index],
        //                                                                         led_event.cycle_time_ms,
        //                                                                         led_event.blink_times,
        //                                                                         led_event.proportion_on_off);
        
        /******************** Execute the event of the led  *********************************** */
			__Led_EventProcess(handler_self,led_event);
	



        /* TBD : delay 延时函数 */
    }

}
