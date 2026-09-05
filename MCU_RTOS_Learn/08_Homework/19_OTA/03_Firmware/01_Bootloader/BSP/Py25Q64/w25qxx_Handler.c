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
#include "w25qxx.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static st_W25Q_Handler s_st_W25Q_Handler_1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* extern variables ---------------------------------------------------------*/

void W25Q64_Init(void)
{
    W25Qx_Init();
    s_st_W25Q_Handler_1.read_index = 0;
    s_st_W25Q_Handler_1.read_sector_index = 0;
    s_st_W25Q_Handler_1.write_databuf_index = 0;
    s_st_W25Q_Handler_1.write_index = 0;
    s_st_W25Q_Handler_1.write_sector_index = 0;
}

u8 W25Q64_EraseChip(void)
{
    if(0 == W25Qx_Erase_Chip())
    {
        s_st_W25Q_Handler_1.read_index = 0;
        s_st_W25Q_Handler_1.read_sector_index = 0;
        s_st_W25Q_Handler_1.write_databuf_index = 0;
        s_st_W25Q_Handler_1.write_index = 0;
        s_st_W25Q_Handler_1.write_sector_index = 0;
        return 0;
    }
    return 1;
}

u8 W25Q64_WriteData(u8 *data, u32 length)
{
    u8 ret = 0;
    u32 addr = 0;
    u16 index = 0;
    for(u16 i = 0; i < length; i++)
    {
        //1.����д�����ݻ���������
        index = s_st_W25Q_Handler_1.write_databuf_index;
        s_st_W25Q_Handler_1.databuf[index] = *(data + i);
        s_st_W25Q_Handler_1.write_databuf_index++;
        //2.�ж�������û��д��4096
        if(s_st_W25Q_Handler_1.write_databuf_index == W25Qx_Para.SUBSECTOR_SIZE)
        {
            s_st_W25Q_Handler_1.write_databuf_index = 0;
            //����1��sector
            addr = W25Qx_Para.SUBSECTOR_SIZE * s_st_W25Q_Handler_1.write_sector_index;
            W25Qx_Erase_Block(addr);
            W25Qx_WriteEnable();
            //д����һ��sector��ִ��д�����   4096��byte
            for(u8 j = 0; j < 16; j++)
            {
                //��ȡ��ǰд���ַ
                addr = (W25Qx_Para.SUBSECTOR_SIZE * s_st_W25Q_Handler_1.write_sector_index) + \
                        (W25Qx_Para.PAGE_SIZE * j);
                //ִ��д�����
                index = W25Qx_Para.PAGE_SIZE * j;
                W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index],addr,W25Qx_Para.PAGE_SIZE);
            }
            s_st_W25Q_Handler_1.write_sector_index++;
            //��¼�ܵ�д�����ݳ���
            s_st_W25Q_Handler_1.write_index += W25Qx_Para.SUBSECTOR_SIZE;
        }
    }
    return 0;
}

u8 W25Q64_WriteData_End(void)
{
    u32 addr = 0;
    u16 index = 0;
    u8  page_size = 0;
    
    //uint8_t Ymodem_Receive_3 = 3000 ;

    //�жϻ���û��ʣ������û��ִ��д��
    if(0 != s_st_W25Q_Handler_1.write_databuf_index)
    {
        //д�����
        page_size = s_st_W25Q_Handler_1.write_databuf_index / W25Qx_Para.PAGE_SIZE;
        //��ִ�в�������
        addr = W25Qx_Para.SUBSECTOR_SIZE * s_st_W25Q_Handler_1.write_sector_index;
        W25Qx_Erase_Block(addr);
        W25Qx_WriteEnable();
        for(u8 j = 0; j < page_size; j++)
        {
            //��ȡ��ǰд���ַ
            addr = (W25Qx_Para.SUBSECTOR_SIZE * s_st_W25Q_Handler_1.write_sector_index) + \
                    (W25Qx_Para.PAGE_SIZE * j);
            //ִ��д�����
            index = W25Qx_Para.PAGE_SIZE * j;
            W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index],addr,W25Qx_Para.PAGE_SIZE);
        }

        //��û��С��256��������
        if(0 != (s_st_W25Q_Handler_1.write_databuf_index % W25Qx_Para.PAGE_SIZE))
        {
            //��ȡ��ǰд���ַ
            addr = (W25Qx_Para.SUBSECTOR_SIZE * s_st_W25Q_Handler_1.write_sector_index) + \
                    (W25Qx_Para.PAGE_SIZE * page_size);
            //ִ��д�����
            index = W25Qx_Para.PAGE_SIZE * page_size;
            W25Qx_Write(&s_st_W25Q_Handler_1.databuf[index],addr, \
                        s_st_W25Q_Handler_1.write_databuf_index % W25Qx_Para.PAGE_SIZE);
        }
        s_st_W25Q_Handler_1.write_index += s_st_W25Q_Handler_1.write_databuf_index;
        //uint8_t Ymodem_Receive_4 = 4000 ;

    }
    return 0;
}

/*ÿ�ζ�ȡһ�����������ݣ��ⲿ�ӿ���Ҫһ��4096��buffer
length:ÿ�ζ�ȡ�����ĳ��ȣ��ӻ�������ȡ
return : 
        0:��ʾ��ȡ�ɹ�
        1:��ȡ���  û��������
        2:��ȡʧ��  ��ȡ����
*/
u8 W25Q64_ReadData(u8 *data, u16 *length)
{
    u8 ret = 0;
    u32 addr = 0;
    u16 index = 0;
    u8  page_size = 0;
    u16  remain_size = 0;

    //1.���ж��Ƿ��ȡ���
    if(s_st_W25Q_Handler_1.write_index > s_st_W25Q_Handler_1.read_index)
    {
        //�ж��������Ƿ�4K
        if(s_st_W25Q_Handler_1.write_sector_index > s_st_W25Q_Handler_1.read_sector_index)
        {
            //��4K���ݵĲ���
            *length = W25Qx_Para.SUBSECTOR_SIZE;
            addr = s_st_W25Q_Handler_1.read_sector_index * W25Qx_Para.SUBSECTOR_SIZE;
            if(0 != W25Qx_Read(data,addr,*length))
                return 2;
            s_st_W25Q_Handler_1.read_sector_index++;
        }
        else
        {
            //��4K���ڵ�����
            *length = s_st_W25Q_Handler_1.write_index - s_st_W25Q_Handler_1.read_index;
            addr = s_st_W25Q_Handler_1.read_sector_index * W25Qx_Para.SUBSECTOR_SIZE;
            if(0 != W25Qx_Read(data,addr,*length))
                return 2;
        }
        s_st_W25Q_Handler_1.read_index += *length;
        return 0;
    }
    else
    {
        return 1;
    }
}