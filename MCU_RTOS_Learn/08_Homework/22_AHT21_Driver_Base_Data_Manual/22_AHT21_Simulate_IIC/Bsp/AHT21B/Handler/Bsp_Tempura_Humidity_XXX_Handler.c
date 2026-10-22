/**
 * 
 * @file   Bsp_Tempura_Humidity_XXX_Handler.c
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-10-17
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
/* Private includes ---------------------------------------------------------*/
// #include "Bsp_AHT21B_Driver.h"

// #include "FreeRTOS.h"
// #include "task.h"

#include "Bsp_Tempura_Humidity_XXX_Handler.h"

#include "Bsp_AHT21B_Driver.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// #include "FreeRTOS.h"
// #include "task.h"




 /* Private define ----------------------------------------------------------*/










/* Private variables --------------------------------------------------------*/

/* queue item type: TempHumiHandler_Event_t *
   queue item size: sizeof(TempHumiHandler_Event_t *)
   send source: &event_ptr
   receive target: &event_ptr */
static void *g_temp_humi_handler_event_queue = NULL;



static TempHumiData_t g_temp_humi_snapshot;

static Bsp_Aht21b_Driver_t  g_sensor_aht21b  ;

/* 用于适配 aht21b  */
static BspAht21b_TimeBaseInterface_t g_aht21b_time_base;
static BspAht21b_RTOS_YieldInterface_t g_aht21b_yield;

// BspAht21b_TimeBaseInterface_t
/* Private typedef ---------------------------------------------------------*/

/* @note 这个结构体其实可以设计为包含一系列有关Handler私有数据的结构体，
        而不仅仅是 inited 的标志位 */
/* @note bool 类型的变量定义后未初始化时，值是多少？ */

typedef struct TempHumiHandlerInitedFlag
{
    bool temp_humi_handler_inited_flag ;
}TempHumiHandlerInitedFlag_t;

typedef struct TempHumiHandlerSensorType
{
#ifdef SENSOR_SUPPORT_AHT21B
    Bsp_Aht21b_Driver_t * sensor_aht21b;
#endif

}TempHumiHandlerSensorType_t;


// typedef IIC_DriverInterface_t TempHumiHandler_ReplaceIICInterface;



/* Private function prototypes ----------------------------------------------*/


void *TempHumiHandler_GetEventQueueHandle(void)
{
    return g_temp_humi_handler_event_queue;
}


static void TempHumiHandler_PublishData(
    float temperature,
    float humidity,
    uint32_t timestamp)
{
#ifdef FREERTOS_SUPPORTING
    taskENTER_CRITICAL();
#endif /* FREERTOS_SUPPORTING */
    g_temp_humi_snapshot.temperature = temperature;
    g_temp_humi_snapshot.humidity    = humidity;
    g_temp_humi_snapshot.timestamp   = timestamp;
    g_temp_humi_snapshot.valid       = true;
#ifdef FREERTOS_SUPPORTING
    taskEXIT_CRITICAL();
#endif /* FREERTOS_SUPPORTING */
}







static TempHumiHandlerStatus_t TempHumiHandler_DispatchEvent(TempHumiHandler_Event_t *event)
{
    if (event == NULL)
    {
        return HANDLER_ERROR_NULL_POINTER;
    }

    if (event->pf_EventCallBack == NULL)
    {
        return HANDLER_ERROR_RESOURCE;
    }

    switch (event->TempHumi_event_type)
    {
        case TempHumi_EVENT_Temp:
        case TempHumi_EVENT_Humi:
        case TempHumi_EVENT_1:
        case TempHumi_EVENT_2:
        case TempHumi_EVENT_3:
        case TempHumi_EVENT_4:
            event->pf_EventCallBack(
                event->temp_humi_sensor);
            return HANDLER_OK;

        default:
            return HANDLER_ERROR_PARAMETER;
    }
}

/**
 * @brief  温湿度处理 Handler 的事件发送函数
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  temp_humi_event 
 * @return      
 * 
 * @details
 * @note
 *      是否要对 队列 资源添加标志位，用于检查资源是否初始化（即创建）或者说是基本状态？
 *      该函数可以暂时搁置，因为由外部进行发送事件shuju
 * @warning
 */
