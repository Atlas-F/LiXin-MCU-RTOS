/**
 * 
 * @file   Test_Aht21b_Driver.c
 * @brief  关于 温湿度传感器 Aht21b 驱动的 单元测试
 * 
 * @author AtlasFg
 * @date   2026-10-18
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
#include "unity.h"

#include "Bsp_AHT21B_Driver.h"
#include <string.h>
// #include "Test_Aht21b_Driver.h"


 /* Private define ----------------------------------------------------------*/







/* Private variables --------------------------------------------------------*/
static Bsp_Aht21b_Driver_t              g_fake_driver;
static IIC_DriverInterface_t            g_fake_iic;
static BspAht21b_TimeBaseInterface_t    g_fake_timebase;
static BspAht21b_RTOS_YieldInterface_t  g_fake_yield;

static BspAht21b_RTOS_YieldInterface_t  g_fake_yield_2;


static uint32_t g_fake_yield_call_count;
static uint32_t g_fake_yield_time;

static uint32_t g_critical_enter_count;
static uint32_t g_critical_exit_count;
static uint32_t g_iic_init_count;


 /* Private function prototypes ----------------------------------------------*/

 static void Fake_RTOS_Yield(const uint32_t delay_time)
{
    g_fake_yield_call_count++;
    g_fake_yield_time = delay_time;
}

static BspAht21bStatus_t Fake_CriticalEnter(void *context)
{
    g_critical_enter_count++;
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_CriticalExit(void *context)
{
    g_critical_exit_count++;
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_Init(void *context)
{
    g_iic_init_count++;
    return AHT21B_STATUS_OK;
}


static BspAht21bStatus_t Fake_IIC_Start(void *context)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_Stop(void *context)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_WaitACK(void *context)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_SendACK(void *context)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_SendNACK(void *context)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_SendByte(
    void *context,
    const uint8_t data)
{
    return AHT21B_STATUS_OK;
}

static BspAht21bStatus_t Fake_IIC_ReceiveByte(
    void *context,
    uint8_t *data)
{
    if (data != NULL)
    {
        *data = 0x00;
    }

    return AHT21B_STATUS_OK;
}

/* -------------------------------------------------- */

void TestAht21b_Init_DeInit(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );
    BspAht21bStatus_t result = g_fake_driver.pfAht21b_Init(&g_fake_driver);

    BspAht21bStatus_t DeInitresult = g_fake_driver.pfAht21b_DeInit(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);
    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, DeInitresult);
    // TEST_ASSERT_EQUAL_INT(1, g_aht21b_inited );

}

void TestAht21b_DeInit(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );
    /* 修正：根据需要先进行初始化 */
    g_fake_driver.pfAht21b_Init(&g_fake_driver);

    BspAht21bStatus_t result = g_fake_driver.pfAht21b_DeInit(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);
    // TEST_ASSERT_EQUAL_INT(0, g_aht21b_inited );

}

void TestAht21b_GetAht21bID(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );
    BspAht21bStatus_t result = g_fake_driver.pfAht21b_ReadID(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);
    // TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, g_device_id);

}

/* TODO :待测试函数：BspAht21bDriver_ReadAht21bStatus */
void TestAht21b_ReadAht21bStatus(void)
{
    BspAht21bStatus_t result ;

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

}


void TestAht21b_pfAht21b_ReadTempHumi(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );

    /* 修正：根据需要先进行初始化 */
    g_fake_driver.pfAht21b_Init(&g_fake_driver);

    float tempera_humi = 0;
    float tempera_humi_2 = 0;
    BspAht21bStatus_t result = g_fake_driver.pfAht21b_ReadTempHumi(
                                &g_fake_driver,
                                &tempera_humi,
                                &tempera_humi_2);

    /* 修正：根据需要在初始化完成后进行反初始化 */              
    g_fake_driver.pfAht21b_DeInit(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

}

void TestAht21b_pfAht21b_ReadHumi(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );

    /* 修正：根据需要先进行初始化 */
    g_fake_driver.pfAht21b_Init(&g_fake_driver);

    float humi = 0;
    BspAht21bStatus_t result = g_fake_driver.pfAht21b_ReadHumi(
                                &g_fake_driver,
                                &humi);

    /* 修正：根据需要在初始化完成后进行反初始化 */              
    g_fake_driver.pfAht21b_DeInit(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

}

