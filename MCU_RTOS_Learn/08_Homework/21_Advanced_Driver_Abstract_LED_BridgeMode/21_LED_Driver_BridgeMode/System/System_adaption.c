/**
 * 
 * @file   System_adaption.c
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-10-03
 * @version 1.0
 * 
 * @par 说明
 *      集成的头文件资源  
 * * 
 * @note 注意事项
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */

/* *********************************** Includes ***************************** */

#include <stdint.h>
#include <stdio.h>

#include "cmsis_os.h"

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



#include "System_adaption.h"


// #include "BSP_Led_Driver.h"
// #include "BSP_LED_Handler.h"


/* *********************************** Defines ****************************** */
/* 分散加载 */
__attribute__((used, section("bsp_target")))
static BspLedHandler_t bsp_led_handler_instance = {0};

__attribute__((used, section("bsp_target")))
static BspLedDriver_t led_1 = {0};



/* ******************************* Declaration ****************************** */




/* 为什么这几个函数不用static修饰， */
static BspLedStatus_t BspLedDemo_On(void)
{
    printf("led is on \r\n");
    //TODO：
    return LED_STATUS_OK;
}
static BspLedStatus_t BspLedDemo_Off(void)
{
    printf("led is off \r\n");
    //TODO：
    return LED_STATUS_OK;
}

/* 这两个函数需要进行声明吗？ */
static BspLedOperations_t g_bsp_led_demo_operations = {
    .BspLed_On = BspLedDemo_On,
    .BspLed_Off = BspLedDemo_Off,
};

static BspLedStatus_t BspLedDemo_GetTimeMs(uint32_t *const time_stamp)
{
    printf("get time now time zero \r\n");
    *time_stamp = 0U;
    //TODO：
	return LED_STATUS_OK;
}
/* 时基函数怎么抽象？ */
static BspLedTimeBaseMs_t g_bsp_led_demo_time_base_ms = {
    .BspLed_GetTimeMs = BspLedDemo_GetTimeMs
};

static BspLedStatus_t BspLedDemo_DelayMs(uint32_t delay_time)
{
    //TODO：
    printf("pf_OS_Delay_ms 1 ms\r\n");
    // (void)delay_time;
    // for (int32_t delay_index = 500; delay_index > 0; --delay_index)
    // {
    // }
    vTaskDelay(delay_time);
    printf("finished 1 ms \r\n");
    return LED_STATUS_OK;
}

static BspLedOsDelay_t g_bsp_led_demo_os_delay = {
    .BspLed_DelayMs = BspLedDemo_DelayMs,
};

static BspLedHandlerStatus_t BspLedDemo_CreateThread(void *create_task,
                                                     const char *const thread_name,
                                                     uint16_t stack_depth,
                                                     void *const parameters,
                                                     uint16_t thread_priority,
                                                     void **thread_handle)
{
    BaseType_t result;

    if ((create_task == NULL) || (thread_name == NULL) ||
        (thread_handle == NULL))
    {
        return LED_HANDLER_STATUS_ERROR_PARAMETER;
    }

    result = xTaskCreate((TaskFunction_t)create_task,
                         thread_name,
                         (configSTACK_DEPTH_TYPE)stack_depth,
                         parameters,
                         (UBaseType_t)thread_priority,
                         (TaskHandle_t *)thread_handle);

    return (result == pdPASS)
        ? LED_HANDLER_STATUS_OK
        : LED_HANDLER_STATUS_ERROR_NO_MEMORY;
}

static BspLedHandlerStatus_t BspLedDemo_DestroyThread( void *thread_handle)
{
    if (thread_handle == NULL)
    {
        return LED_HANDLER_STATUS_ERROR_NULL_POINTER;
    }

    vTaskDelete((TaskHandle_t)thread_handle);
    return LED_HANDLER_STATUS_OK;
}



/* ---------------------- Unity Test ---------------------------------------*/
// #ifdef BSP_LED_DEBUG_UNITY_TEST

// TBD: Handler OS queue functions.

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
static BspLedHandlerStatus_t BspLedDemo_QueueCreate(uint32_t item_num,
                                                     uint32_t item_size,
                                                     void **queue_handle)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueueCreate \r\n");