static TempHumiHandlerStatus_t TempHumiHandler_SendTempHumiEvent(
                                    TempHumi_Handler_t * temp_humi_handler,
                                    TempHumiHandler_Event_t* temp_humi_event)
{
    /* 1 check the input parameters and resources-------------------------- */
    if( (NULL == temp_humi_handler) ||
        (NULL == temp_humi_event)   )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Init ERROR resources \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }

    /* 检查是否初始化 */
    if( false == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Inst is NOT inited \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }

    
    /* 2  send the event to the queue -------------------------- */
    if( HANDLER_OK != temp_humi_handler->os_queue->TempHumiHandler_PutQueue(
                        // temp_humi_handler->event_queue_handle,
                        g_temp_humi_handler_event_queue,
                        &temp_humi_event,
                        0) )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_PutQueue is Put failed \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR;
    }


    return HANDLER_OK;
}


/**
 * @brief  温湿度处理 Handler 的事件接收函数
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  temp_humi_event 
 * @return      
 * 
 * @details
 * @note
 *      这里出参因为要传出 TempHumiHandler_Event_t* 对象指针，是否要使用二级指针？
 *          使用二级指针
 * @warning
 */
static TempHumiHandlerStatus_t TempHumiHandler_ReceiveTempHumiEvent(
                                    TempHumi_Handler_t * temp_humi_handler,
                                    TempHumiHandler_Event_t** temp_humi_event)
{
    /* 1 check the input parameters and resources-------------------------- */
    if( (NULL == temp_humi_handler) ||
        (NULL == temp_humi_event)   )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Init ERROR resources \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }

    /* 检查是否初始化 */
    if( false == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
    {
#ifdef HANDLER_DEBUG
        printf(" temp_humi_handler is NOT inited \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }
    /* 2  Receive the event to the queue then fill -------------------------- */
    /* @note 这里的数据出队方案还可以优化一下，针对不同的返回结果进行区分，进项分别处理 */
    if( HANDLER_OK != temp_humi_handler->os_queue->TempHumiHandler_PopQueue(
                        // temp_humi_handler->event_queue_handle,
                        g_temp_humi_handler_event_queue,
                        temp_humi_event,
                        100) )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_PopQueue is Pop failed \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR;
    }

    return HANDLER_OK;
}




/**
 * @brief  温湿度处理 Handler 的初始化函数
 * 
 * @param [in]  temp_humi_handler 
 * @return      
 * 
 * @details
 *      检查资源和参数
 *      创建事件通信队列
 * @note
 *      这个要初始化什么东西呢？
 * @warning
 */
static TempHumiHandlerStatus_t TempHumiHandler_Init(TempHumi_Handler_t * temp_humi_handler)
{
    /* 1 check the input parameters and resources-------------------------- */
    if( NULL == temp_humi_handler)
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Init ERROR resources \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }

    /* 避免重复初始化 */
    if( true == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Inst is inited \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }
    /* 2  execute the init contents -------------------------- */
    temp_humi_handler->temp_humi_data_valid = false;
    /* @note 这里应该检查函数返回值还是检查队列句柄？ */
    TempHumiHandlerStatus_t create_retval = HANDLER_RESERVED;
    create_retval = temp_humi_handler->os_queue->TempHumiHandler_CreateQueue(
                                                                                10, 
                                                    sizeof(TempHumiHandler_Event_t*),
                                            // &temp_humi_handler->event_queue_handle
                                                    &g_temp_humi_handler_event_queue);
    if( HANDLER_OK != create_retval)
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Init CreateQueue ERROR Failed \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR;
    }

    /* 3 check sensor type then mount it ------------------ */
    /* TODO : 识别出传感器实例的具体型号，并对其进行实例化 */


    /* 适配 aht21b  */
    g_aht21b_time_base.pf_GetTickCount =
        temp_humi_handler->time_base_interface->pf_GetTickCount;

    g_aht21b_yield.pf_RTOS_Yield =
        temp_humi_handler->yield_interface->pf_RTOS_Yield;


