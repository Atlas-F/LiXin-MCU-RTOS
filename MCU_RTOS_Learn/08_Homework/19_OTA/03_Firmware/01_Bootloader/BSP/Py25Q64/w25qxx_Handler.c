/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file W25Q_Handler.c
 * 
 * @par dependencies 
 * - W25Q_Handler.h
 * 
 * @author Jack | R&D Dept. | EternalChip ��оǶ��ʽ
 * 
 * @brief Functions related to reading and writing in the chip's flash area.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-09-13
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "w25qxx_Handler.h"
#include "sfud_adapter.h"  // 使用SFUD适配层
#include "sfud.h"  // 包含SFUD头文件
#include "w25qxx.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static st_W25Q_Handler s_ast_W25Q_Handler[2];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* extern variables ---------------------------------------------------------*/

/**
 * @brief  Set the Block Parmeter object
 * 
 * @param [in]  block_index 
 * @param [in]  app_size 
 * 
 * @details
 * @note
 * @warning
 */
void SetBlockParmeter( u8 block_index, int32_t app_size )
{
    s_ast_W25Q_Handler[block_index].write_index = app_size;
    s_ast_W25Q_Handler[block_index].write_databuf_index = app_size % BLOCK_SIZE;
    s_ast_W25Q_Handler[block_index].write_sector_index = app_size / BLOCK_SIZE;
    //s_ast_W25Q_Handler[block_index].write_sector_index = app_size / 1096;
    //s_ast_W25Q_Handler[block_index].write_databuf_index = app_size % 1096;
    // 同时设置SFUD适配层的块参数
    SFUD_SetBlockParmeter(block_index, app_size);
}

uint32_t Read_BlockSize(u8 block_index)
{
    // 从SFUD适配层读取块大小
    return SFUD_Read_BlockSize(block_index);
}

/**
 * @brief   Py25q64 初始化，两个分区都初始化
 * 
 * 
 * @details
 * @note
 * @warning
 */
void W25Q64_Init(void)
{
    //W25Qx_Init();
    // 使用SFUD初始化替换原有的W25Qx_Init
    SFUD_W25Q64_Init();
	
    s_ast_W25Q_Handler[0].read_index = 0;
    s_ast_W25Q_Handler[0].read_sector_index = 0;
    s_ast_W25Q_Handler[0].write_databuf_index = 0;
    s_ast_W25Q_Handler[0].write_index = 0;
    s_ast_W25Q_Handler[0].write_sector_index = 0;

    s_ast_W25Q_Handler[1].read_index = 0;
    s_ast_W25Q_Handler[1].read_sector_index = 0;
    s_ast_W25Q_Handler[1].write_databuf_index = 0;
    s_ast_W25Q_Handler[1].write_index = 0;
    s_ast_W25Q_Handler[1].write_sector_index = 0;
}


/**
 * @brief  读取 指定Block 大小
 * 
 * @param [in]  block_index 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
uint32_t ReadBlockSize(u8 block_index)
{
    return s_ast_W25Q_Handler[block_index].write_index ;

}


/**
 * @brief  擦除指定 flash 中的 block
 * 
 * @param [in]  block_index 
 * 
 * @details
 * @note
 * @warning
 */
void EraseFlashBlock(u8 block_index)
{
	// 使用SFUD擦除Flash块
    SFUD_Erase_Flash_Block(block_index);
	
	s_ast_W25Q_Handler[block_index].read_index = 0;
    s_ast_W25Q_Handler[block_index].read_sector_index = 0;
    s_ast_W25Q_Handler[block_index].write_databuf_index = 0;
    s_ast_W25Q_Handler[block_index].write_index = 0;
    s_ast_W25Q_Handler[block_index].write_sector_index = 0;

}

