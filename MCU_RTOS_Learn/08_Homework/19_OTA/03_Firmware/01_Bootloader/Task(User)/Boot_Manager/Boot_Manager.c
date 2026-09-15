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

#include "AT24Cxx_Driver.h"
#include "w25qxx_Handler.h"

/* Private typedef -----------------------------------------------------------*/

/* 定义类型 */
typedef void (*pFunction)(void);


/* Private define ------------------------------------------------------------*/

/* APP flash address */
//#define APPLICATION_ADDRESS             (0x8008000)
//// #define NVIC_VECTOR_FLASH               ((uint32_t)0x8000000)
//#define NVIC_VectTab_FLASH               ((uint32_t)0x8000000)


int32_t g_AppSize = 0 ;

uint8_t g_au8_test[1024] = {0} ;

/* 一次读取的block 的大小 */
u8 g_Mem_read_buffer[4096] = {0};       // 有类似的变量，需要核对，使用的是同一个，还是两个？

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
                        /* 1234567890abcdef */

/* 密钥数据*/
unsigned char Key[32] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38};
                        /* 12345678123456781234567812345678 */

/**
 * @brief 软件复位单片机
 * 
 * @param None
 * 
 * @return: None
 */
void System_SoftwareReset(void)
{
  __NVIC_SystemReset();
}


/**
 * @brief   Bootloader 中对于 OTA 的状态机管理
 * 
 * 
 * @details
 * @note
 * @warning
 */
