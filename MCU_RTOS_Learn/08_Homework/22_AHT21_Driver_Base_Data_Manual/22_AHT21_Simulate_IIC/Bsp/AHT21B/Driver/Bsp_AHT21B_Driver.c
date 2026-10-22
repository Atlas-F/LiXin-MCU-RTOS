/**
 * 
 * @file   Bsp_AHT21B_Driver.c
 * @brief  BSP 层 AHT21B 温湿度传感器 driver 源码
 * 
 * @author AtlasFg
 * @date   2026-10-14
 * @version 1.0
 * 
 * @par 说明
 * 
 *      dependencies
 *          stdio.h
 *          stdint.h
 *          Bsp_Aht21b_Driver.h
 *      
 * * 
 * @note 注意事项
 *      目前剩余的主要问题：
        1. 底层 ACK/I²C错误没有向上返回；
        2. 状态检查分支无论结果都返回成功；
        3. 最后一轮 Busy 后多等待 5 ms；
        4. 0x80仍是未命名魔法数。
 * * 
 * @warning 参考
 * * 
 * * 
 * ********************************************************************************
 */


/* Private includes ---------------------------------------------------------*/
#include "Bsp_AHT21B_Driver.h"

#include <stdio.h>
#include <stdint.h>








 /* Private define ----------------------------------------------------------*/

#define CRC8_POLYNOMIAL           0x31        // CRC-8 polynomial
#define CRC8_INITIAL              0xFF        // CRC-8 initial value









/* Private variables --------------------------------------------------------*/

static int8_t  g_aht21b_inited    = AHT21B_NOT_INITED;
static uint8_t g_device_id = 0 ;





/* Private function prototypes ----------------------------------------------*/

/**
 * @brief Function for calculating CRC-8 checksum.
 * 
 * Steps:
 *  1, XOR each byte of the input data with the CRC register.
 *  2, For each bit, check the most significant bit.
 *  3, Shift the CRC register left and, if the most significant bit is 1,
 *  XOR with the polynomial.
 * 
 * @param[in] p_data : Pointer to the input data.
 * @param[in] length : Length of the input data.
 * 
 * @return uint8_t : The calculated CRC-8 checksum.
 * 
 * */
static uint8_t BspAht21bDriver_CheckCRC8(const uint8_t *p_data, const uint8_t length)
{
    uint8_t crc = CRC8_INITIAL; // Initialize CRC register

    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= p_data[i]; // XOR the current byte with the CRC

        // Process each bit of the current byte
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                // If MSB is 1, shift and XOR with polynomial
                crc = (crc << 1) ^ CRC8_POLYNOMIAL; 
            }
            else
            {
                // Otherwise, just shift left
                crc <<= 1; 
            }
        }
    }

    return crc; // Return the final CRC value

}


/**
 * @brief  read aht21b sensor ID 
 * 
 * @param [in]  aht21b_driver 
 * 
 * @details
 * @note
 *      // TODO: 没有在手册上看到读取传感器 id 相关内容  
 *      // 是应该在 返回值中将数据返回？
 *  
 *      读取设备ID的方式
 * @warning
 */
static BspAht21bStatus_t __ReadAht21bID(Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( NULL == aht21b_driver)
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }


    /* 2、check the source will used ---------------------------------- */



    /* 3、Execute the read the Aht21b ID  ---------------------------------- */
    uint8_t data = 0;

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalEnter(NULL);
#endif /* End of HARDWARE_IIC */
    
    // Send the IIC start Signal
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Start(NULL);
    
    // Send command to read ID
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL,\
                                                AHT21_REG_READ_ADDR);
    
    // Wait the ACK of IIC slave device
    if ( AHT21B_STATUS_OK == \
        aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL))
    {
        // Receive the data from slave device
        aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,\
                                                                     &data);
    }
    
    // Send the stop signal
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Stop(NULL);
    