#ifdef SENSOR_SUPPORT_AHT21B
    BspAht21bStatus_t inst_retval = AHT21B_STATUS_RESERVED;
    inst_retval = pAht21b_DriverInst(   &g_sensor_aht21b, 
                                        temp_humi_handler->iic_handler_interface,
                                        &g_aht21b_time_base,
                                        &g_aht21b_yield );
    if (inst_retval != AHT21B_STATUS_OK)
    {
        temp_humi_handler->os_queue->TempHumiHandler_DestroyQueue(
                                                &g_temp_humi_handler_event_queue);

        g_temp_humi_handler_event_queue = NULL;

        return HANDLER_ERROR;
    }
#endif /* SENSOR_SUPPORT_AHT21B */


    /* TODO：绑定到 temp_humi_handler.sensor_device.sensor_aht21b  */
    temp_humi_handler->sensor_device->sensor_aht21b = &g_sensor_aht21b;

    inst_retval = g_sensor_aht21b.pfAht21b_Init(&g_sensor_aht21b);
    if (inst_retval != AHT21B_STATUS_OK)
    {
        /* TODO : 销毁资源 */

        return HANDLER_ERROR;
    }

    /* change the init flag  */
    temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag = true;
    return HANDLER_OK;
}

/**
 * @brief  外部获取最新 温湿度 数据
 * 
 * @param [in]  data 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
TempHumiHandlerStatus_t TempHumiHandler_GetLatestData(
    TempHumiData_t *data)
{
    if (data == NULL)
    {
        return HANDLER_ERROR_NULL_POINTER;
    }

    /* @note 这里怎么解耦？和FreeRTOS 解耦？ */
#ifdef FREERTOS_SUPPORTING
    taskENTER_CRITICAL();
#endif /* FREERTOS_SUPPORTING */

    if (!g_temp_humi_snapshot.valid)
    {
#ifdef FREERTOS_SUPPORTING
        taskEXIT_CRITICAL();
#endif /* FREERTOS_SUPPORTING */

        return HANDLER_ERROR_RESOURCE;
    }

    *data = g_temp_humi_snapshot;
#ifdef FREERTOS_SUPPORTING
    taskEXIT_CRITICAL();
#endif /* FREERTOS_SUPPORTING */

    return HANDLER_OK;
}

/**
 * @brief  获取温湿度数据函数
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  temp 
 * @param [in]  humi 
 * @return      
 * 
 * @details     直接从handler 返回数据
 * @note
 * @warning
 */
// TempHumiHandlerStatus_t TempHumiHandler_GetTempHumiData(
//                                     TempHumi_Handler_t * temp_humi_handler,
//                                     TempHumiHandler_Event_t * event,
//                                     float * temp,
//                                     float * humi )
// {
//     /* 1 check the input parameters and resources-------------------------- */
//     if( (NULL == temp_humi_handler) ||
//         (NULL == temp) ||
//         (NULL == humi) ||
//         (NULL == event) ) 
//     {
// #ifdef HANDLER_DEBUG
//         printf(" TempHumiHandler_GetTempHumiData ERROR resources \r\n");
// #endif /* HANDLER_DEBUG */
//         return  HANDLER_ERROR_RESOURCE;
//     }

//     /* 检查是否初始化 */
//     if( false == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
//     {
// #ifdef HANDLER_DEBUG
//         printf(" temp_humi_handler is NOT inited \r\n");
// #endif /* HANDLER_DEBUG */
//         return  HANDLER_ERROR_RESOURCE;
//     }

//     if( false == temp_humi_handler->temp_humi_data_valid )
//     {
// #ifdef HANDLER_DEBUG
//         printf(" temp_humi_data is Invalid ! \r\n");
// #endif /* HANDLER_DEBUG */
//         return  HANDLER_ERROR_RESOURCE;
//     }

//     /* 2 return the tempurature and humiditity data -------------------------- */

//     // *temp = temp_humi_handler->tempurature;
//     // *humi = temp_humi_handler->humiditity;

//     switch (event->TempHumi_event_type)
//     {
//         case TempHumi_EVENT_Temp:

//             break;

//         case TempHumi_EVENT_Humi:
//                 event->pf_EventCallBack(temp_humi_sensor);
//             break;

