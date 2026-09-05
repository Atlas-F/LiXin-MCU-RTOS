/**
  ******************************************************************************
	WeAct 微行创新 
	>> 标准库实例例程
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

#include "Usart.h"
#include "Flash.h"
#include "Ymodem.h"

#include "elog.h"

#include "Debug.h"
#include "Boot_Manager.h"

#include "SPI.h"
#include "w25qxx.h"

#include "w25qxx_Handler.h" 

// 全局定义 STM32F411xE 或者 STM32F401xx
// 当前定义 STM32F411xE

// STM32F411 外部晶振25Mhz，考虑到USB使用，内部频率设置为96Mhz
// 需要100mhz,自行修改system_stm32f4xx.c

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TAG_MAIN    "MAIN"
#define TAG_UART    "UART"
#define TAG_MOTOR   "MOTOR"
#define TAG_SENSOR  "SENSOR"
#define TAG_SERIAL  "SERIAL"
#define TAG_TEST    "TEST"


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

uint8_t g_au8_test[1024] = {0} ;

int32_t g_file_siz = 0 ;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
 /*
  *power by WeAct Studio
  *The board with `WeAct` Logo && `version number` is our board, quality guarantee. 
  *For more information please visit: https://github.com/WeActTC/MiniF4-STM32F4x1
  *更多信息请访问：https://gitee.com/WeActTC/MiniF4-STM32F4x1
  */
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/* Enable Clock Security System(CSS): this will generate an NMI exception
     when HSE clock fails *****************************************************/
  RCC_ClockSecuritySystemCmd(ENABLE);
	
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */

  /* SysTick end of count event each 1ms */
  SystemCoreClockUpdate();
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
	
  /* Add your application code here */
  /* Insert 50 ms delay */
//   Delay(50);
	
//   GPIO_Config();
    // 测试基本外设操作和初始化
    Key_IO_Init();
    LED_IO_Init();
    // 测试串口初始化
    USART1_Init();
    AppEasylogInit();

    SPI1_Init();

    W25Q64_Init();
    // if( 0 == W25Qx_Init() )
    // {
    //     log_a(" W25 Init OK ! \n ");
    // }
    log_a(" This is  Bootloader");

    if( KeyScan() )
    {
        // 下载到备份区
        // Ymodem_Receive(g_au8_test);
        g_file_siz = Ymodem_Receive(g_au8_test);
        // 将备份区数据拷贝到A区中
        if( 0 == BackToApp(g_file_siz))     // 0 == BackToApp()
        {
            JumpToApp();
        }else
        {

        }
        

    }else
    {
        JumpToApp();
    }

    // TIM_Config(); 
    // TIM_Cmd(TIM3,DISABLE);
  

    
// 测试 Flash 操作函数
//    EreaseAppSector(FLASH_Sector_3);
//    Flash_Write(0x0800C000, 0x55);

    //  Ymodem_Receive(g_au8_test);
    
    //JumpToApp();
//   AppEasylogInit();
  
  //JumpToApp();
  //EasylogTest();
    
      
  Delay(10);
  
  //JumpToApp();
  /* Infinite loop */
  while (1)
  {
    log_e("No Valid App,Please press key and download new App!");
    if( KeyScan() )
    {
        // 下载到备份区
        // Ymodem_Receive(g_au8_test);
        g_file_siz = Ymodem_Receive(g_au8_test);
        // 将备份区数据拷贝到A区中
        if( 0 == BackToApp(g_file_siz))     // 0 == BackToApp()
        {
            JumpToApp();
        }else
        {
            
        }
        

    }
        //如果是按下，则Led翻转
		// if( KeyScan() )
		// {
		// 	//log_a("LED ON");
		// 	USART_SendChar(USART1,'A');
		// 	GPIO_ResetBits(LED_C13_PORT,LED_C13_PIN);
		// }
		// else
		// {
		// 	USART_SendChar(USART1,'B');
		// 	//log_a("LED OFF");
		// 	GPIO_SetBits(LED_C13_PORT,LED_C13_PIN);
		// }
//    if( KeyScan())
//    {
//        log_i("LED ON ");
//        USART_SendChar(USART1, 'A');
//        //GPIO_ResetBits(LED_C13_PORT,LED_C13_PIN);
//    }
//    else
//    {
//        log_i("LED OFF ");
//        USART_SendChar(USART1, 'B');        
//        //GPIO_SetBits(LED_C13_PORT,LED_C13_PIN);
//    }

#if soft_pwm
		/* C13 呼吸灯测试 */
		static uint8_t pwmset;
		static uint16_t time;
		static uint8_t timeflag;
		static uint8_t timecount;

		 /* 呼吸灯 */
		if(timeflag == 0)
		{
			time ++;
			if(time >= 1600) timeflag = 1;
		}
		else
		{
			time --;
			if(time == 0) timeflag = 0;
		}

		/* 占空比设置 */
		pwmset = time/80;

		/* 20ms 脉宽 */
		if(timecount > 20) timecount = 0;
		else timecount ++;

		if(timecount >= pwmset ) GPIO_SetBits(LED_C13_PORT,LED_C13_PIN);
		else GPIO_ResetBits(LED_C13_PORT,LED_C13_PIN);
		
		Delay(1);
#endif
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