#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalExit(NULL);
#endif /* End of HARDWARE_IIC */
    printf("g_device_id data :[%d] \r\n", data);

    // Check the ID
    if ( STATUS_CHECK_AND_CODE == (data & STATUS_CHECK_AND_CODE) )
    {
        g_device_id = STATUS_CHECK_AND_CODE;
        printf("g_device_id :[%d] \r\n", g_device_id);
        
        return AHT21B_STATUS_OK;
    }
    printf("g_device_id out :[%d] \r\n", g_device_id);

    
    return AHT21B_STATUS_ERROR_RESOURCE;

}

/**
 * @brief  Get Aht21b sensor id, just return global value
 * 
 * @param [in]  aht21b_driver 
 * @return      
 * 
 * @details 通过 __ReadAht21bID 返回 g_device_id
 * @note
 *        
 * @warning
 */
static uint8_t BspAht21bDriver_GetAht21bID(
                                        Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( NULL == aht21b_driver)
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 Execute Get Aht21b ID ----------------------------------- */


    BspAht21bStatus_t ret_val = AHT21B_STATUS_OK;
    /* 这里读取 id 需要分情况，如果之前没有 执行过 __ReadAht21bID 那么直接返回 g_device_id
        就不是正确的方式 */
    ret_val = __ReadAht21bID(aht21b_driver);
    if(AHT21B_STATUS_OK != ret_val)
    {
#ifdef DEBUG
        printf("GetAht21bID read aht21b id  !");
#endif /* DEBUG */
        return -1;
    }
    return g_device_id;

}












/* pfAht21b_Init */

/**
 * @brief  Init the aht21b sensor 
 * 
 * @param [in]  aht21b_self 
 * 
 * @details 初始化的核心是读取传感器ID,验证 传感器是否可用 
 * @note
 *      初始化的核心是读取传感器ID，那么是否要判断读取结果成功与否？
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_Init(Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( NULL == aht21b_driver)
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }


    /* 2 check source --------------------------------------------- */
    if( AHT21B_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }

    /* @note 这里让出 CPU 时间需要核对手册以及，进行检查 */
    aht21b_driver->pRTOS_YieldInstance->pf_RTOS_Yield(100);


/* 这里的这些接口，因为上层已经调用过了，所以可以不再检查 */
/* @note 使用临界区操作，处理并发，防止重复初始化 */
#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalEnter(NULL);
#endif /* HARDWARE_IIC */

    /* @note 应该在这里初始化总线资源，还是在别的地方一开始就初始化总线资源？ */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Init(NULL);

    /* @note 这里应该检查返回值的 */
    __ReadAht21bID(aht21b_driver); /* @note */

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalExit(NULL);
#endif /* HARDWARE_IIC */

    g_aht21b_inited = AHT21B_INITED;
#ifdef DEBUG
        printf("g_aht21b_inited:[%d] \r\n", g_aht21b_inited);
#endif /* DEBUG */
    return AHT21B_STATUS_OK;

}


/**
 * @brief  DeInit the aht21b sensor 
 * 
 * @param [in]  aht21b_driver 
 * @return      
 * 
 * @details 这里的反初始化操作仅仅是修改 初始化标志位，因为初始化操作的语义是
 *          读取传感器id，无实质的资源操作，所以这里设置初始化标志位 即可
 *  
 * @note
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_DeInit(Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( NULL == aht21b_driver)
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }

    /* 3 execute --------------------------------------------------- */

    g_aht21b_inited = AHT21B_NOT_INITED;

    return AHT21B_STATUS_OK;

}


/**
 * @brief  读取 Aht21b 传感器 信息
 * 
 * @param [in]  aht21b_driver 
 * @return      
 * 
 * @details 根据手册的上电读取传感器信息，即发送 0x71 读回状态字，并检查状态字，
 *          如果状态字不对，则执行特定的初始化流程
 * @note
 *      这种 void * 的参数形式，是否是合适的？需要如何进行填充参数？
 * 
 *      当前 status_ret错误检查的结构是正确的，但只有底层结果被逐级检查并返回后，它才真正有效。
 *          并没有对内部的 iic 函数进行返回值检查
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_ReadAht21bStatus(
                                    Bsp_Aht21b_Driver_t * const aht21b_driver,
                                    uint8_t * const sensor_status)
{
    /* 1 check input parameters ----------------------------------- */
    if( (NULL == aht21b_driver) ||
        (NULL == sensor_status))
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }

    /* 3 execute --------------------------------------------------- */