//         case TempHumi_EVENT_1:
//                 event->pf_EventCallBack(temp_humi_sensor);
//             break;

//         case TempHumi_EVENT_2:
//                 event->pf_EventCallBack(temp_humi_sensor);
//             break;

//         case TempHumi_EVENT_3:
//                 event->pf_EventCallBack(temp_humi_sensor);
//             break;

//         case TempHumi_EVENT_4:
//                 event->pf_EventCallBack(temp_humi_sensor);
//             break;
        
//         default:
//             break;
//     }

//     return HANDLER_OK ;

// }



/**
 * @brief  定时更新数据
 * 
 * @param [in]  temp_humi_handler 
 * 
 * @param [out]  temp 
 * @param [out]  humi 
 * @return      
 * 
 * @details     将最新的数据返回到 Handler 中
 * @note    如果这是一个公共的独立函数，但是在线程中被固定调用，那么就需要考虑竞态问题，重入；
            不如直接做成内部函数，返回数据有另一个函数进行，该函数只负责刷新数据，最新数据直接保存在handler中
            另一个函数直接从handler 中获取数据
 * @warning
 */
static TempHumiHandlerStatus_t TempHumiHandler_UpdateTempHumiData(
                                    TempHumi_Handler_t * temp_humi_handler,
                                    float * temp,
                                    float * humi )
{
/* @note 这里其实已经没有必要再进行参数和资源检查了，因为前面的执行流程都已经检查过了，如果有问题，前面就会截断
        不对，这是一个公共的独立函数，应该自己保证入口契约 */

    float temperature;
    float humidity;

    /* 1 check the input parameters and resources-------------------------- */
//     if( (NULL == temp_humi_handler) ||
//         (NULL == temp) ||
//         (NULL == humi)  )
//     {
// #ifdef HANDLER_DEBUG
//         printf(" TempHumiHandler_GetTempHumiData ERROR resources \r\n");
// #endif /* HANDLER_DEBUG */
//         return  HANDLER_ERROR_RESOURCE;
//     }

//     /* 检查是否初始化 */
//     if( false == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
//     {
// #ifdef HANDLER_DEBUG
//         printf(" temp_humi_handler is NOT inited \r\n");
// #endif /* HANDLER_DEBUG */
//         return  HANDLER_ERROR_RESOURCE;
//     }


    /* 2  Update last data tick -------------------------- */
    uint32_t tim = temp_humi_handler->time_base_interface->pf_GetTickCount();

    /* @note 将数据有效性和 数据是否过期绑定在一起，感觉有点畸形啊，好奇怪 */
    if( !temp_humi_handler->temp_humi_data_valid ||
        ((tim - temp_humi_handler->last_temp_tick) >= DATA_TEMP_HUMI_LIFETIME)   )
    {
        BspAht21bStatus_t ret = temp_humi_handler->sensor_device->sensor_aht21b->pfAht21b_ReadTempHumi(
                                                        temp_humi_handler->sensor_device->sensor_aht21b,
                                                        temp,       // temp
                                                        humi    );  // humi
        if( AHT21B_STATUS_OK != ret )
        {
            return HANDLER_ERROR;
        }
        // temp_humi_handler->tempurature = *temp;
        // temp_humi_handler->humiditity = *humi;
        /* 原本的设计会将数据进行一次多余的无效复制 */

        // temp_humi_handler->tempurature = 
        temp_humi_handler->last_temp_tick = tim;

        /* @note 这里是判断操作费时间，还是赋值操作费时间？ 如果赋值操作更加费时间，那么用判断来取消赋值是有收益的 */
        temp_humi_handler->temp_humi_data_valid = true;
        /* 获取最新数据后通过发布数据 */
        TempHumiHandler_PublishData(*temp, *humi, tim);
    }

    return  HANDLER_OK;

}




/**
 * @brief 温湿度处理 Handler 的构造函数  
 * 
 * @param [in]  temp_humi_handler 
 * @param [in]  iic_handler_interface 
 * @param [in]  critical_interface 
 * @param [in]  time_base_interface 
 * @param [in]  yield_interface 
 * @param [in]  os_queue 
 * @param [in]  os_delay 
 * @param [in]  os_thread 
 * @return      
 * 
 * @details
 * @note
 *      这里的 handler 的句柄类型不应该还是void了吧？ 接口声明可以使用void，但是公共API不应该
 *          使用 void
 *      如果 handler 的设计是 让handler 运行在thread中，那么就应该在线程中创建，那么句柄类型应该为void*
 * @warning
 */
