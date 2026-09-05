/**
 * 
 * @file   Boot_Manager.c
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

/* Includes ------------------------------------------------------------------*/
#include "Boot_Manager.h"
#include "main.h"
#include "elog.h"
/* Private typedef -----------------------------------------------------------*/

/* 定义类型 */
typedef void (*pFunction)(void);


/* Private define ------------------------------------------------------------*/

/* APP flash address */
//#define APPLICATION_ADDRESS             (0x8008000)
//// #define NVIC_VECTOR_FLASH               ((uint32_t)0x8000000)
//#define NVIC_VectTab_FLASH               ((uint32_t)0x8000000)


int32_t g_AppSize = 0 ;


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  从 Bootloader 跳转到 APP
 * 
 * 
 * @details
 * @note
 * @warning
 */
void JumpToApp(void)
{
  uint32_t JumpAddress;
  pFunction Jump_To_Application;

  /* 检查栈顶地址是否合法 */
  if(((*(__IO uint32_t *)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)
  {
    /* 屏蔽所有中断，防止在跳转过程中，中断干扰出现异常 */
    __disable_irq();
    /* 设置中断向量表的地址为新的 APP 的中断向量表的地址 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
    RCC_DeInit();

    /* 用户代码区第二个 字 为程序开始地址(复位地址) */
    JumpAddress = *(__IO uint32_t *) (APPLICATION_ADDRESS + 4);

    /* Initialize user application's Stack Pointer */
    /* 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址) */
    __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);

    /* 类型转换 */
    Jump_To_Application = (pFunction) JumpAddress;

    /* 跳转到 APP */
    Jump_To_Application();
  }
}


/* 向量数据，初始参数*/
unsigned char IV[16] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x39, 0x30, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66};

/* 密钥数据*/
unsigned char Key[32] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38};


 /**
  * @brief  将备份区的数据，解密并写入到运行区
  * 
  * @param [in]  fl_size 
  * @return      
  * 
  * @details
  * @note
  * @warning
  */

  u8  Mem_Read_buffer[4096];
int8_t BackToApp(int32_t fl_size)
{
    u8 Temp[16];  //原密文数据缓存
    u8 wirteTime=0;  //一个解析包写入次数
    u16 readTime=0,readDataCount=0;   //读取数据再解密的次数（每次解密16个字节）
    u32 AppSize=0;  //升级包的大小
    //u32 FlashDestination=ApplicationAddress;
    u16 Read_Memory_Size=0;
    u32 Read_Memory_index=0;
    uint8_t *pu8_IV_IN_OUT = IV;
    uint8_t *pu8_key256bit = Key;
    uint32_t RamSource = 0;
    uint32_t AppRunFlashDestination = APPLICATION_ADDRESS;
    if(fl_size <= 0)
    {
      return -1;
    }
    if ((g_AppSize > (0x18010 - 1)) ||\
    (g_AppSize < 0))
    {
      return -1;
    }
    //先读一帧，用来解析头文件格式
    W25Q64_ReadData(Mem_Read_buffer,&Read_Memory_Size);
    if(Read_Memory_Size  >= 16)
    {
      memcpy(Temp,Mem_Read_buffer,16);
      Aes_IV_key256bit_Decode(pu8_IV_IN_OUT,Temp,pu8_key256bit);//解析得到自定义内容+文件大小
      AppSize=(Temp[15]<<24)+(Temp[14]<<16)+(Temp[13]<<8)+Temp[12];
      log_d("AppSize=%d",AppSize);
      
      //计算升级包读取次数
      readDataCount=AppSize/16;
      if(AppSize%16!=0)
      {
        readDataCount+=1;
      }
      Read_Memory_index += 16;
    }

    //数据帧
    //将待写入区的内容擦除
    uint8_t flash_erase_state = Flash_erase(AppRunFlashDestination,AppSize);
    if(flash_erase_state == 0)
    {
      for(readTime=0;readTime<readDataCount;readTime++)
      {
        //判断下当前buffer下的数据是否读取完毕
        if(Read_Memory_index == Read_Memory_Size)
        {
          if(2 == W25Q64_ReadData(Mem_Read_buffer,&Read_Memory_Size))
          {
            log_d("Write_Flash_After_AES_Decode read extern buffer error");
            return -1;
          }
          Read_Memory_index = 0;
        }
        //拷贝16个数据
        memcpy(Temp,Mem_Read_buffer + Read_Memory_index,16);
        Read_Memory_index += 16;
        //解析16个数据
        Aes_IV_key256bit_Decode(pu8_IV_IN_OUT,Temp,pu8_key256bit);//解析
        
        RamSource = (uint32_t)Temp;
        for (wirteTime = 0;wirteTime<4;wirteTime++)
        {
          Flash_Write(AppRunFlashDestination, *(uint32_t*)RamSource);
          AppRunFlashDestination += 4;
          RamSource += 4;
        }
      }
      log_d("Write_Flash_After_AES_Decode end");
      return 0;
    }
    else
    {
      log_d("Write_Flash_After_AES_Decode Error");
      return -1;
    }
}