void TestAht21b_pfAht21b_Sleep(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );

    /* 修正：根据需要先进行初始化 */
    g_fake_driver.pfAht21b_Init(&g_fake_driver);
    
    BspAht21bStatus_t result = g_fake_driver.pfAht21b_Sleep(&g_fake_driver);

    /* 修正：根据需要在初始化完成后进行反初始化 */              
    g_fake_driver.pfAht21b_DeInit(&g_fake_driver);

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

}


void TestAht21b_pfAht21b_WakeUp(void)
{
    /* 修正：根据需要显式调用实例化函数 */
    pAht21b_DriverInst(
        &g_fake_driver,
        &g_fake_iic,
        &g_fake_timebase,
        &g_fake_yield
    );
    /* 修正：根据需要先进行初始化 */
    g_fake_driver.pfAht21b_Init(&g_fake_driver);

    BspAht21bStatus_t result = g_fake_driver.pfAht21b_WakeUp(&g_fake_driver);

    /* 修正：根据需要在初始化完成后进行反初始化 */              
    g_fake_driver.pfAht21b_DeInit(&g_fake_driver);
    
    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

}



 /**
  * @brief  
  * 
  * 
  * @details
  * @note
  *         当前设计，fake 接口都只是空的定义，没有实际内容，但是内存中的数值不是明确的，
  *         是否要进行指定值填充？魔术字
  * @warning
  */
void TestAht21bDriverInst_ShouldMountDependenciesAndOperations(void)
{
    BspAht21bStatus_t result =
        pAht21b_DriverInst(
            &g_fake_driver,
            &g_fake_iic,
            &g_fake_timebase,
            &g_fake_yield
        );

    TEST_ASSERT_EQUAL_INT(AHT21B_STATUS_OK, result);

    TEST_ASSERT_EQUAL_PTR(
        &g_fake_iic,
        g_fake_driver.pIIC_DriverInstance
    );

    TEST_ASSERT_EQUAL_PTR(
        &g_fake_timebase,
        g_fake_driver.pTimeBaseInterface
    );

    TEST_ASSERT_EQUAL_PTR(
        &g_fake_yield,
        g_fake_driver.pRTOS_YieldInstance
    );

    /* 想要测试模块内部的static 函数怎么测试？
        在 inst 函数中挂载在了 */
    // TEST_ASSERT_NULL(
    //     g_fake_driver.pfAht21b_Init
    // );


    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_Init);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_DeInit);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_ReadID);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_ReadTempHumi);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_ReadHumi);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_Sleep);
    TEST_ASSERT_NOT_NULL(g_fake_driver.pfAht21b_WakeUp);


}
/* ------------------------------------------------------- */
void setUp(void)
{
    memset(&g_fake_driver, 0, sizeof(g_fake_driver));
    memset(&g_fake_iic, 0, sizeof(g_fake_iic));
    memset(&g_fake_timebase, 0, sizeof(g_fake_timebase));
    memset(&g_fake_yield, 0, sizeof(g_fake_yield));

    g_fake_iic.pf_IIC_Init        = Fake_IIC_Init;
    g_fake_iic.pf_CriticalEnter   = Fake_CriticalEnter;
    g_fake_iic.pf_CriticalExit    = Fake_CriticalExit;
    g_fake_iic.pf_IIC_Start       = Fake_IIC_Start;
    g_fake_iic.pf_IIC_Stop        = Fake_IIC_Stop;
    g_fake_iic.pf_IIC_WaitACK     = Fake_IIC_WaitACK;
    g_fake_iic.pf_IIC_SendACK     = Fake_IIC_SendACK;
    g_fake_iic.pf_IIC_SendNACK    = Fake_IIC_SendNACK;
    g_fake_iic.pf_IIC_SendByte    = Fake_IIC_SendByte;
    g_fake_iic.pf_IIC_ReceiveByte = Fake_IIC_ReceiveByte;

    g_fake_yield.pf_RTOS_Yield = Fake_RTOS_Yield;
}

void tearDown(void)
{
}




int TestUnity(void)
{


    
    UNITY_BEGIN();
/* 修正：使用RUN_TEST，而非裸函数 */
    RUN_TEST(TestAht21bDriverInst_ShouldMountDependenciesAndOperations);
    
    RUN_TEST(TestAht21b_Init_DeInit);
    // RUN_TEST(TestAht21b_DeInit);
    RUN_TEST(TestAht21b_GetAht21bID);
    RUN_TEST(TestAht21b_pfAht21b_ReadTempHumi);
    RUN_TEST(TestAht21b_pfAht21b_ReadHumi);
    RUN_TEST(TestAht21b_pfAht21b_Sleep);
    RUN_TEST(TestAht21b_pfAht21b_WakeUp);

    return UNITY_END();
}



