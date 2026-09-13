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



#include "elog.h"

#include "Debug.h"
#include "Boot_Manager.h"

#include "SPI.h"
#include "Usart.h"
#include "Flash.h"
#include "Ymodem.h"
#include "w25qxx.h"

#include "w25qxx_Handler.h" 
#include "AT24Cxx_Driver.h"

#include "SEGGER_RTT.h"
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

extern uint8_t g_au8_test[1024]  ;     /* 这个变量是否添加需要考虑 */




/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*
*power by WeAct Studio
*The board with `WeAct` Logo && `version number` is our board, quality guarantee. 
*For more information please visit: https://github.com/WeActTC/MiniF4-STM32F4x1
*更多信息请访问：https://gitee.com/WeActTC/MiniF4-STM32F4x1
*/


/**
 * @brief  debug 测试eeprom 各个标志位的值
 * 
 * 
 * @details
 * @note
 * @warning
 */
void Debug_AT_Flags()
{
    /* 逐步打印eeprom 中的各个标志位的数据值*/
    uint8_t flag_ota_state = 0;
    uint32_t flag_ota_new_app_size = 0 ;
    uint32_t flag_current_app_size =0 ;

    ee_ReadBytes(&flag_ota_state, 0x00, 1 );
    ee_ReadBytes((uint8_t * )&flag_ota_new_app_size, 0x01, 4 );
    ee_ReadBytes((uint8_t * )&flag_current_app_size, 0x05, 4 );

    log_i("flag_ota_state:[%d] \n", flag_ota_state);
    log_i("flag_ota_new_app_size:[%d] \n", flag_ota_new_app_size);
    log_i("flag_current_app_size:[%d] \n", flag_current_app_size);

    USART_SendString(USART1, &flag_ota_state, 1);
    USART_SendString(USART1, &flag_ota_new_app_size, 4);
    USART_SendString(USART1, &flag_current_app_size, 4);

    /* 判断各个标志位的数据值*/
}

void DebugAT24Clear()
{
    uint8_t clear_data = 0 ;
    uint32_t clear_data_32 = 0 ;
    ee_WriteBytes(&clear_data, 0x05, 4); 
    ee_WriteBytes(&clear_data, 0x00, 1);
    ee_WriteBytes(&clear_data, 0x01, 4);
}



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
    // EasylogTest();

    SPI1_Init();
    W25Q64_Init();
    ee_CheckOk();
    
    uint8_t read_ota_state = 0 ;
    ee_ReadBytes(&read_ota_state, 0x00, 1);
    // SEGGER_RTT_Write(0, &read_ota_state, 1);
    USART_SendChar(USART1, read_ota_state);
    USART_SendChar(USART1, 0XAB);

    // SEGGER_RTT_WriteString(0, "ok\r\n");

    uint8_t hello_USART[] = "1234567890";
    for( int i= 0; i < 10; i++ )
    {
        
        log_a(" This is  Bootloader");
        Delay(5);
    }

    //DebugAT24Clear();
    Debug_AT_Flags();
    // if(KeyScan())
    // {
        // read_ota_state = 0x22 ;
        // ee_WriteBytes(&read_ota_state, 0x00 , 1);
        // ee_ReadBytes(&read_ota_state, 0x00, 1);
        // USART_SendChar(USART1, read_ota_state);
    // }
    
    OTA_StateManager();
    // if( 0 == W25Qx_Init() )
    // {
    //     log_a(" W25 Init OK ! \n ");
    // }

    // if( KeyScan() )
    // {
    //     // 下载到备份区
    //     // Ymodem_Receive(g_au8_test);
    //     g_file_siz = Ymodem_Receive(g_au8_test);
    //     // 将备份区数据拷贝到A区中
    //     if( 0 == BackToApp(g_file_siz))     // 0 == BackToApp()
    //     {
    //         JumpToApp();
    //     }else
    //     {

    //     }
    // }else
    // {
    //     JumpToApp();
    // }

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
    int32_t file_size = 0 ;
    uint32_t current_app_size = 0 ;
    // static uint8_t g_au8_test[1024] = {0} ;     /* 这个变量是否添加需要考虑 */

/* Infinite loop */
while (1)
{
    log_e("No Valid App,Please press key and download new App!");
    if( KeyScan() )
    {
        /* 下载流程  */
        file_size = Ymodem_Receive(g_au8_test);
        // 将备份区数据拷贝到外部 Flash B区中
        if( 0 == ExA_To_ExB_AES(file_size))     // 0 == BackToApp()
        {
            /* 将当前正在运行的APP的数据搬运到外部 Flash A 区, 进行备份，防止下载失败或者异常问题导致设备变砖 */
            /* 增加一个代码回退功能 */
            // 读取当前APP的大小
            ee_ReadBytes((uint8_t *)&current_app_size, 0x05, 4);
            App_To_ExA(current_app_size);
            /* 外部flash B区搬运到内部flash 区 */
            ExB_To_App();
            /* 执行跳转*/
            JumpToApp();
            /* 校验数据是否合法，如果数据无效，将外部 Flash A 区中的数据搬运到 APP 中 */
            ExA_To_App();
            JumpToApp();

        }else
        {
            log_a(" Boot download failed \n");
            JumpToApp();
        }
        /* 下载流程  */
    }
}

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
