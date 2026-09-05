/**
 * 
 * @file   Debug.c
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-08-26
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
#include "Debug.h"

#include "elog.h"
#include "stdio.h"

/* Private define ------------------------------------------------------------*/

#define TAG_MAIN    "MAIN"
#define TAG_UART    "UART"
#define TAG_MOTOR   "MOTOR"
#define TAG_SENSOR  "SENSOR"
#define TAG_SERIAL  "SERIAL"
#define TAG_TEST    "TEST"


 /**
 * @brief  应用层Elog 初始�???
 * 
 * 
 * @details
 * @note
 * @warning
 */
void AppEasylogInit(void)
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
 * @brief  Easylog 测试
 * 
 * 
 * @details
 * @note
 * @warning
 */
void EasylogTest(void)
{
    // uint32_t tick_current = 0 ;
    // tick_current = HAL_GetTick();

    log_a("this assert , ");
    log_e("this is error , ");
    log_w("this is warning , ");
    log_i("this is info , ");
    log_d("this is debug , ");
    log_v("this is verbose , ");

    elog_a(TAG_TEST,"this assert , ");
    elog_e(TAG_TEST,"this is error , ");
    elog_w(TAG_TEST,"this is warning , ");
    elog_i(TAG_TEST,"this is info , ");
    elog_d(TAG_TEST,"this is debug , ");
    elog_v(TAG_TEST,"this is verbose , ");
}

