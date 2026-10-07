/**
 * 
 * @file   System_adaption.c
 * @brief  LED 驱动 包括 Handler 的适配层，适配上层操作系统 
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




/* *********************************** Defines ****************************** */
/* 分散加载 */
__attribute__((used, section("bsp_target")))
static Bsp_Led_Handler_t bsp_led_handler_instance = {0xACACACAC} ; // {0};

__attribute__((used, section("bsp_target")))
static BspLedDriver_t led_1 = {0xACACACAC} ; // {0};



/* ******************************* Declaration ****************************** */

#if 1

/**
 * @brief  BSP LED Handler对象 适配封装函数：退出 FreeRTOS 临界区。
 *
 * @return      
 * @details 使用 Free RTOS 的退出临界区函数
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
 * @brief  BSP LED Handler对象 适配封装函数：进入 FreeRTOS 临界区。
 *
 * @return      
 * @details 使用 Free RTOS 的进入临界区函数
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


/* 为什么这几个函数不用static修饰， */
/**
 * @brief  BSP LED Handler对象 适配封装函数
 * 
 * @return      
 * 
 * @details LED 开
 * @note
 * @warning
 */
static BspLedStatus_t BspLedDemo_On(void)
{
    printf("led is on \r\n");
    //TODO：
    return LED_STATUS_OK;
}
/**
 * @brief  BSP LED Handler对象 适配封装函数
 * 
 * @return      
 * 
 * @details LED 关
 * @note
 * @warning
 */
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

/**
 * @brief  BSP LED Handler对象 适配封装函数：获取时间
 * 
 * @param [in]  time_stamp 时间戳数据
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
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

/**
 * @brief  BSP LED Handler对象 适配封装函数：延时函数
 *  
 * @param [in]  delay_time 延时时间
 * @return      
 * 
 * @details 使用 FreeRTOS 延时实现
 * @note
 * @warning
 */
static BspLedStatus_t BspLedDemo_DelayMs(uint32_t delay_time)
{
    //TODO：
    printf("pf_OS_Delay_ms 1 ms\r\n");

    vTaskDelay(delay_time);
    return LED_STATUS_OK;
}

static BspLedOsDelay_t g_bsp_led_demo_os_delay = {
    .BspLed_DelayMs = BspLedDemo_DelayMs,
};


/**
 * @brief  BSP LED Handler对象 适配封装函数：创建线程
 * 
 * @param [in]  create_task     线程函数
 * @param [in]  thread_name     线程名称
 * @param [in]  stack_depth     线程使用栈深度
 * @param [in]  parameters      线程使用参数
 * @param [in]  thread_priority 线程优先级
 * @param [in]  thread_handle   线程句柄
 * @return      
 * 
 * @details 使用 FreeRTOS 实现
 * @note
 * @warning
 */
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

    /* *********** 具体接口 *********** */
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


/**
 * @brief  BSP LED Handler对象 适配封装函数：销毁线程
 * 
 * @param [in]  thread_handle 线程句柄
 * @return      
 * 
 * @details 使用 FreeRTOS 实现
 * @note
 * @warning
 */
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
 * @brief  BSP LED Handler对象 适配封装函数：队列创建
 * 
 * @param [in]  item_num     队列数据项容量
 * @param [in]  item_size    队列数据项大小
 * @param [in]  queue_handle 队列句柄
 * @return      模块调用结果或错误状态。      
 * @details 使用 FreeRTOS 实现
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
 * @brief  BSP LED Handler对象 适配封装函数：销毁队列
 * 
 * @param [in]  queue_handle 队列句柄
 * @return      
 * @details
 * @note
 * @warning
 */
static BspLedHandlerStatus_t BspLedDemo_QueueDestroy(void **queue_handle)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueueDestroy \r\n");
#endif
    /* *************************** 1、checking input parameters ******************** */
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
 * @brief  BSP LED Handler对象 适配封装函数：将操作数据项入队
 * 
 * @param [in]  queue_handle 队列句柄
 * @param [in]  queue_item   队列数据项
 * @param [in]  timeout      入队操作允许超时时间
 * @return      
 * @details     使用 FreeRTOS 实现
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
 * @brief  BSP LED Handler对象 适配封装函数：将操作数据项出队
 * 
 * @param [in]  queue_handle 队列句柄
 * @param [in]  queue_item   队列数据项
 * @param [in]  timeout      出队操作允许超时时间，阻塞等待时间
 * @return      
 * @details
 * @note
 *          该 入队和出队 函数还应该有中断版本，FromISR，不能复用普通 入队和出队函数
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
    .Bsp_Led_Handler_GetTimeMs = BspLedDemo_GetTimeMs,
};


static BspLedHandlerOsDelay_t g_bsp_led_handler_demo_os_delay = {
    .Bsp_Led_Handler_DelayMs = BspLedDemo_DelayMs
};




