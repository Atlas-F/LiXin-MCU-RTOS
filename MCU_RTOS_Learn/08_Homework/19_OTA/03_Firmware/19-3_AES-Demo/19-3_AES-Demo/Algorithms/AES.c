/**
 * 
 * @file   AES.c
 * @brief  
 * 
 * @author AtlasFg
 * @date   2026-09-01
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

/* Private Includes ***************************************************************/
#include "AES.h"
#include "string.h"
#include "usart.h"

#include <stdint.h>  // uint8_t
#include <stddef.h>  // size_t
#include <stdio.h>   // printf

/* Private Define   ***************************************************************/

/* 兼容 C51  */
#ifndef __C51__
    #define code
    #define data
    #define idata
    #define xdata
    #define pdata
    typedef unsigned char BOOL ;
#else
    typedef bit BOOL ;
#endif

/**
 * @brief  Nk 表示AES密钥的长度(单位是字 , 4字节/字)。
 *              假设AES_KEY_LENGTH为128、192或256,则Nk分别为4、65、8
 * @note
 * @warning
 */
#define Nk      (AES_KEY_LENGTH/32)

/**
 * @brief  Nb 表示数据块大小(单位是字),固定为4。
 *          这是AES算法的固定标准,AES处理的是4x4的字节块。
 * 
 */
#define Nb      (4)
 

/**
 * @brief  定义加密轮数
 * @details
 * @note
 * @warning
 */
#if     AES_KEY_LENGTH == eAES_KEY_LENGTH_128
    #define Nr  10
#elif   AES_KEY_LENGTH == eAES_KEY_LENGTH_192
    #define Nr  12
#elif   AES_KEY_LENGTH == eAES_KEY_LENGTH_256
    #define Nr  14
#else
    #error  AES_KEY_LENGTH must be 128, 192 or 256 BOOLs!
#endif


/**
 * @brief  BPOLY用于AES中有限域(GF(2^8))运算。其值0x1B表示GH(2^8)中的不可约多项式
 *          (x^8+x^4+×+1)的低8位。
 *          在AES的 MixColumns操作中,有限域多项式BPOLY被用作乘法的模,确保运算在8位内循环。
 * @details
 * @note
 * @warning
 */
#define BPOLY 0X1B

/** 子密钥表 */
static xdata unsigned char g_roundKeyTable[ 4*Nb*(Nr+1)] ;

#include <stdint.h>

// static const uint8_t AES_SBOX[16][16] 
/**
 * @brief  加密用的 SBox
 * 
 */
static code const unsigned char AES_SBox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};


/**
 * @brief  解密用的 SBox
 * 
 */