#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalEnter(NULL);
#endif /* HARDWARE_IIC */   
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Start(NULL);

    /* 发送 0x71 获取状态字 */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL, AHT21B_STATUS_CMD_CODE);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,sensor_status);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendNACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_Stop(NULL);

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalExit(NULL);
#endif /* HARDWARE_IIC */

    /* 检查获取的状态字 */
    if(STATUS_CHECK_AND_CODE != (*sensor_status&STATUS_CHECK_AND_CODE) )
    {
        /* TODO: 初始化 指定寄存器组：INIT_ABNORMAL_REG_1、2、3 */

        return AHT21B_STATUS_OK;
    }
    else
    {
        return AHT21B_STATUS_OK;
    }

}



/**
 * @brief  通过 IIC 总线读取温湿度数据，原始未解析的温湿度数据
 * 
 * @param [in]  aht21b_driver 
 * @param [in]  tempera_humi 
 * @return      
 * 
 * @details 这里打算使用数组进行入参和出参
 * @note
 *      // TODO：等待 10 ms
 *      // TODO：等待 80 ms 测量完成
 *      // TODO：温湿度数据解析 
 *      // 温湿度参数到底应该用 uint8_t 还是 float？
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_ReadTemperaHumi(
                                    Bsp_Aht21b_Driver_t * const aht21b_driver,
                                    float * const temp, 
                                    float * const humi)      
{
    /* 1 check input parameters ----------------------------------- */
    if( (NULL == aht21b_driver) ||
        (NULL == temp) ||
        (NULL == humi))
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }

    uint8_t  cnt                     = 5;
    uint8_t  byte_1_status           = 0;
    uint8_t  byte_2_humidity         = 0;
    uint8_t  byte_3_humidity         = 0;
    uint8_t  byte_4_humi_temperature = 0;
    uint8_t  byte_5_temperature      = 0;
    uint8_t  byte_6_temperature      = 0;
    uint32_t return_data             = 0 ;

    uint8_t data_crc = 0;


    /* 3 execute --------------------------------------------------- */
#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalEnter(NULL);
#endif /* HARDWARE_IIC */   
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Start(NULL);

    /* 发送 0x70  */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL, AHT21B_REG_WRITE_ADDR);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    /* @note 这里到底要不要等待 10 ms，根据手册需要等待10 ms，但是并不清楚 */
    /* TODO：等待 10 ms */

    /* TODO：等待 10 ms */

    /* 发送触发 测量指令 */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL, TRIGGER_MEASURE_CODE);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL, TRIGGER_MEASURE_PAR_1);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL, TRIGGER_MEASURE_PAR_2);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_Stop(NULL);

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalExit(NULL);
#endif /* HARDWARE_IIC */

/* ----- 等待 80 ms 测量完成 ----- */
    // Do the delay with AHT21B_MEASURE_WAITING_TIME 
    int32_t start_time = \
                aht21b_driver->pTimeBaseInterface->pf_GetTickCount();
    while(aht21b_driver->pTimeBaseInterface->pf_GetTickCount() - 
                                       start_time < AHT21B_MEASURE_WAITING_TIME)
    {   
#ifdef OS_SUPPORTING
        aht21b_driver->pRTOS_YieldInstance->pf_RTOS_Yield(\
                                                   AHT21B_MEASURE_WAITING_TIME);
#endif //End of OS_SUPPORTING
    }
    // Do the delay with AHT21B_MEASURE_WAITING_TIME 

    /* 反复读取状态字，等待 AHT21B 完成测量。 */
    uint8_t sensor_status = 0U;
    BspAht21bStatus_t status_ret = AHT21B_STATUS_ERROR;

        /* cnt是“剩余状态查询次数”，本质上是一个软件超时机制 */
    while (cnt > 0U)
    {
        status_ret = BspAht21bDriver_ReadAht21bStatus(
            aht21b_driver,
            &sensor_status);

        if (AHT21B_STATUS_OK != status_ret)
        {
            return status_ret;
        }

        if (0U == (sensor_status & 0x80U))
        {
            /* 如果 bit7 已经清零，退出循环，开始读取测量数据。 */
            break;
        }

    #ifdef OS_SUPPORTING
        aht21b_driver->pRTOS_YieldInstance->pf_RTOS_Yield(5U);
    #endif

        cnt--;
    }

    if (0U == cnt)
    {
        return AHT21B_STATUS_ERROR_TIMEOUT;
    }

    