#endif
//    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
    /* TBD：填充接口 */
        // (void)item_num;
        // (void)item_size;
        // (void)queue_handle;
    // *queue_handle = xQueueCreate(item_num, item_size);
    /* *************************** 1、checking input parameters ******************** */
    if( (item_num <= 0) || ( item_size <= 0))
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("xQueueCreate error parameters \r\n");
#endif   
        return LED_HANDLER_STATUS_ERROR_PARAMETER ;
    }


    /* *************************** 2、Do the action ******************************** */
    *queue_handle = xQueueGenericCreate(item_num, item_size, queueQUEUE_TYPE_BASE);
    if(NULL == *queue_handle)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("xQueueCreate failed \r\n");
#endif   
        return LED_HANDLER_STATUS_ERROR_NULL_POINTER ;
    }


    return LED_HANDLER_STATUS_OK;
}

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
static BspLedHandlerStatus_t BspLedDemo_QueueDestroy(void **queue_handle)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueueDestroy \r\n");
#endif
//    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
    /* *************************** 1、checking input parameters ******************** */
    // (void)queue_handle;
    if ((queue_handle == NULL) || (*queue_handle == NULL))
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("QueueDestroy error parameters \r\n");
#endif  
        return LED_HANDLER_STATUS_ERROR_NULL_POINTER;
    }

    /* *************************** 2、Do the action ******************************** */
    vQueueDelete((QueueHandle_t)*queue_handle);
    /* 释放后句柄应该指向 NULL 对吗？ */
    /* 这里是否需要检查 delete 是否成功？ */
    *queue_handle = NULL;
    /* 队列销毁之后，系统只是去特定地址销毁内存是吗？但是句柄中的数据地址还是在的？所以需要
        专门设置为 NULL， 否则里面存储的还是原本的数据 */

    return LED_HANDLER_STATUS_OK;
}

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
static BspLedHandlerStatus_t BspLedDemo_QueuePut(void *const queue_handle,
                                                  void *const queue_item,
                                                  uint32_t timeout)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueuePut \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
    /* *************************** 1、checking input parameters ******************** */
    if( (NULL == queue_handle) || ( NULL == queue_item))
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("xQueueCreate error parameters \r\n");
#endif   
        return LED_HANDLER_STATUS_ERROR_PARAMETER ;
    }


    /* *************************** 2、Do the action ******************************** */
    /* TBD：填充接口 */
    status = xQueueSendToBack((QueueHandle_t)queue_handle, queue_item, timeout);
    if ( pdFALSE == status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("BspLedDemo_QueuePut QueueSendToBack failed \r\n");
#endif 
        return LED_HANDLER_STATUS_ERROR;
    }
    return LED_HANDLER_STATUS_OK;
}

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
static BspLedHandlerStatus_t BspLedDemo_QueuePop(void *const queue_handle,
                                                  void *const     queue_item,
                                                  uint32_t   timeout)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueuePop \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
    /* *************************** 1、checking input parameters ******************** */
    if( (NULL == queue_handle) || ( NULL == queue_item))
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("xQueueCreate error parameters \r\n");
#endif   
        return LED_HANDLER_STATUS_ERROR_PARAMETER ;
    }

    /* *************************** 2、Do the action ******************************** */
    /* TBD：填充接口 */

    /* 这里的实现是否正确 ？将数据存放在一个临时的机构体中，传递指针给外面，应该用二级指针才对吧？ */
    status = xQueueReceive((QueueHandle_t)queue_handle, queue_item, timeout);
    if ( pdFALSE == status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("BspLedDemo_QueuePop QueueReceive failed \r\n");
#endif 
        return LED_HANDLER_STATUS_ERROR;
    }

    return LED_HANDLER_STATUS_OK;
}


static BspLedHandlerTimeBaseMs_t g_bsp_led_handler_demo_time_base_ms = {
    .BspLedHandler_GetTimeMs = BspLedDemo_GetTimeMs,
};


static BspLedHandlerOsDelay_t g_bsp_led_handler_demo_os_delay = {
    .BspLedHandler_DelayMs = BspLedDemo_DelayMs
};




static BspLedHandlerOsQueue_t g_bsp_led_handler_demo_os_queue = {
    .BspLedHandler_CreateQueue  = BspLedDemo_QueueCreate,
    .BspLedHandler_DestroyQueue = BspLedDemo_QueueDestroy,
    .BspLedHandler_PopQueue     = BspLedDemo_QueuePop,
    .BspLedHandler_PutQueue     = BspLedDemo_QueuePut
};

