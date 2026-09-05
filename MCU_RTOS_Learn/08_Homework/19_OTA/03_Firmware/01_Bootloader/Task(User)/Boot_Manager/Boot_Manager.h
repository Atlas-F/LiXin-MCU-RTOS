/**
 * 
 * @file   Boot_Manager.h
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

#ifndef __BOOT_MANAGER_H__
#define __BOOT_MANAGER_H__

#include <stdint.h>
#include "Flash.h"



extern uint32_t RamSource;

#define APPLICATION_ADDRESS             (0x8008000)
#define BACK_APPLICATION_ADDRESS        (0x8020000)     // 备份区地址

// #define NVIC_VECTOR_FLASH               ((uint32_t)0x8000000)
#define NVIC_VectTab_FLASH               ((uint32_t)0x8000000)





/* Private functions ---------------------------------------------------------*/
void JumpToApp(void);
// int8_t BackToApp( void );
 int8_t BackToApp(int32_t fl_size);


#endif /* __BOOT_MANAGER_H__ */