void OTA_StateManager(void)
{
    /* 从eeprom 中读取标志位判断 ota 状态*/
    uint8_t ota_state = EEPROM_CHECK_NO_APP_UPDATE ;
    uint32_t current_app_size = 0 ;
    int32_t file_size = 0 ;
    int32_t origin_app_size = 0 ;      // 第一份APP代码大小

    ee_ReadBytes(&ota_state, 0x00, 1);
    USART_SendChar(USART1, ota_state);
    switch (ota_state)
    {
        case EEPROM_CHECK_NO_APP_UPDATE:
            if( KeyScan() )          // 能不能从这里写一个判断，只有当指定区域内存全F的时候，即被全片擦除的时候，此时会通过bootloader
            {                        // 下载第一份运行APP，然后才是从这里开始获取当前APP的大小                   
                // 下载到外部 flash 的 A 区
                file_size = Ymodem_Receive(g_au8_test);

                // 将备份区数据拷贝到外部 Flash B区中
                if( 0 == ExA_To_ExB_AES(file_size))     
                {
                    /* 将当前正在运行的APP的数据搬运到外部 Flash A 区, 进行备份，防止下载失败或者异常问题导致设备变砖 */
                    /* 增加一个代码回退功能 */
                    // 读取当前APP的大小
                    ee_ReadBytes((uint8_t *)&current_app_size, 0x05, 4);
                        
                    App_To_ExA(current_app_size);
                    /* 外部flash B区搬运到内部flash 区 */
                    ExB_To_App();
                    ota_state = EEPROM_APP_FIRST_CHECK_START ;
                    ee_WriteBytes(&ota_state,0x00,1);
                    //执行软件复位
                    System_SoftwareReset();

                        // /* 执行跳转*/
                        // JumpToApp();
                        // /* 校验数据是否合法，如果数据无效，将外部 Flash A 区中的数据搬运到 APP 中 */
                        // ExA_To_App();
                        // JumpToApp();

                }else
                {
                    log_a(" Boot download failed \n");
                    JumpToApp();
                        //App校验无效
                        ota_state = EEPROM_CHECK_NO_APP_UPDATE;
                        ee_WriteBytes(&ota_state,0x00,1);
                        /*软件复位*/
                        System_SoftwareReset();
                }
            }else
            {
                JumpToApp();
                    //App校验无效
                    ota_state = EEPROM_CHECK_NO_APP_UPDATE;
                    ee_WriteBytes(&ota_state,0x00,1);
                    /*软件复位*/
                    System_SoftwareReset();
            }
            break;
        case EEPROM_CHECK_APP_DOWNLOADING:
            log_a(" APP download failed ! \n ");
            JumpToApp();

            // 跳转失败，下载新 APP 
            log_a(" No Valid APP");
                // 下载到外部 flash 的 A 区
                /* 下载流程  */
                file_size = Ymodem_Receive(g_au8_test);
                // 将备份区数据拷贝到外部 Flash B区中
                if( 0 == ExA_To_ExB_AES(file_size))     // 0 == BackToApp()
                {
                    // 读取当前APP的大小
                    ee_ReadBytes((uint8_t *)&current_app_size, 0x05, 4);
                    App_To_ExA(current_app_size);
                    /* 外部flash B区搬运到内部flash 区 */
                    ExB_To_App();
                    //App校验无效
                    ota_state = EEPROM_APP_FIRST_CHECK_START;
                    ee_WriteBytes(&ota_state,0x00,1);
                    /*软件复位*/
                    System_SoftwareReset();
                        // /* 执行跳转逻辑 */
                        // JumpToApp();
                        // /* 校验数据是否合法，如果数据无效，将外部 Flash A 区中的数据搬运到 APP 中 */
                        // ExA_To_App();
                        // JumpToApp();

                }else
                {
                    log_a(" Boot download failed \n");
                    JumpToApp();
                }
                /* 下载流程  */

            /* 跳转失败，下载新的APP */
            // TODO :

            break;
        case EEPROM_CHECK_APP_DOWNLOAD_COMPLETE:
            /* 读取当前需要更新的APP大小 */
            ee_ReadBytes((uint8_t *)&current_app_size, 0x01, 4);
            /* APP 大小更新到内部flash 存储数据量的管理结构体中 */
            SetBlockParmeter(BLOCK_1, current_app_size);
            log_i(" Update APP size :[%d] ! \n ", current_app_size);

            // if( 0 == ExA_To_ExB_AES(file_size))     // 0 == BackToApp()
            if( 0 == ExA_To_ExB_AES(current_app_size))  
            {
                // 读取当前APP的大小
                ee_ReadBytes((uint8_t *)&current_app_size, 0x05, 4);
                log_i(" read current_app_size APP size :[%d] ! \n ", current_app_size);
                App_To_ExA(current_app_size);
                /* 外部flash B区搬运到内部flash 区 */
                ExB_To_App();       // 这里应该如果执行成功的话，执行前后的内存数据是不一样的
                    // /* 清除标志位 */
                    // int32_t clear_app_size = 0 ;
                    // ee_WriteBytes((uint8_t *)&clear_app_size, 0x00, 4); 
                ota_state = EEPROM_APP_FIRST_CHECK_START;
                ee_WriteBytes(&ota_state,0x00,1);
                //执行软件复位
                /*软件复位*/
                System_SoftwareReset();
                        // /* 执行跳转*/
                        // JumpToApp();
                        // /* 校验数据是否合法，如果数据无效，将外部 Flash A 区中的数据搬运到 APP 中 */
                        // ExA_To_App();
                        // JumpToApp();

            }else
            {
                log_a(" Boot download failed \n");
                JumpToApp();
                    //App校验无效
                    ota_state = EEPROM_CHECK_NO_APP_UPDATE;
                    ee_WriteBytes(&ota_state,0x00,1);
                    /*软件复位*/
                    System_SoftwareReset();
            }
            /* 解密搬运数据 */
            break;
        case EEPROM_APP_FIRST_CHECK_START:
            ota_state = EEPROM_APP_FIRST_CHECKING;
            ee_WriteBytes(&ota_state,0x00,1);
            /*开看门狗*/
            IWDG_Init(IWDG_Prescaler_64,3000);
            /*5.执行跳转逻辑*/
            JumpToApp();
            break;

        case EEPROM_APP_FIRST_CHECKING:
            //App校验无效
            ota_state = EEPROM_CHECK_NO_APP_UPDATE;
            ee_WriteBytes(&ota_state,0x00,1);
            /*软件复位*/
            System_SoftwareReset();
            /*如果运行到这一步，说明数据无效，把外部A区的数据搬运到App中*/
            //ExA_To_App();
            /*再执行一次跳转*/
            //JumpToApp();
            break;

        default:
            break;
    }

}


/**
 * @brief  将数据从 Flash A 区拷贝到 Flash B 区域
 *          需要进行 AES 解密
 * @param [in]  fl_size 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
int8_t ExA_To_ExB_AES(int32_t fl_size)
{
    if( 0 >= fl_size )
    {
        return -1 ;
    }

    u8 Temp[16];  //原密文数据缓存
    u8 wirteTime=0;  //一个解析包写入次数
    u16 readTime=0,readDataCount=0;   //读取数据再解密的次数（每次解密16个字节）
    u32 AppSize=0;  //升级包的大小
    u16 Read_Memory_Size=0;
    u32 Read_Memory_index=0;
    uint8_t *pu8_IV_IN_OUT = IV;
    uint8_t *pu8_key256bit = Key;
    uint32_t RamSource = 0;
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
    W25Q64_ReadData(BLOCK_1, g_Mem_read_buffer,&Read_Memory_Size);
    if(Read_Memory_Size  >= 16)
    {
        memcpy(Temp,g_Mem_read_buffer,16);
        Aes_IV_key256bit_Decode(pu8_IV_IN_OUT,Temp,pu8_key256bit);    //解析得到自定义内容+文件大小
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

    /* 擦除外部 flash 比较耗时*/
    for(readTime=0;readTime<readDataCount;readTime++)
    {
        //判断下当前buffer下的数据是否读取完毕
        if(Read_Memory_index == Read_Memory_Size)
        {
            if(2 == W25Q64_ReadData(BLOCK_1, g_Mem_read_buffer,&Read_Memory_Size))
            {
            log_d("Write_Flash_After_AES_Decode read extern buffer error");
            return -1;
            }
            Read_Memory_index = 0;
        }
        //拷贝16个数据
        memcpy(Temp,g_Mem_read_buffer + Read_Memory_index,16);
        Read_Memory_index += 16;
        //解析16个数据
        Aes_IV_key256bit_Decode(pu8_IV_IN_OUT,Temp,pu8_key256bit);//解析

        W25Q64_WriteData(BLOCK_2, Temp, 16);        // 这里错误：BLOCK_1

    }
    W25Q64_WriteData_End(BLOCK_2);
    log_d("Write_Flash_After_AES_Decode end");
    return 0;

}