TempHumiHandlerStatus_t TempHumiHandler_Inst(  
                                        TempHumi_Handler_t * temp_humi_handler,
                            // TempHumiHandler_IICInterface_t * iic_handler_interface,
                            TempHumiHandler_ReplaceIICInterface_t * iic_handler_interface,
                    TempHumiHandler_TimeBaseInterface_t    * time_base_interface,
#ifdef OS_SUPPORTING
                    TempHumiHandler_RTOS_YieldInterface_t  * yield_interface,
                                TempHumiHandler_OsQueue_t  * os_queue,
                                TempHumiHandler_OsDelay_t  * os_delay,
                                TempHumiHandler_OsThread_t * os_thread)
#endif /* OS_SUPPORTING */
{

    /* 1 check the input parameters and resources-------------------------- */
    if( (NULL == temp_humi_handler) ||
        (NULL == iic_handler_interface) ||
        (NULL == time_base_interface) ||
        (NULL == yield_interface) ||
        (NULL == os_queue) ||
        (NULL == os_delay) ||
        (NULL == os_thread)     )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Inst ERROR resources \r\n");
#endif /* HANDLER_DEBUG */
        /* 尝试加断言 */
        return  HANDLER_ERROR_RESOURCE;
    }

    if( true == temp_humi_handler->is_inited_flag->temp_humi_handler_inited_flag)
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Inst is inited \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_ERROR_RESOURCE;
    }
    /* TODO：需要集中检查一下必须的回调，至少包括创建/销毁/接收队列等、tick和yield ，
             这样以后就无需每次重复检查全部资源 */


    /* 2 mount the interfaces to Handler -------------------------- */

    temp_humi_handler->iic_handler_interface = iic_handler_interface;
    temp_humi_handler->time_base_interface   = time_base_interface;
    temp_humi_handler->yield_interface       = yield_interface;
    temp_humi_handler->os_delay              = os_delay;
    temp_humi_handler->os_queue              = os_queue;
    temp_humi_handler->os_thread             = os_thread;
    


    /* 3 init Handler -------------------------- */

    if( HANDLER_OK == TempHumiHandler_Init(temp_humi_handler))
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Init is success  \r\n");
#endif /* HANDLER_DEBUG */
        return  HANDLER_OK;
    }

    return HANDLER_ERROR;
}


/**
 * @brief  温湿度处理 Handler 的运行线程
 * 
 * @param [in]  argument 
 * 
 * @details
 * @note
 *      目前温湿度Handler 和 具体的传感器驱动的耦合的唯一之处在于 通信事件
 * 
 *      将温湿度传感器变量 temp_humi_sensor类型定义为 void *，然后进行宏选择转换，
 *          后面使用temp_humi_sensor 而不是具体的类型变量可行吗？
 * @warning
 */
void TempHumiHandler_RunnableThread(void * argument)
{

    /* 1 Create the resources will used -------------------- */
    // Bsp_Aht21b_Driver_t * temp_humi_aht21b;
//     void * temp_humi_sensor;
//     /* @note 这里应该可以使用条件选择分支进行设备选择 */
// #ifdef SENSOR_SUPPORT_AHT21B
//     // Bsp_Aht21b_Driver_t * temp_humi_aht21b = (Bsp_Aht21b_Driver_t *)temp_humi_sensor;
//     /* 这两个表达式哪个是正确的？转换后进行赋值，变量的类型会永久变化吗？ */
//     temp_humi_sensor = (Bsp_Aht21b_Driver_t *)temp_humi_sensor;

// #endif /* SENSOR_SUPPORT_AHT21B */

    if( NULL == argument )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_RunnableThread argument is ERROR  \r\n");
#endif /* HANDLER_DEBUG */
        return  ;
    }


#ifdef SENSOR_SUPPORT_AHT21B

    // Bsp_Aht21b_Driver_t * temp_humi_sensor;