static const uint8_t AES_INV_SBOX[256] = {
    /* 0x00 */ 0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    /* 0x10 */ 0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    /* 0x20 */ 0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    /* 0x30 */ 0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    /* 0x40 */ 0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    /* 0x50 */ 0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    /* 0x60 */ 0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    /* 0x70 */ 0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    /* 0x80 */ 0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    /* 0x90 */ 0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    /* 0xA0 */ 0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    /* 0xB0 */ 0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    /* 0xC0 */ 0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    /* 0xD0 */ 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    /* 0xE0 */ 0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    /* 0xF0 */ 0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/**
 * @brief  定义一个静态函数，旋转输入的字节
 * 
 * @param [in]  pWord  要右移的 4 字节数据
 * 
 * @details
 * @note
 * @warning
 */
static void RotationWord(unsigned char * pWord)
{
    if(NULL == pWord )
        return ;
    unsigned char temp = pWord[0];

    pWord[0] = pWord[1] ;
    pWord[1] = pWord[2] ;
    pWord[2] = pWord[3] ;

    // pWord[0] = temp ;
    pWord[3] = temp ;

}   

/**
 * @brief  批量异或两组数据
 * 
 * @param [in]  pData1  要异或的第一组数据，同时用于存储异或结果
 * @param [in]  pData2  要异或的第二组数据
 * @param [in]  nCount  要异或的数据字节长度
 * 
 * @details
 * @note        调用者需要确保 pData1 和 pData2 均至少有nCount个字节，否则可能导致访问越界
 * @warning
 */
static void XOR_Bytes(unsigned char * pData1, const unsigned char * pData2, unsigned char nCount)
{
    if( NULL == pData1 || NULL == pData2)
        return  ;

    for ( unsigned char i = 0 ; i < nCount ; i++)
    {
        pData1[i] ^= pData2[i];
    }    
}

/**
 * @brief  轮密钥加，将中间状态数据与子密钥进行异或操作
 * 
 * @details
 * @note
 * @warning
 */
#define     ADD_ROUND_KEY( pState, pRoundKey)       XOR_Bytes(pState,pRoundKey, 4*Nb)


/**
 * @brief  使用 S 盒对状态数据进行字节替换
 * 
 * @param [in]  pState  状态数据
 * @param [in]  nCount  状态数据的长度
 * @param [in]  bInvert 
 * 
 * @details     此函数将状态数据中的每个字节使用 S-盒 进行替换，可选择使用反向 S-盒（解密时使用）
 * @note
 * @warning
 */
static void SubBytes(unsigned char * pState, unsigned char nCount, BOOL bInvert)
{
    unsigned char i;        // 循环索引
    const unsigned char code * pSBox = bInvert ? AES_INV_SBOX : AES_SBox ;
    for( i = 0 ; i < nCount; i++ )
    {
        pState[i] = pSBox[pState[i]];
    }

}

/**
 * @brief  对状态数据进行移位操作
 * 
 * @param [in]  pState  状态数据，经过移位后更新为新的状态数据
 * @param [in]  bInvert 指示是否进行反向移位
 * 
 * @details 此函数根据 AES 算法的规定，将状态数据中的每一行移位，
 *          可选择反向移位（解密时使用）
 * @note    状态数据以列为主进行存储，调用者需要确保 pState 不为空
 * @warning
 */
static void ShiftRows(unsigned char *pState, BOOL bInvert)
{
    unsigned char r, c;
    unsigned char temp ;
    unsigned char rowData[4];

    for( r = 1 ;r < 4; r++)
    {
        for( c = 0; c < 4; c++ )
        {   
            rowData[c] = pState[r + 4 * c];
        }
        temp = bInvert ? (4-r) : r;
        for( c = 0; c < 4;c++ )
        {
            pState[r + 4 * c] = rowData[(c + temp)%4];
        }
    }
}

/**
 * @brief  在 GF（2^8）域内进行乘以2的运算
 * 
 * @param [in]  num 需要乘以2的数（8位无符号整数）
 * @return      
 * 
 * @details 该函数实现GF(2^8)的乘2运算。根据有限域的定义
            当乘数的最高位为0时,直接左移一位;当最高位为1时,
            左移后需要与多项式 BPOLY进行异或运算。
 * @note
 * @warning
 */
static unsigned char GF_Mult_By02(unsigned char num)
{
    if( (num & 0x80) == 0 )
    {
        num = num << 1;
    }
    else
    {
        num = (num << 1) ^ BPOLY;
    }
    return num ;
}

/**
 * @brief  混合状态的各列数据
 * 
 * @param [in]  pState  状态数据（每列包含四个字节）
 * @param [in]  bInvert 是否执行反向混合（用于解密）
 * 
 * @details 该函数实现 AES 加密算法中的列混合操作，对于每一列的状态数据
 *          进行一定的线性变换以增加数据的扩散性。在解密时，该过程需要反向进行
 * @note
 * @warning
 */
static void MixColumns(unsigned char *pState, BOOL bInvert)
{
    unsigned char i ;
    unsigned char temp ;
    unsigned char a0Pa2_M4;
    unsigned char a1Pa3_M4;
    unsigned char result[4];

    // 对每一列进行混合操作
    for (i = 0; i < 4; i++, pState += 4)
    {
        // 计算 b0 = 2a0 + 3a1 + a2 + a3
        // temp 是所有四个字节的异或结果
        temp = pState[0] ^ pState[1] ^ pState[2] ^ pState[3];

        // 计算混合后的每个字节
        result[0] = temp ^ pState[0] ^
                    GF_Mult_By02((unsigned char)(pState[0] ^ pState[1]));

        result[1] = temp ^ pState[1] ^
                    GF_Mult_By02((unsigned char)(pState[1] ^ pState[2]));

        result[2] = temp ^ pState[2] ^
                    GF_Mult_By02((unsigned char)(pState[2] ^ pState[3]));

        result[3] = temp ^ pState[3] ^
                    GF_Mult_By02((unsigned char)(pState[3] ^ pState[0]));

        // 如果是解密，进行反向混合操作
        if (bInvert)
        {
            // 反向混合：b0' = 14a0 + 11a1 + 13a2 + 9a3
            a0Pa2_M4 =
                GF_Mult_By02(
                    GF_Mult_By02(
                        (unsigned char)(pState[0] ^ pState[2])
                    )
                );  // 计算 4(a0 + a2)

            a1Pa3_M4 =
                GF_Mult_By02(
                    GF_Mult_By02(
                        (unsigned char)(pState[1] ^ pState[3])
                    )
                );  // 计算 4(a1 + a3)

            temp =
                GF_Mult_By02(
                    (unsigned char)(a0Pa2_M4 ^ a1Pa3_M4)
                );  // 计算 2(a0 + a2 + a1 + a3)

            // 更新结果
            result[0] ^= temp ^ a0Pa2_M4;
            result[1] ^= temp ^ a1Pa3_M4;
            result[2] ^= temp ^ a0Pa2_M4;
            result[3] ^= temp ^ a1Pa3_M4;
        }

        // 将混合后的结果写回状态数据
        memcpy(pState, result, 4);
    }
}

/**
 * @brief  对单块数据进行加密
 * 
 * @param [in]  pState  状态数据（每一个字节表示一个状态值）
 * 
 * @details     此函数实现AES加密算法的基本机构，对给定的状态数据进行加密处理，
 *              加密过程中包含多个步骤，包括轮密钥添加、字节替换、行移位和列混合
 *              最后，返回加密后的状态数据
 * @note
 * @warning
 */
static void Block_Encrypt(unsigned char *pState)
{
    unsigned char i;  // 循环索引

    // 添加初始轮密钥
    ADD_ROUND_KEY(pState, g_roundKeyTable);

    // 进行 Nr 轮加密操作，i 从 1 到 Nr
    for (i = 1; i <= Nr; i++)  // i = [1, Nr]
    {
        // 替换字节
        SubBytes(pState, 4*Nb, 0);

        // 行移位
        ShiftRows(pState, 0);

        // 只有在最后一轮前，才进行列混合
        if (i != Nr)
        {
            MixColumns(pState, 0);
        }

        // 添加当前轮的轮密钥
        ADD_ROUND_KEY(pState, &g_roundKeyTable[4*Nb*i]);
    }

    // 为了节省代码，合并到循环执行
    // SubBytes(pState, 4*Nb);
    // ShiftRows(pState, 0);
    // ADD_ROUND_KEY(pState, &g_roundKeyTable[4*Nb*Nr]);
}

/**
 * @brief  对单块数据进行解密
 * 
 * @param [in]  pState 
 * 
 * @details     此函数实现AES解密算法的基本机构，对给定的状态数据进行加密处理，
 *              解密过程中包含多个步骤，包括轮密钥添加、字节替换、行移位和列混合
 *              最后，返回解密后的状态数据
 * @note
 * @warning
 */
static void Block_Decrypt(unsigned char *pState)
{
    unsigned char i;  // 循环索引

    // 添加最后一轮的轮密钥
    ADD_ROUND_KEY(pState, &g_roundKeyTable[4*Nb*Nr]);

    // 进行 Nr 轮解密操作，i 从 Nr 到 1
    for (i = Nr; i > 0; i--)  // i = [Nr, 1]
    {
        // 反向行移位
        ShiftRows(pState, 1);

        // 反向替换字节
        SubBytes(pState, 4*Nb, 1);

        // 添加当前轮的轮密钥
        ADD_ROUND_KEY(pState, &g_roundKeyTable[4*Nb*(i-1)]);

        // 只有在最后一轮前，才进行列反混合
        if (i != 1)
        {
            MixColumns(pState, 1);
        }
    }

    // 为了节省代码，合并到循环执行
    // ShiftRows(pState, 1);
    // SubBytes(pState, 4*Nb, 1);
    // ADD_ROUND_KEY(pState, g_roundKeyTable);
}

/**
 * @brief  初始化 AES 算法，执行扩展密钥操作
 * 
 * @param [in]  pKey 指向原始密钥的指针，长度必须是 16、24、32 字节
 * 
 * @details 此函数根据提供的原始密钥生成轮密钥表，轮密钥将在后续的加密和解密过程中使用
 *          输入的原始密钥长度必须是 AES_KEY_LENGTH / 8 字节，函数根据 AES 标准扩展密钥
 * @note
 * @warning
 */
void AES_Init(const void *pKey)
{
    // 扩展密钥
    unsigned char i;                         // 循环索引
    unsigned char *pRoundKey;                // 指向当前轮密钥的指针
    unsigned char Rcon[4] = {0x01, 0x00, 0x00, 0x00};  // 轮常量初始值

    // 将原始密钥复制到轮密钥表的开始部分
    memcpy(g_roundKeyTable, pKey, 4 * Nk);

    // 设置指向当前轮密钥的指针
    pRoundKey = &g_roundKeyTable[4 * Nk];

    // 生成从第 Nk 到 Nb*(Nr+1) 的轮密钥
    for (i = Nk; i < Nb * (Nr + 1); pRoundKey += 4, i++)
    {
        // 将前一个轮密钥复制到当前轮密钥
        memcpy(pRoundKey, pRoundKey - 4, 4);

        // 每 Nk 轮的进行特殊处理
        if (i % Nk == 0)
        {
            // 旋转当前轮密钥
            RotationWord(pRoundKey);

            // 对轮密钥进行字节替换
            SubBytes(pRoundKey, 4, 0);

            // 将轮常量与当前轮密钥异或
            XOR_Bytes(pRoundKey, Rcon, 4);

            // 更新轮常量
            Rcon[0] = GF_Mult_By02(Rcon[0]);
        }
        // 针对 Nk 大于 6 的情况进行字节替换
        else if (Nk > 6 && i % Nk == Nb)
        {
            SubBytes(pRoundKey, 4, 0);
        }

        // 与 Nk 前的轮密钥异或，生成新的轮密钥
        XOR_Bytes(pRoundKey, pRoundKey - 4 * Nk, 4);
    }
}

/**
 * @brief  AES 加密函数，对输入的明文数据进行加密
 * 
 * @param [in]  pPlainText  明文数据,需加密的数据,长度为 nDataLen字节。
 * @param [in]  pCipherText 密文数据,加密后的数据存储地址,允许与 pPlainText相同。
 * @param [in]  nDataLen    数据长度,单位为字节,必须为AES_KEY_LENGTH/8的整数倍。
 * @param [in]  pIV         初始化向量(IV),如果使用ECB模式可以为NULL。
 * 
 * @details 该函数可以在ECB或CBC模式下执行AES加密。对于CBCC模式,需要提供初始化向量(IV)
            而ECB模式不需要IV。在加密过程中,数据块会逐一加密,输出密文块。
 * @note
 * @warning
 */
void AES_Encrypt(const unsigned char *pPlainText,
                 unsigned char *pCipherText,
                 unsigned int nDataLen,
                 const unsigned char *pIV)
{
    unsigned int i;

    // 若密文和明文指针不同，将明文复制到密文存储区
    if (pPlainText != pCipherText)
    {
        memcpy(pCipherText, pPlainText, nDataLen);
    }

    // 分块加密，每个块大小为 4 * Nb 字节
    for (i = nDataLen / (4 * Nb);
         i > 0;
         i--, pCipherText += 4 * Nb)
    {
        // 若使用 CBC 模式，进行块 XOR 操作
#if AES_ENCRYPTION_MODE == eAES_ENCRYPT_CBC
        XOR_Bytes(pCipherText, pIV, 4 * Nb);
#endif

        // 对当前块进行加密
        Block_Encrypt(pCipherText);

        // 更新 IV 为当前块密文，用于下一块加密
        pIV = pCipherText;
    }
}

/**
 * @brief  AES 解密函数，对输入的密文数据进行解密
 * 
 * @param [in]  pCipherText 密文数据,需解密的数据,长度为 nDataLen字节。
 * @param [in]  pPlainText  明文数据,解密后的数据存储地址,可以与 pCipherText 相同。
 * @param [in]  nDataLen    数据长度,单位为字节,必须为AES_KEY_LENGTH/8的整教倍。
 * @param [in]  pIV         初始化向量(IV),如果使用ECB模式可以为NULL。
 * 
 * @details     该函数支持ECB和CBC模式解密。对于CBC模式,需提供共初始化向量(IV)
                在解密过程中,数据块会逆序逐一解密并恢复到原始明文数据an
 * @note
 * @warning
 */
void AES_Decrypt(const unsigned char *pCipherText,
                 unsigned char *pPlainText,
                 unsigned int nDataLen,
                 const unsigned char *pIV)
{
    unsigned int i;

    // 若密文和明文指针不同，将密文复制到明文存储区
    if (pPlainText != pCipherText)
    {
        memcpy(pPlainText, pCipherText, nDataLen);
    }

    // 设置指针从最后一个数据块开始解密，避免开辟额外空间保存 IV
    pPlainText += nDataLen - 4 * Nb;

    for (i = nDataLen / (4 * Nb);
         i > 0;
         i--, pPlainText -= 4 * Nb)
    {
        // 对当前数据块进行解密
        Block_Decrypt(pPlainText);

        // 若为 CBC 模式，对解密后的数据块进行 XOR 操作
#if AES_ENCRYPTION_MODE == eAES_ENCRYPT_CBC
        if (i == 1)
        {
            // 第一块（初始块），与 IV 异或
            XOR_Bytes(pPlainText, pIV, 4 * Nb);
        }
        else
        {
            // 其余块，与前一个密文块异或
            XOR_Bytes(pPlainText, pPlainText - 4 * Nb, 4 * Nb);
        }
#endif
    }
}

/**
 * @brief       16 进制数据打印
 * 
 * @param [in]  data 
 * @param [in]  length 
 * 
 * @details
 * @note
 * @warning
 */
void print_hex(uint8_t * inputdata, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x", inputdata[i]);
    }

    printf(" Hexdata\n");
}

/**
 * @brief       AES 测试函数
 * 
 * 
 * @details
 * @note
 * @warning
 */
void test(void)
{
    uint8_t buf[16] = "11aa22bb33cc44dd";   // Eternal-Chip  11aa22bb33cc44dd
    uint8_t saveBuf[16], descryptBuf[17];
    uint8_t AES128key[17] = "123456789abcdefa";  // 秘钥
    uint8_t AES_IV[17] = "0102030405123456";     // 向量表

    AES_Init(AES128key);  // AES初始化

    AES_Encrypt(buf, saveBuf, sizeof(buf), AES_IV);
        printf("AES_Encrypt = ");
        print_hex(saveBuf, sizeof(saveBuf));
    AES_Decrypt(saveBuf, descryptBuf, sizeof(buf), AES_IV);
        printf("AES_Decrypt = %s \r\n", descryptBuf);

    // print_hex(saveBuf, sizeof(saveBuf));
    // printf("%s \r\n", descryptBuf);
    // printf(" DescryptBuf \r\n" );
}

void AES_IV_Key256bit_Decode(unsigned char *IV_IN_OUT, unsigned char *State_IN_OUT, unsigned char *key256bit)
{
    
}