static BspLedHandlerOsThread_t g_bsp_led_handler_demo_os_thread = {
    .BspLedHandler_CreateThread  = BspLedDemo_CreateThread ,// xTaskCreate,
    .BspLedHandler_DestroyThread = BspLedDemo_DestroyThread ,// vTaskDelete
};



// /**
//  * @brief  LED Handler inst 实例化
//  *
//  * @param [in]  handler_self
//  * @param [in]  time_base_ms
//  * @param [in]  os_delay
//  * @return
//  *
//  * @details
//  * @note
//  * 1、    handler_self->BspLedHandler_Control = BspLedHandler_Control;
//  *              这里的控制策略其实是绑定的，那么对于不同的 LED 来说，其实是同样的控制策略
//                     但是不同的LED应该有不同的控制策略，所以这里不应该绑定，应该由 各自的LED
//                     初始化的时候，绑定不同的控制策略，即将具体的 LED 控制函数指针绑定下放
//                     而不是在 Handler 层绑定
//                 但是如果将控制策略升级到 handler 层中，那么对于不同的控制策略也需要调整，
//                 如果只是一种闪烁模式，通过调整参数来达到不同的效果，那么其实还是一种模式的抽象？
//                 应该是包含不同的闪烁模式，例如：呼吸灯、固定间隔闪烁、渐亮、渐灭等，都是不同的控制策略
//  *              那么就应该升级到 handler 层进行使用和适配
//     2、handler_self->os_queue = os_queue;
//             这里的 OS_Queue 在 inst 函数内部绑定具体的接口和在外部绑定效果相同吗？外部已经绑定好接口，内部在进行绑定可以吗？
//  * @warning
//  */
// BspLedHandlerStatus_t BspLedHandler_Instantiate(BspLedHandler_t *const handler_self,
//                                                  BspLedHandlerTimeBaseMs_t *const time_base_ms,
// #ifdef OS_SUPPORTING
//                                                  BspLedHandlerOsDelay_t *const os_delay,
//                                                  BspLedHandlerOsQueue_t *const os_queue,
//                                                  BspLedHandlerOsThread_t *     os_thread
// #endif /* OS_SUPPORTING */
// )
// {
// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("led handler inst kick-off \r\n");
// #endif
//     BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; /* 驱动初始化结果状态 */
//     /**************** 1.Checing the input parameters **************** */

//     if ((NULL == handler_self) || (NULL == time_base_ms)) return LED_HANDLER_STATUS_ERROR_PARAMETER;
// #ifdef OS_SUPPORTING
//     if ((NULL == os_delay) || (NULL == os_queue) ||( NULL == os_thread)) 
//         return LED_HANDLER_STATUS_ERROR_PARAMETER;
// #endif /* OS_SUPPORTING */

//     /**************** 2.Checing the Resources *********************** */
//     if (BSP_LED_INITIALIZED == handler_self->initialized)
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT("LED_HANDLER_STATUS_ERROR_RESOURCE");
// #endif
//         return LED_HANDLER_STATUS_ERROR_RESOURCE;
//     }

// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("led handler inst start \r\n");
// #endif

//     /**************** 3.Adding the interface  *********************** */
//         /* mount the external interface  */
//         handler_self->time_base_ms = time_base_ms;
//         handler_self->os_delay     = os_delay;
//         handler_self->os_queue     = os_queue;
//         handler_self->os_thread    = os_thread;

//         /* mount the internal interface  */
//         handler_self->BspLedHandler_Control                       = BspLedHandler_Control;
//         handler_self->BspLedHandler_RegisterDriver                = BspLedHandler_Register;
//         handler_self->critical_region.BspLedHandler_CriticalEnter = BspLedHandler_CriticalEnter; /* 这里的临界区操作应该只是 对于该模块handler 来说，需要应用到其他模块吗？ */
//         handler_self->critical_region.BspLedHandler_CriticalExit  = BspLedHandler_CriticalExit;

//             // handler_self->instance_group->instance_num = 0;
//             // handler_self->instance_group->instance_group = NULL ;
//             /*  这里 是否要进行赋值？ */
//     /**************** 4. Init the target  *************************** */
//     /* 初始化创建对象，避免数组溢出越界，但是看案例没有这一步啊
//         这里的静态对象传递给非静态，是否会有问题？ */
//     static BspLedHandlerInstanceGroup_t instance_group = {0};
//     handler_self->instance_group = &instance_group;