//  int8_t BackToApp(int32_t fl_size)
// {
//     uint32_t AppRunFlashDestination = APPLICATION_ADDRESS;
//     uint8_t *pu8_IV_IN_OUT = IV;
//     uint8_t *pu8_key256bit = Key;
//     uint8_t *pu8_temp = (uint8_t *)BACK_APPLICATION_ADDRESS;  // 原始数据 ，备份区地址
//     uint8_t Temp[16];  // 原密文数据缓存
//     uint8_t *pTemp = Temp;
//     uint16_t readTime = 0, readDataCount = 0;  // 读取数据再解密的次数（每次解密16个字节）
//     u32 AppSize = 0;  // 升级包的大小

//     if (fl_size <= 0)    // 判断文件的大小
//     {
//         return -1;
//     }

//     if ((g_AppSize > (0x18010 - 1)) ||      // 0x18010 是0x18000 加上一帧加密软件的辅助数据 16 字节
//         (g_AppSize < 0))
//     {
//         return -1;
//     }

//     // 擦除运行区数据

//     memcpy(pTemp, pu8_temp, 16);
//     pu8_temp += 16;
//     Aes_IV_key256bit_Decode(pu8_IV_IN_OUT, pTemp, pu8_key256bit);  // 解析得到自定义内容+文件大小
//     AppSize = (pTemp[15] << 24) + (pTemp[14] << 16) +
//               (pTemp[13] << 8) + pTemp[12];     /* 这里的四个数据，是由于加密后的文件的特殊格式，\
//                                                     加密后文件格式为 4字节自定义数据长度 + 自定义数据 + 4字节明文长度 + 明文数据
//                                                     这里自定义的数据长度为 8个字节 */

//                                                 // 解密后的数据是原地存储的吗？ 会，解密结果会覆盖原来的数据缓冲区，属于原地存储

//     readDataCount = AppSize / 16;   // 计算需要解密多少次？16个字节为一个数据块，一次解密
//     if (AppSize % 16 != 0)
//     {
//         readDataCount += 1;
//     }

//     if (1 == Flash_erase(APPLICATION_ADDRESS, AppSize))
//     {
//         return -1;
//     }

//     // 读数据的总次数
//     for (readTime = 0; readTime < readDataCount; readTime++)
//     {
//         // 加密原文读取16个字节到临时区中
//         pTemp = Temp;
//         memcpy(pTemp, pu8_temp, 16);
//         pu8_temp += 16;
//         Aes_IV_key256bit_Decode(pu8_IV_IN_OUT, pTemp, pu8_key256bit);  // 解密数据

//         // 解密后的数据存入App运行区中
//         for (uint8_t j = 0; j < 16; j += 4)
//         {
//             Flash_Write(AppRunFlashDestination, *(uint32_t *)pTemp);
//             if (*(uint32_t *)AppRunFlashDestination != *(uint32_t *)pTemp)
//             {
//                 return -1;
//             }

//             AppRunFlashDestination += 4;
//             pTemp += 4;
//         }
//     }

//     return 0;
// }



// /**
//  * @brief  将备份区的数据，写入到 APP 运行区
//  * 
//  * @details
//  * @note
//  * @warning
//  */
// int8_t BackToApp( void  )
// {
//     int32_t j = 0 ;
//     uint32_t FlashDestination = APPLICATION_ADDRESS;
//     uint32_t BackflashSource;

//     if( g_AppSize > (0x18000 -1 ) || g_AppSize < 0 )     // 因为这里设计的APP运行区域大小范围为96k
//     {
//         return ;
//     }

//     BackflashSource = BACK_APPLICATION_ADDRESS ;

//     for (int32_t j = 0 ; j < g_AppSize ;j += 4)  
//     {     
//         /* Program the data received into STM32F10x Flash */
//         //    FLASH_ProgramWord(FlashDestination, *(uint32_t*)RamSource);
//         Flash_Write(FlashDestination, *(uint32_t*)BackflashSource);
//         if (*(uint32_t*)FlashDestination != *(uint32_t*)BackflashSource)
//         {
//             /* End session */
//             return -1;
//         }
//         FlashDestination += 4;
//         BackflashSource += 4;
//     }

//     return 0 ;
// }