static BspLedHandlerOsQueue_t g_bsp_led_handler_demo_os_queue = {
    .Bsp_Led_Handler_CreateQueue  = BspLedDemo_QueueCreate,
    .Bsp_Led_Handler_DestroyQueue = BspLedDemo_QueueDestroy,
    .Bsp_Led_Handler_PopQueue     = BspLedDemo_QueuePop,
    .Bsp_Led_Handler_PutQueue     = BspLedDemo_QueuePut
};

/*  适配实现，将FreeRTOS 连接起来 */
static BspLedHandlerOsThread_t g_bsp_led_handler_demo_os_thread = {
    .Bsp_Led_Handler_CreateThread  = BspLedDemo_CreateThread ,// xTaskCreate,
    .Bsp_Led_Handler_DestroyThread = BspLedDemo_DestroyThread ,// vTaskDelete
};




/* ************************************** Driver ************************** */





/**
 * @brief   BSP LED Handler对象 适配封装函数：整体系统初始化
 * 
 * @return      
 * 
 * @details 
 *          LED Handler 对象实例化；LED 驱动对象实例化
 * @note
 *          待办：实例化失败后的资源清理
 * @warning
 */
SystemAdaptStatus_t SystemAdaption_LedInit(void){



#ifdef BSP_LED_DEBUG_UNITY_TEST

    BspLedHandlerStatus_t handler_init_ret = LED_HANDLER_STATUS_RESERVED;
    handler_init_ret = Bsp_Led_Handler_Instantiate(&bsp_led_handler_instance,
                                                 &g_bsp_led_handler_demo_time_base_ms,
                                                 &g_bsp_led_handler_demo_os_delay,
                                                 &g_bsp_led_handler_demo_os_queue,
                                                 &g_bsp_led_handler_demo_os_thread);
    if( LED_HANDLER_STATUS_OK != handler_init_ret)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("Bsp_Led_Handler_Instantiate init failed \r\n");
#endif
        /* TODO: clear state and data 、function pointers settings */
        /* maybe could use a function tu Deinit , all set NULL or 0xFF or 0x00 */

        return SYS_ADAPT_STATUS_ERROR;
    }
    /* 在适配层绑定 临界区函数 */
    /* 必须要先绑定 临界区函数，后面 register 函数会调用 */
    bsp_led_handler_instance.critical_region.Bsp_Led_Handler_CriticalEnter = Bsp_Led_Handler_CriticalEnter;
    bsp_led_handler_instance.critical_region.Bsp_Led_Handler_CriticalExit  = Bsp_Led_Handler_CriticalExit;

    BspLedStatus_t driver_init_ret = LED_STATUS_RESERVED;
    driver_init_ret = BspLedDriver_Instantiate( &led_1,
                                                &g_bsp_led_demo_operations,
                                                &g_bsp_led_demo_time_base_ms,
                                                &g_bsp_led_demo_os_delay);
    if( LED_STATUS_OK != driver_init_ret)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("BspLedDriver_Instantiate init failed  \r\n");
#endif
        /* TODO: clear state and data 、function pointers settings */
        /* maybe could use a function tu Deinit , all set NULL or 0xFF or 0x00 */
        

        return SYS_ADAPT_STATUS_ERROR;
    }

    /* LED 驱动初始化执行操作：闪烁 参数信息 */
    BspLedHandlerEvent_t led_event = {
        .cycle_time_ms     = 6,
        .blink_times       = 4,
        .proportion_on_off = LED_PROPORTION_1_2,
        .led_index         = LED_INDEX_1,
    } ;                        

    BspLedHandlerStatus_t register_status = LED_HANDLER_STATUS_RESERVED ;
    register_status = bsp_led_handler_instance.Bsp_Led_Handler_RegisterDriver(&bsp_led_handler_instance,
                                                                            &led_1,
                                                                            &bsp_led_handler_instance.led_index);
    if( LED_HANDLER_STATUS_OK != register_status)
    {
#ifdef DEBUG_ENABLED
        DEBUG_OUT("Bsp_Led_Handler_RegisterDriver  failed  \r\n");
#endif
        /* TODO: unregister  clear state and data 、function pointers settings */
        /* maybe could use a function tu Deinit , all set NULL or 0xFF or 0x00 */
        

        return LED_HANDLER_STATUS_ERROR;
    }     

    /* LED 驱动初始化执行操作：闪烁 */
    bsp_led_handler_instance.Bsp_Led_Handler_Control(&bsp_led_handler_instance, 
                                                    led_event.cycle_time_ms, 
                                                    led_event.blink_times, 
                                                    led_event.proportion_on_off,
                                                    led_event.led_index);


#endif /* BSP_LED_DEBUG_UNITY_TEST */

    /* 飞秒加载启动，直接从特定地址启动加载，快速初始化 */
    //Bsp_Led_Handler_t * p_Handler_1 = (Bsp_Led_Handler_t *)(0x08007000);

    

    return LED_HANDLER_STATUS_OK;
}