//     // TODO :
//         /* Init the variables will be used */
//         /* 这里的初始化要不要分配空间？还是register 的时候再分配空间？ */
//         handler_self->instance_group->instance_num = 0U;
//         status = BspLedHandler_InitializeArray(handler_self->instance_group->instance_group,
//                                                BSP_LED_HANDLER_MAX_INSTANCE_NUM);
//     if (LED_HANDLER_STATUS_OK != status)
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT(" BspLedHandler_InitializeArray Init failed \r\n");
// #endif
//         handler_self->os_delay     = NULL;
//         handler_self->time_base_ms = NULL;
//         return status;
//     }

//         /* Init the variables will be used */
//     status = handler_self->os_queue->BspLedHandler_CreateQueue(5, sizeof(BspLedHandlerEvent_t), &handler_self->queue_handle);
//     if( LED_HANDLER_STATUS_OK != status )
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT(" BspLedHandler_CreateQueue failed \r\n");
// #endif
//         handler_self->os_queue->BspLedHandler_DestroyQueue(&handler_self->queue_handle);
//     }

//     status = handler_self->os_thread->BspLedHandler_CreateThread(BspLedHandler_ThreadController,      /* 这个函数指针应该怎么传？ 这个算是线程函数，应该和 handler 在同一层吗？或者是driver？*/
//                                                                 "BspLedHandler_ThreadController",
//                                                                 128U,
//                                                                 handler_self,
//                                                                 NULL,
//                                                                 &handler_self->thread_handle );
//     if( 1  != status )  /* pdPASS != status  */
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT(" BspLedHandler_CreateThread failed \r\n");
// #endif
//         handler_self->os_thread->BspLedHandler_DestroyThread(handler_self->thread_handle);
//     }



//     /**************************************************************** */


//     handler_self->initialized = BSP_LED_INITIALIZED;
// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("BspLedHandler_Instantiate finished  \r\n");
// #endif

//     return status;

// }


#if 0
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
static BspLedHandlerStatus_t BspLedHandler_CriticalExit(void)
{
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK;

#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedHandler_CriticalExit \r\n");
#endif
    /* TODO: 真实逻辑填充*/


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
    /* TODO: 真是逻辑填充*/




    return status;
}

#endif

/* ************************************** Driver ************************** */





// /**
//  * @brief   Instiantiate the target of BSP_Led_Driver  实例化
//  *
//  * Steps:
//     1, Adding the Core interfaces into target of bsp_led_driver instance.
//     2, Adding the OS interfaces into target of bsp_ledd_driver instance.
//     3, Adding the timebase interfaces into targetof bsp_led_driver instance.
//  * @param[in,out] self         驱动对象实例；输入时不能为 NULL，函数会写入其接口指针、
//  *                              初始闪烁参数和初始化状态。
//  * @param[in]     operations   LED 核心操作接口，必须指向有效的 BspLedOperations_t 对象，
//  *                              并提供 BspLed_On 与 BspLed_Off 回调函数。
//  * @param[in]     time_base_ms 毫秒时基接口，必须指向有效的 BspLedTimeBaseMs_t 对象，
//  *                              并提供用于读取当前时间的 BspLed_GetTimeMs 回调函数。
//  * @param[in]     os_delay     操作系统    延时接口，必须指向有效的 BspLedOsDelay_t 对象，
//  *                              并提供毫秒延时的 BspLed_DelayMs 回调函数；仅在定义
//  *                              OS_SUPPORTING 时生效。
//  * @return      LED_STATUS_OK              实例化成功。
//  * @return      LED_STATUS_ERROR_PARAMETER self、operations、time_base_ms 或 os_delay 为 NULL。
//  * @return      LED_STATUS_ERROR_RESOURCE  self 对象已经完成实例化。
//  * @return      其他 BspLedStatus_t        驱动初始化流程返回的错误状态。
//  *
//  * @details
//  *  函数成功后，@p self 会持有上述接口对象的地址，因此这些对象在驱动使用期间
//  *  必须保持有效；函数不会复制接口对象本身。
//  * @note      调用前应将 self 指向的 BspLedDriver_t 对象清零或设置为 BSP_LED_NOT_INITIALIZED。
//  * @warning   传入的接口对象及其函数指针不得为 NULL，且生命周期必须覆盖驱动实例的使用期。
//  */
// BspLedStatus_t BspLedDriver_Instantiate(BspLedDriver_t *const self,
//                                          BspLedOperations_t *const operations, // const BspLedOperations_t *const operations,
//                                          BspLedTimeBaseMs_t *const time_base_ms, // const BspLedTimeBaseMs_t *const time_base_ms,
// #ifdef OS_SUPPORTING
//                                          BspLedOsDelay_t *const os_delay // const BspLedOsDelay_t *const os_delay
// #endif /* OS_SUPPORTING */
// )
// {
// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("led inst kick-off \r\n");
// #endif
//     BspLedStatus_t status; /* 驱动初始化结果状态 */
//     /**************** 1.Checing the input parameters **************** */