#ifdef DEBUG
        printf("Aht21b_Driver read temp !");
#endif /* DEBUG */


/* ----- 等待 80 ms 测量完成 ----- */

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalEnter(NULL);
#endif /* HARDWARE_IIC */   
    aht21b_driver->pIIC_DriverInstance->pf_IIC_Start(NULL);

    /* 发送 0x71 获取状态字 */
    /* @note 这里使用 函数去接收状态字，那么会不会错过 IIC 总线信息？
        还是应该执行操作指令，发送 0x71 并接收状态字？如果使用执行函数的话，状态字信息应该返回 */
    // uint8_t sensor_status  = 0;
    // BspAht21bDriver_ReadAht21bStatus(aht21b_driver, &byte_1_status);

    
    /* TODO：@note 这里status 待接收 */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendByte(NULL,AHT21_REG_READ_ADDR);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_WaitACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_1_status);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_2_humidity);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_3_humidity);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_4_humi_temperature);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_5_temperature);
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&byte_6_temperature);
#ifdef CRC_ENABLE
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendACK(NULL);

    aht21b_driver->pIIC_DriverInstance->pf_IIC_ReceiveByte(NULL,&data_crc);
#endif /* CRC_ENABLE */
    aht21b_driver->pIIC_DriverInstance->pf_IIC_SendNACK(NULL);   

    aht21b_driver->pIIC_DriverInstance->pf_IIC_Stop(NULL);

#ifndef HARDWARE_IIC
    aht21b_driver->pIIC_DriverInstance->pf_CriticalExit(NULL);
#endif /* HARDWARE_IIC */

/* ----- 温湿度数据解析 ----- */
    /* TODO：@note 这里status 待接收 */

    return_data = (return_data|byte_2_humidity)<<8;
    return_data = (return_data|byte_3_humidity)<<8;
    return_data = (return_data|byte_4_humi_temperature);
    return_data =return_data >>4;
    *humi = (return_data * 1000 >> 20);
    *humi /= 10;
    
    return_data = 0;
    return_data = (return_data|(byte_4_humi_temperature&0x0f))<<8;
    return_data = (return_data|byte_5_temperature)<<8;
    return_data = (return_data|byte_6_temperature);
    return_data = return_data&0xfffff;
    *temp = ((return_data * 2000 >> 20)- 500);
    *temp /= 10;

/* ----- 温湿度数据解析 ----- */

    return AHT21B_STATUS_OK;

}


/**
 * @brief  获取湿度数据（湿度可以快速变化）
 * 
 * @param [in]  p_aht21_instance 
 * @param [in]  humi 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_ReadHumidity(
                                Bsp_Aht21b_Driver_t * const aht21b_driver, 
                                                        float * const humi)
{
    /* 1 check input parameters ----------------------------------- */
    if( (NULL == aht21b_driver) ||
        (NULL == humi) )             /* 数组是否需要检查空指针？ */
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }


    /* 3 execute --------------------------------------------------- */
    /* TODO：返回数据还是执行读取操作，湿度不是可以快速变化吗 */


    return AHT21B_STATUS_OK;
}


/**
 * @brief  Aht21b 休眠设置
 * 
 * @param [in]  aht21b_driver 
 * @return      
 * 
 * @details
 * @note    
 *      休眠方法，这种低功耗操作，需要在编写设计驱动的时候考虑吗？还需要考虑什么额外的操作模式吗？
 * 
 *      Option：休眠模式 功能的话，应该带有休眠时间作为参数的吧？
 *                  指定休眠时间长度（到期自动唤醒）、或者不指定休眠时间一直休眠，直到被唤醒
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_Sleep(
                                Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( (NULL == aht21b_driver))             
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }


    /* 3 execute --------------------------------------------------- */
    /* TODO：*/


    return AHT21B_STATUS_OK;



}