#endif /* SENSOR_SUPPORT_AHT21B */

    TempHumiHandlerSensorType_t sensor_device = {0};
    TempHumiHandlerInitedFlag_t inited_flag = { false };
    TempHumi_Handler_t  temp_humi_handler_instance = {0};
    temp_humi_handler_instance.is_inited_flag = &inited_flag;
    temp_humi_handler_instance.sensor_device  = &sensor_device;

    TempHumiHandler_InputArgument_t * temp_humi_handler_input_arg = (TempHumiHandler_InputArgument_t *)argument;

    TempHumiHandlerStatus_t inst_retval = HANDLER_RESERVED;
    inst_retval = TempHumiHandler_Inst(&temp_humi_handler_instance,
                            temp_humi_handler_input_arg->iic_handler_interface,
                            temp_humi_handler_input_arg->time_base_interface,
                            temp_humi_handler_input_arg->yield_interface,
                            temp_humi_handler_input_arg->os_queue,
                            temp_humi_handler_input_arg->os_delay,
                            temp_humi_handler_input_arg->os_thread);

    if( HANDLER_OK != inst_retval )
    {
#ifdef HANDLER_DEBUG
        printf(" TempHumiHandler_Inst is FAILED  \r\n");
#endif /* HANDLER_DEBUG */
        return  ;
    }

    // temp_humi_handler_instance.sensor_device->sensor_aht21b = &g_sensor_aht21b;




    /* 2 execute the receive event  -------------------- */
    /* @note 这里如果单独定义一个事件，那么其实可以直接执行的，不需要从handler 去索引事件变量，
            handler和 温湿度事件解耦了，温湿度事件也和 具体的温湿度传感器解耦了
            那么 handler 一定要和通信事件进行耦合吗？*/
    TempHumiHandler_Event_t* temp_humi_event = NULL;
    TempHumiHandlerStatus_t rece_event_retval = HANDLER_RESERVED;

    for(;;)
    {

        /* TODO：接收超时问题的处理 待定，暂时用每次接收前清除来解决*/
        temp_humi_event = NULL;
        rece_event_retval = TempHumiHandler_ReceiveTempHumiEvent(
                                &temp_humi_handler_instance,
                                &temp_humi_event);
        if( HANDLER_OK == rece_event_retval )
        {
            TempHumiHandler_DispatchEvent(temp_humi_event);
        }
        else if (rece_event_retval != HANDLER_ERROR_TIMEOUT)
        {
            /* 真正错误 */
#ifdef HANDLER_DEBUG
            printf(" TempHumiHandler Receive Event is FAILED  \r\n");
#endif /* HANDLER_DEBUG */
            continue;
        }

        /* @note 这里的事件逻辑，应该根据具体的业务逻辑进行还是直接设计为返回特定数据？ */
        /* @note 四个分支绑定的是同一个回调啊，分支个dan啊，人傻了 */
        /* @note 这里的分支执行接收的回调函数 */
        // switch (temp_humi_event->TempHumi_event_type)
        // {
        //     case TempHumi_EVENT_Temp:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;

        //     case TempHumi_EVENT_Humi:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;

        //     case TempHumi_EVENT_1:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;

        //     case TempHumi_EVENT_2:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;

        //     case TempHumi_EVENT_3:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;

        //     case TempHumi_EVENT_4:
        //             temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);
        //         break;
            
        //     default:
        //         break;
        // }


        /* 更新数据 */
        // if ((temp_humi_event != NULL) &&
        //     (temp_humi_event->pf_EventCallBack != NULL))
        // {
        //     // temp_humi_event->pf_EventCallBack(temp_humi_event->temp_humi_sensor);

        // }
        TempHumiHandler_UpdateTempHumiData(&temp_humi_handler_instance,
                                    &temp_humi_handler_instance.tempurature,
                                    &temp_humi_handler_instance.humiditity  );


    }


}













/* --------------------------- Handler unity test -------------------------- */

#ifdef UNITY_TEST_TEMP_HUMI_HANDLER




#endif /* UNITY_TEST_TEMP_HUMI_HANDLER */










/* --------------------------- Handler unity test -------------------------- */








