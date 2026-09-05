/**
 * 
 * @file   Usart.c
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-08-27
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

/* Includes ------------------------------------------------------------------*/

 #include "Usart.h"


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  RCC 配置初始化 USART1 USART2
 * 
 * 
 * @details
 * @note
 * @warning
 */
static void RCC_Configuration(void)
{
    // 启用 GPIOA 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 启用UASRT1 时钟
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );
}

/**
 * @brief  串口GPIO配置初始化 USART1 USART2
 * 
 * @details
 * @note
 * @warning
 */
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //配置PA9为USART1 TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //配置PA10为USART1 RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //连接PA9和PA10到USART1的复用功能
    GPIO_PinAFConfig (GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

/**
 * @brief  串口外设相关配置 USART1 Baudrate = 115200， 8N1
 * 
 * 
 * @details 初始化配置串口 USART1 
 * @note
 * @warning
 */
static void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    // 配置USART1
    //USART_InitStructure. USART_BaudRate = 9600;
    USART_InitStructure. USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b ;
    USART_InitStructure. USART_StopBits = USART_StopBits_1;
    USART_InitStructure. USART_Parity = USART_Parity_No;
    USART_InitStructure. USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure. USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd (USART1, ENABLE);
    
    //配置USART2
    // USART_InitStructure. USART_BaudRate = 115200;
    // USART_Init(USART2, &USART_InitStructure);
    // USART_Cmd (USART2, ENABLE);
}

/**
 * @brief  串口发送字节函数，使用USART1或者USART2,使用阻塞方式
 * 
 * @param [in]  USARTx  USART1或者USART2
 * @param [in]  data    数据
 * 
 * @details
 * @note
 * @warning
 */
void USART_SendChar( USART_TypeDef* USARTx, uint8_t data )
{
    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
    
    USART_SendData(USARTx, data);
    
}

/**
 * @brief  串口接收字节函数，使用USART1或者USART2,使用阻塞方式
 * 
 * @param [in]  USARTx  USART1或者USART2
 * 
 * @details
 * @note
 * @warning
 */
uint8_t USART_ReceiveChar(USART_TypeDef* USARTx)
{
    while( USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET );

    return USART_ReceiveData(USARTx);
}

/**
 * @brief  串口1 初始化函数
 * 
 * @details
 * @note
 * @warning
 */
void USART1_Init(void)
{
    RCC_Configuration();
    GPIO_Configuration();
    USART_Configuration();
}