/**
 * @brief  
 * 
 * @param [in]  fl_size 文件大小
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
int8_t App_To_ExA(int32_t fl_size)
{
    u32 FlashDes = APPLICATION_ADDRESS ;

    // 验证数据大小
    if ((fl_size > (0x18010 - 1)) || \
    (fl_size < 0))
    {
        return -1;
    }

    EraseFlashBlock(BLOCK_1);
    W25Q64_WriteData(BLOCK_1, (uint8_t *)FlashDes, fl_size);
    W25Q64_WriteData_End(BLOCK_1);
    return 0;
}


/**
 * @brief  外部 Flash B区 拷贝到当前运行 App 中
 * 
 * @return      
 * 
 * @details
 * @note        
 * @warning
 */
int8_t ExB_To_App(void)
{
    u32      FlashDes = APPLICATION_ADDRESS ;
    uint32_t flash_size = 0 ;
    u8       read_data_state = 0 ;
    u16      read_memory_size = 0 ;
    u32      ram_source = 0 ;      
    u16      write_time = 0 ;  

    flash_size = ReadBlockSize(BLOCK_2);

    if( 0 == Flash_erase(FlashDes, flash_size) )     /* 只擦除一个扇区吗？APP的程序占的内存空间不一定就只是一个扇区吧 ？*/
    {
        while(1)
        {
            read_data_state = W25Q64_ReadData(BLOCK_2, g_Mem_read_buffer, &read_memory_size);   // 这里的读取数据和具体的内存关系是什么？
            if( 1 == read_data_state)
            {
                break;
            }
            else if (2 == read_data_state)
            {
                /* 外部 Flash 读取数据有问题 */
                log_a(" Extern Flash read data failed \n");
                break;
            }
            else if( 0 == read_data_state)
            {
                ram_source = (u32)g_Mem_read_buffer ;
                /* 循环搬运数据 */
                for( write_time = 0 ; write_time < (read_memory_size/4); write_time++)
                {
                    // Flash_Write( FlashDes, ram_source);  // old wrong
                    Flash_Write( FlashDes, *(u32 * )ram_source);

                    FlashDes += 4 ;
                    ram_source += 4 ;
                }
            }
            
        }
        return flash_size ;
    }
    else{
        return -1 ;
    }
}

/**
 * @brief  将外部 Flash A 区的数据搬运到 APP
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
int8_t ExA_To_App(void)
{
    u32      FlashDes = APPLICATION_ADDRESS ;
    uint32_t flash_size = 0 ;
    u8       read_data_state = 0 ;
    u16      read_memory_size = 0 ;
    u32      ram_source = 0 ;      
    u16      write_time = 0 ;  

    flash_size = ReadBlockSize(BLOCK_1);

    if( 0 == Flash_erase(FlashDes, flash_size) )     /* 只擦除一个扇区吗？APP的程序占的内存空间不一定就只是一个扇区吧 ？*/
    {
        while(1)
        {
            read_data_state = W25Q64_ReadData(BLOCK_1, g_Mem_read_buffer, &read_memory_size);   // 这里的读取数据和具体的内存关系是什么？
            if( 1 == read_data_state)
            {
                break;
            }
            else if (2 == read_data_state)
            {
                /* 外部 Flash 读取数据有问题 */
                log_a(" Extern Flash read data failed \n");
                break;
            }
            else if( 0 == read_data_state)
            {
                ram_source = (u32)g_Mem_read_buffer ;
                /* 循环搬运数据 */
                for( write_time = 0 ; write_time < (read_memory_size/4); write_time++)
                {
                    // Flash_Write( FlashDes, ram_source);
                    Flash_Write( FlashDes, *(u32 * )ram_source);
                    FlashDes += 4 ;
                    ram_source += 4 ;
                }
            }
        }
        return flash_size ;
    }
    else{
        return -1 ;
    }
}





