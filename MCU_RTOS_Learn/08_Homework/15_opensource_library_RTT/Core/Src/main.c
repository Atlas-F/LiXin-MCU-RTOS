/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "elog.h"
#include "SEGGER_RTT.h"
#include "stdio.h"

#include <time.h>

#include "rtc.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LOG_TAG "MAIN"

#define TAG_MAIN    "MAIN"
#define TAG_UART    "UART"
#define TAG_MOTOR   "MOTOR"
#define TAG_SENSOR  "SENSOR"
#define TAG_SERIAL  "SERIAL"
#define TAG_TEST    "TEST"

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void SEGGER_RTT_Test(void);

void AppElogInit(void);

void SerialPortRedirectTest();
void EasylogTest(void);

void RTCTest(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    AppElogInit();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  elog_i(LOG_TAG, " 15_RTT_easylog start !\r\n");

    // SEGGER_RTT_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
//    RTCTest();
//    HAL_Delay(500);
    // SEGGER_RTT_Test();
    // HAL_Delay(500);
//    EasylogTest();
//    HAL_Delay(500);
    //     SEGGER_RTT_WriteString(
    //     0,
    //     RTT_CTRL_TEXT_RED "SEGGER RED TEST" RTT_CTRL_RESET "\r\n"
    // );

    // SEGGER_RTT_WriteString(
    //     0,
    //     "\x1B[31mANSI RED TEST\x1B[0m\r\n"
    // );
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Segger RTT 测试
 * 
 * 
 * @details
 * @note
 * @warning
 */
void SEGGER_RTT_Test(void)
{
    SEGGER_RTT_Init();
    uint32_t cycle =10;
    while (cycle -- )
    {
        SEGGER_RTT_printf(0, "0123456789abcdefgh, cycle = [%d] \r\n", cycle);
    }
    // HAL_Delay(500);
    
}

//初始化只�??在单片机运行�??始时调用�??次即�??


/**
 * @brief  应用层Elog 初始�?
 * 
 * 
 * @details
 * @note
 * @warning
 */
void AppElogInit(void)
{
    elog_init();

    elog_set_text_color_enabled(true);
    

    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    /* start EasyLogger */
    elog_start();
}

/**
 * @brief  串口重定�? 测试
 * 
 * 
 * @details
 * @note
 * @warning
 */
void SerialPortRedirectTest(void)
{
    printf("hello world! \r\n");
    HAL_Delay(500);
}

/**
 * @brief  Easylog 测试
 * 
 * 
 * @details
 * @note
 * @warning
 */
void EasylogTest(void)
{
    uint32_t tick_current = 0 ;
    tick_current = HAL_GetTick();
    // test log output for all level
    log_a("this assert , tick:[%d] ", tick_current);
    log_e("this is error , tick:[%d] ", tick_current);
    log_w("this is warning , tick:[%d] ", tick_current);
    log_i("this is info , tick:[%d] ", tick_current);
    log_d("this is debug , tick:[%d] ", tick_current);
    log_v("this is verbose , tick:[%d] ", tick_current);

    elog_a(TAG_TEST,"this assert , tick:[%d] ", tick_current);
    elog_e(TAG_TEST,"this is error , tick:[%d] ", tick_current);
    elog_w(TAG_TEST,"this is warning , tick:[%d] ", tick_current);
    elog_i(TAG_TEST,"this is info , tick:[%d] ", tick_current);
    elog_d(TAG_TEST,"this is debug , tick:[%d] ", tick_current);
    elog_v(TAG_TEST,"this is verbose , tick:[%d] ", tick_current);
}


/**
 * @brief  RTC 时间测试
 * 
 * 
 * @details
 * @note
 * @warning
 */
void RTCTest(void)
{
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};

    if (HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK) {
        // 必须先读Time，再读Date，锁存日期
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
        printf("Time: %02d:%02d:%02d\r\n", time.Hours, time.Minutes, time.Seconds);
        printf("Date: 20%02d-%02d-%02d\r\n", date.Year, date.Month, date.Date);
    } else {
        printf("RTC read error\r\n");
    }

    // HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    // HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    // printf("tick=%lu time=%02u:%02u:%02u "
    //        "CSR=%08lX BDCR=%08lX ISR=%08lX SSR=%lu PRER=%08lX\r\n",
    //        HAL_GetTick(),
    //        time.Hours, time.Minutes, time.Seconds,
    //        RCC->CSR, RCC->BDCR, RTC->ISR,
    //        RTC->SSR, RTC->PRER);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