/**
 * @brief  Py25q64 芯片数据擦除
 * 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
u8 W25Q64_EraseChip(void)
{

    // 使用SFUD擦除整个芯片
    if(1 == SFUD_W25Q64_EraseChip())    //if(0 == W25Qx_Erase_Chip())
    {
        s_ast_W25Q_Handler[0].read_index = 0;
        s_ast_W25Q_Handler[0].read_sector_index = 0;
        s_ast_W25Q_Handler[0].write_databuf_index = 0;
        s_ast_W25Q_Handler[0].write_index = 0;
        s_ast_W25Q_Handler[0].write_sector_index = 0;

        s_ast_W25Q_Handler[1].read_index = 0;
        s_ast_W25Q_Handler[1].read_sector_index = 0;
        s_ast_W25Q_Handler[1].write_databuf_index = 0;
        s_ast_W25Q_Handler[1].write_index = 0;
        s_ast_W25Q_Handler[1].write_sector_index = 0;
        return 0;
    }
    return 1;
}

/**
 * @brief  Py25q64 数据写入，并针对指定的数据块进行写入
 * 
 * @param [in]  block_index     指定的数据块，
 * @param [in]  data 
 * @param [in]  length 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
u8 W25Q64_WriteData(u8 block_index, u8 *data, u32 length)
{
    //u8 ret = 0;
    u32 addr = 0;
    u16 index = 0;
    const u32 SUBSECTOR_SIZE = 4096;  // 4KB subsector size
    const u32 PAGE_SIZE = 256;        // 256 byte page size
    
    for(u16 i = 0; i < length; i++)
    {
        //1.����д�����ݻ���������
        index = s_ast_W25Q_Handler[block_index].write_databuf_index;
        s_ast_W25Q_Handler[block_index].databuf[index] = *(data + i);
        s_ast_W25Q_Handler[block_index].write_databuf_index++;
        //2.�ж�������û��д��4096
				//if(s_ast_W25Q_Handler[block_index].write_databuf_index == W25Qx_Para.SUBSECTOR_SIZE)
        if(s_ast_W25Q_Handler[block_index].write_databuf_index == SUBSECTOR_SIZE)
        {
            s_ast_W25Q_Handler[block_index].write_databuf_index = 0;
            //����1��sector
            addr = SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index;
			//addr = W25Qx_Para.SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index;
            addr += block_index * BLOCK_SIZE ;      // 这里为什么要偏移一个 block ？
            //W25Qx_Erase_Block(addr);
            //W25Qx_WriteEnable();
            // 使用SFUD写入整个扇区数据
            uint32_t write_addr = (SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index) + (block_index * BLOCK_SIZE);

            //for(u8 j = 0; j < 16; j++)
			// 直接使用SFUD的擦除写入功能
            if(SFUD_SUCCESS == sfud_erase_write(sfud_get_device(SFUD_W25Q64_DEVICE_INDEX), write_addr, SUBSECTOR_SIZE, s_ast_W25Q_Handler[block_index].databuf))
            {
				s_ast_W25Q_Handler[block_index].write_sector_index++;
				s_ast_W25Q_Handler[block_index].write_index += SUBSECTOR_SIZE;
                //��ȡ��ǰд���ַ
                //addr = (W25Qx_Para.SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index) + \
                //        (W25Qx_Para.PAGE_SIZE * j);
                //addr += block_index * BLOCK_SIZE;
                //Ö´ÐÐÐ´Èë²Ù×÷
                //index = W25Qx_Para.PAGE_SIZE * j;
                //W25Qx_Write(&s_ast_W25Q_Handler[block_index].databuf[index],addr,W25Qx_Para.PAGE_SIZE);
            }
            //s_ast_W25Q_Handler[block_index].write_sector_index++;
            //��¼�ܵ�д�����ݳ���
            //s_ast_W25Q_Handler[block_index].write_index += W25Qx_Para.SUBSECTOR_SIZE;
        }
    }
    return 0;
}

/**
 * @brief  Py25q64 最后遗留不满一个块的数据写入
 * 
 * @param [in]  block_index 
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
u8 W25Q64_WriteData_End(u8 block_index)
{
    if(0 != s_ast_W25Q_Handler[block_index].write_databuf_index)
    {
        // 使用SFUD写入剩余的数据
        uint32_t write_addr = (4096 * s_ast_W25Q_Handler[block_index].write_sector_index) + (block_index * BLOCK_SIZE);
        
        if(SFUD_SUCCESS == sfud_erase_write(sfud_get_device(SFUD_W25Q64_DEVICE_INDEX), write_addr, 
                                           s_ast_W25Q_Handler[block_index].write_databuf_index, 
                                           s_ast_W25Q_Handler[block_index].databuf))
        {
            s_ast_W25Q_Handler[block_index].write_index += s_ast_W25Q_Handler[block_index].write_databuf_index;
            s_ast_W25Q_Handler[block_index].write_databuf_index = 0;
        }
    }
    return 0;


    // u32 addr = 0;
    // u16 index = 0;
    // u8  page_size = 0;
    
    // //uint8_t Ymodem_Receive_3 = 3000 ;

    // //�жϻ���û��ʣ������û��ִ��д��
    // if(0 != s_ast_W25Q_Handler[block_index].write_databuf_index)
    // {
    //     //д�����
    //     page_size = s_ast_W25Q_Handler[block_index].write_databuf_index / W25Qx_Para.PAGE_SIZE;
    //     //��ִ�в�������
    //     addr = W25Qx_Para.SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index;
    //     addr += block_index * BLOCK_SIZE;   /* 这里缺失代码，会出问题，记得复盘这里代码的作用 */
    //     W25Qx_Erase_Block(addr);
    //     W25Qx_WriteEnable();
    //     for(u8 j = 0; j < page_size; j++)
    //     {
    //         //��ȡ��ǰд���ַ
    //         addr = (W25Qx_Para.SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index) + \
    //                 (W25Qx_Para.PAGE_SIZE * j);
    //         addr += block_index * BLOCK_SIZE ;
    //         //ִ��д�����
    //         index = W25Qx_Para.PAGE_SIZE * j;
    //         W25Qx_Write(&s_ast_W25Q_Handler[block_index].databuf[index],addr,W25Qx_Para.PAGE_SIZE);
    //     }

    //     //��û��С��256��������
    //     if(0 != (s_ast_W25Q_Handler[block_index].write_databuf_index % W25Qx_Para.PAGE_SIZE))
    //     {
    //         //��ȡ��ǰд���ַ
    //         addr = (W25Qx_Para.SUBSECTOR_SIZE * s_ast_W25Q_Handler[block_index].write_sector_index) + \
    //                 (W25Qx_Para.PAGE_SIZE * page_size);
    //         addr += block_index * BLOCK_SIZE ;
    //         //ִ��д�����
    //         index = W25Qx_Para.PAGE_SIZE * page_size;
    //         W25Qx_Write(&s_ast_W25Q_Handler[block_index].databuf[index],addr, \
    //                     s_ast_W25Q_Handler[block_index].write_databuf_index % W25Qx_Para.PAGE_SIZE);
    //     }
    //     s_ast_W25Q_Handler[block_index].write_index += s_ast_W25Q_Handler[block_index].write_databuf_index;
    //     //uint8_t Ymodem_Receive_4 = 4000 ;

    // }
    // return 0;
}