/**
 * @brief  Aht21b 唤醒设置
 * 
 * @param [in]  aht21b_driver 
 * @return      
 * 
 * @details
 * @note
 *      问题同上
 * @warning
 */
static BspAht21bStatus_t BspAht21bDriver_WakeUp(
                                Bsp_Aht21b_Driver_t * const aht21b_driver)
{
    /* 1 check input parameters ----------------------------------- */
    if( (NULL == aht21b_driver))             
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }

    /* 2 check source --------------------------------------------- */
    if( AHT21B_NOT_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver Not inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }


    /* 3 execute --------------------------------------------------- */
    /* TODO：*/


    return AHT21B_STATUS_OK;
}





/**
 * @brief  Aht21b 驱动实例化
 * 
 * @param [in]  pAht21bInstance 
 * @param [in]  pIIC_DriverInstance 
 * @param [in]  pTimeBaseInterface 
 * @param [in]  pRTOS_YieldInstance 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
BspAht21bStatus_t pAht21b_DriverInst(                             
                    Bsp_Aht21b_Driver_t             * const pAht21bInstance ,
                    IIC_DriverInterface_t           * const pIIC_DriverInstance,
                    BspAht21b_TimeBaseInterface_t   * const pTimeBaseInterface,
#ifdef OS_SUPPORTING
                    BspAht21b_RTOS_YieldInterface_t * const pRTOS_YieldInstance)
#endif /* OS_SUPPORTING */                                                     
{
#ifdef DEBUG
        printf("Aht21b_Driver start inst  !");
#endif /* DEBUG */
    /* 1、check input parameters -------------------------------------- */
    if( (NULL == pIIC_DriverInstance) || 
        (NULL == pTimeBaseInterface) ||
        (NULL == pRTOS_YieldInstance)   )
    {
#ifdef DEBUG
        printf("Aht21b_Driver parameters ERROR !");
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_NULL_POINTER;
    }


    /* 2、check the source will used ---------------------------------- */
    if( AHT21B_INITED == g_aht21b_inited )
    {
#ifdef DEBUG
        printf("Aht21b_Driver is inited !" );
#endif /* DEBUG */
        return AHT21B_STATUS_ERROR_RESOURCE;
    }

    /* 3、execute entity init ,fill source ---------------------------- */


    /* mount interface and source ------------------------------------- */
    pAht21bInstance->pIIC_DriverInstance = pIIC_DriverInstance;
    pAht21bInstance->pRTOS_YieldInstance = pRTOS_YieldInstance;
    pAht21bInstance->pTimeBaseInterface  = pTimeBaseInterface;

    /* Todo: mount IIC interface  */
        // pAht21bInstance->pIIC_DriverInstance->pf_CriticalEnter   = NULL ; /* TODO: */
        // ...
    /* 这些上面已经挂载过了，不需要再进行挂载了，应该是别的接口 */


    pAht21bInstance->pfAht21b_Init         = BspAht21bDriver_Init ; /* TODO: */
    pAht21bInstance->pfAht21b_DeInit       = BspAht21bDriver_DeInit ; /* TODO: */
    pAht21bInstance->pfAht21b_ReadID       = BspAht21bDriver_GetAht21bID ; /* TODO: */
    pAht21bInstance->pfAht21b_ReadTempHumi = BspAht21bDriver_ReadTemperaHumi ; /* TODO: */
    pAht21bInstance->pfAht21b_ReadHumi     = BspAht21bDriver_ReadHumidity ; /* TODO: */
    pAht21bInstance->pfAht21b_Sleep        = BspAht21bDriver_Sleep ; /* TODO: */
    pAht21bInstance->pfAht21b_WakeUp       = BspAht21bDriver_WakeUp ; /* TODO: */

    /* @note 挂载完了之后还要不要再次检查是否为NULL？ */
    /* @note 应该在实例化之后调用初始化 */

    // g_aht21b_inited = AHT21B_INITED;
#ifdef DEBUG
        printf("Aht21b_Driver inst OK !");
#endif /* DEBUG */

    return AHT21B_STATUS_OK;

}

