/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"

#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "BSP_Led_Driver.h"
#include "BSP_LED_Handler.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


static BspLedStatus_t BspLedDemo_On(void)
{
    printf("led is on \r\n");
    return LED_STATUS_OK;
}
static BspLedStatus_t BspLedDemo_Off(void)
{
    printf("led is off \r\n");
    return LED_STATUS_OK;
}

static BspLedOperations_t g_bsp_led_demo_operations = {
    .BspLed_On = BspLedDemo_On,
    .BspLed_Off = BspLedDemo_Off,
};

static BspLedStatus_t BspLedDemo_GetTimeMs(uint32_t *const time_stamp)
{
    printf("get time now time zero \r\n");
    *time_stamp = 0U;
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
    (void)delay_time;
    for (int32_t delay_index = 500; delay_index > 0; --delay_index)
    {
    }
    printf("finished 1 ms \r\n");
    return LED_STATUS_OK;
}

static BspLedOsDelay_t g_bsp_led_demo_os_delay = {
    .BspLed_DelayMs = BspLedDemo_DelayMs,
};


/* ---------------------- Unity Test ---------------------------------------*/
#ifdef BSP_LED_DEBUG_UNITY_TEST

// TBD: Handler OS queue functions.

static BspLedHandlerStatus_t BspLedDemo_QueueCreate(uint32_t item_num,
                                                     uint32_t item_size,
                                                     void **queue_handle)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueueCreate \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
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

static BspLedHandlerStatus_t BspLedDemo_QueueDestroy(void **queue_handle)
{
#ifdef DEBUG_ENABLED
    DEBUG_OUT("BspLedDemo_QueueDestroy \r\n");
#endif
    BspLedHandlerStatus_t status = LED_HANDLER_STATUS_OK; 
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
    .BspLedHandler_CreateThread  = xTaskCreate,
    .BspLedHandler_DestroyThread = vTaskDelete
};



#endif /* BSP_LED_DEBUG_UNITY_TEST */

/* ---------------------- Unity Test ---------------------------------------*/


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
 
//   BspLedDriver_t led_1;


#ifdef BSP_LED_DEBUG_UNITY_TEST

    static BspLedHandler_t bsp_led_handler_instance = {0};
    BspLedHandler_Instantiate(&bsp_led_handler_instance,
                              &g_bsp_led_handler_demo_time_base_ms,
                              &g_bsp_led_handler_demo_os_delay,
                              &g_bsp_led_handler_demo_os_queue,
                              &g_bsp_led_handler_demo_os_thread);

    BspLedDriver_t led_1 = {0};
    BspLedDriver_Instantiate(&led_1,
                             &g_bsp_led_demo_operations,
                             &g_bsp_led_demo_time_base_ms,
                             &g_bsp_led_demo_os_delay);

    bsp_led_handler_instance.BspLedHandler_RegisterDriver(&bsp_led_handler_instance,
                                                           &led_1,
                                                           &bsp_led_handler_instance.led_index);
    bsp_led_handler_instance.BspLedHandler_Control(&bsp_led_handler_instance, 6U, 4U, LED_PROPORTION_1_2);


#endif /* BSP_LED_DEBUG_UNITY_TEST */


    // led_1.BspLedDriver_Control(&led_1, 4U, 6U, LED_PROPORTION_1_1);


  for(;;)
  {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