/*ÿ�ζ�ȡһ�����������ݣ��ⲿ�ӿ���Ҫһ��4096��buffer
length:ÿ�ζ�ȡ�����ĳ��ȣ��ӻ�������ȡ
return : 
        0:��ʾ��ȡ�ɹ�
        1:��ȡ���  û��������
        2:��ȡʧ��  ��ȡ����
*/

/**
 * @brief  Py25q64 数据写入，
 * 
 * @param [in]  block_index     指定的数据块
 * @param [in]  data            数据
 * @param [in]  length          数据长度
 * @return      
 * 
 * @details
 * @note
 * @warning
 */
u8 W25Q64_ReadData(u8 block_index, u8 *data, u16 *length)
{
    //u8 ret = 0;
    u32 addr = 0;
    const u32 SUBSECTOR_SIZE = 4096;  // 4KB subsector size
    //u16 index = 0;
    //u8  page_size = 0;
    //u16  remain_size = 0;

    //1.���ж��Ƿ��ȡ���
    if(s_ast_W25Q_Handler[block_index].write_index > s_ast_W25Q_Handler[block_index].read_index)
    {
        //�ж��������Ƿ�4K
        if(s_ast_W25Q_Handler[block_index].write_sector_index > s_ast_W25Q_Handler[block_index].read_sector_index)
        {
            //��4K���ݵĲ���
            *length = SUBSECTOR_SIZE;
            addr = s_ast_W25Q_Handler[block_index].read_sector_index * SUBSECTOR_SIZE;
            addr += block_index * BLOCK_SIZE;
            // 使用SFUD读取数据
            uint32_t read_addr = s_ast_W25Q_Handler[block_index].read_sector_index * SUBSECTOR_SIZE + (block_index * BLOCK_SIZE);
            if(SFUD_SUCCESS != sfud_read(sfud_get_device(SFUD_W25Q64_DEVICE_INDEX), read_addr, *length, data))

            // *length = W25Qx_Para.SUBSECTOR_SIZE;
            // addr = s_ast_W25Q_Handler[block_index].read_sector_index * W25Qx_Para.SUBSECTOR_SIZE;
            // addr += block_index * BLOCK_SIZE ;
            // if(0 != W25Qx_Read(data,addr,*length))
                return 2;
            s_ast_W25Q_Handler[block_index].read_sector_index++;
        }
        else
        {
            //��4K���ڵ�����
            *length = s_ast_W25Q_Handler[block_index].write_index - s_ast_W25Q_Handler[block_index].read_index;
            addr = s_ast_W25Q_Handler[block_index].read_sector_index * SUBSECTOR_SIZE;
            addr += block_index * BLOCK_SIZE;
            // 使用SFUD读取数据
            uint32_t read_addr2 = s_ast_W25Q_Handler[block_index].read_sector_index * SUBSECTOR_SIZE + (block_index * BLOCK_SIZE);
            if(SFUD_SUCCESS != sfud_read(sfud_get_device(SFUD_W25Q64_DEVICE_INDEX), read_addr2, *length, data))

            // *length = s_ast_W25Q_Handler[block_index].write_index - s_ast_W25Q_Handler[block_index].read_index;
            // addr = s_ast_W25Q_Handler[block_index].read_sector_index * W25Qx_Para.SUBSECTOR_SIZE;
            // addr += block_index * BLOCK_SIZE;
            // if(0 != W25Qx_Read(data,addr,*length))
                return 2;
        }
        s_ast_W25Q_Handler[block_index].read_index += *length;
        return 0;
    }
    else
    {
        return 1;
    }
}