//     if ((NULL == self) || (NULL == operations) || (NULL == time_base_ms)) return LED_STATUS_ERROR_PARAMETER;
// #ifdef OS_SUPPORTING
//     if (NULL == os_delay) return LED_STATUS_ERROR_PARAMETER;
// #endif /* OS_SUPPORTING */

//     /**************** 1.Checing the Resources **************** */
//     if (BSP_LED_INITIALIZED == self->initialized)
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT("LED_STATUS_ERROR_RESOURCE");
// #endif
//         return LED_STATUS_ERROR_RESOURCE;
//     }

// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("led inst start \r\n");
// #endif

//     /**************** 1.Adding the interface  **************** */
//     /* mount the external interface  */
//     self->operations   = operations;
//     self->os_delay     = os_delay;
//     self->time_base_ms = time_base_ms; /* 这里可以内部做参数的校验 */

//     /* mount the internal interface  */
//     self->BspLedDriver_Control = BspLedDriver_Control; /*  这里赋值可以吗？因为不是从参数传递的 */

//     /**************** 1. Init the target  **************** */
//     self->blink_times       = 0U;
//     self->cycle_time_ms     = 0U;
//     self->proportion_on_off = LED_PROPORTION_INVALID;

//     status = BspLedDriver_Initialize(self);
//     if (LED_STATUS_OK != status)
//     {
// #ifdef DEBUG_ENABLED
//         DEBUG_OUT(" BspLedDriver_Initialize Init failed \r\n");
// #endif
//         self->operations   = NULL;
//         self->os_delay     = NULL;
//         self->time_base_ms = NULL;

//         return status;
//     }

//     self->initialized = BSP_LED_INITIALIZED;
// #ifdef DEBUG_ENABLED
//     DEBUG_OUT("init finished  \r\n");
// #endif
//     return status;
// }

void SystemAdaption_LedDemoInit(void){


#ifdef BSP_LED_DEBUG_UNITY_TEST

    // static BspLedHandler_t bsp_led_handler_instance = {0};
    BspLedHandler_Instantiate(&bsp_led_handler_instance,
                              &g_bsp_led_handler_demo_time_base_ms,
                              &g_bsp_led_handler_demo_os_delay,
                              &g_bsp_led_handler_demo_os_queue,
                              &g_bsp_led_handler_demo_os_thread);

    // BspLedDriver_t led_1 = {0};
    BspLedDriver_Instantiate(&led_1,
                             &g_bsp_led_demo_operations,
                             &g_bsp_led_demo_time_base_ms,
                             &g_bsp_led_demo_os_delay);
    
    BspLedHandlerEvent_t led_event = {
        .cycle_time_ms     = 6,
        .blink_times       = 4,
        .proportion_on_off = LED_PROPORTION_1_2,
        .led_index         = LED_INDEX_1,
    } ;                        

    bsp_led_handler_instance.BspLedHandler_RegisterDriver(&bsp_led_handler_instance,
                                                           &led_1,
                                                           &bsp_led_handler_instance.led_index);
    bsp_led_handler_instance.BspLedHandler_Control(&bsp_led_handler_instance, 
                                                    led_event.cycle_time_ms, 
                                                    led_event.blink_times, 
                                                    led_event.proportion_on_off,
                                                    led_event.led_index);


#endif /* BSP_LED_DEBUG_UNITY_TEST */
}




// #endif /* BSP_LED_DEBUG_UNITY_TEST */
