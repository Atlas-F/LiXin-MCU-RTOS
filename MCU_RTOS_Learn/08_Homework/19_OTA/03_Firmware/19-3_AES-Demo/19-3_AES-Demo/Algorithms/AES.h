/**
 * 
 * @file   AES.h
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

#ifndef __AES_H__
#define __AES_H__


#include "usart.h"

/**
 * @brief  定义 AES 密钥长度枚举
 */
typedef enum
{
    eAES_KEY_LENGTH_128 = 128 ,
    eAES_KEY_LENGTH_192 = 192 ,
    eAES_KEY_LENGTH_256 = 256 
} AES_key_Length_t ;

#define AES_KEY_LENGTH      (eAES_KEY_LENGTH_128)   /* 定义 AES 密钥长度*/

/**
 * @brief 定义 AES 的加密模式  
 * 
 */
typedef enum
{
    eAES_ENCRYPT_ECB = 0 ,
    eAES_ENCRYPT_CBC = 1 
} AES_Encrypt_Mode_t ;

#define AES_ENCRYPTION_MODE     (eAES_ENCRYPT_CBC)


/**
 * @brief  AES  加密初始化，执行密钥扩展操作
 * 
 * @param [in]  pKey  指向原始密钥的指针
 * @retval 
 * 
 * @details
 * @note 密钥长度只能是 AES_KEY_LENGTH/8 个字节
 * @warning
 */
void AES_Init( const void * pKey );


/**
 * @brief  用于对给定的明文进行 AES 加密并生成密文
 * 
 * @param [in]  pPlainText      指向明文数据的指针
 * @param [in]  pCipherText     指向存储密文数据的缓冲区指针，数据长度为 nDatalen 字节
 * @param [in]  nDatalen         明文数据的数据长度 ，以字节为单位
 * @param [in]  pIV             指向初始化向量指针，如果使用 ECB 模式，设置为 NULL 
 * 
 * @details
 * @note        nDatalen 必须为 AES_KEY_LENGTH/8 的整倍数，如果不满足长度要求需要进行长度填充
 * @warning
 */
void AES_Encrypt( const unsigned char * pPlainText, unsigned char *pCipherText, 
                            unsigned int nDatalen, const unsigned char * pIV );

/**
 * @brief  用于对给定的明文进行 AES 解密并生成明文
 * 
 * @param [in]  pCipherText     指向密文数据的指针，数据长度为 nDatalen 字节
 * @param [in]  pPlainText      指向存储解密后明文数据的缓冲区指针，数据长度为 nDatalen 字节
 * @param [in]  nDatalen        明文数据的数据长度 ，以字节为单位
 * @param [in]  pIV             指向初始化向量指针，如果使用 ECB 模式，设置为 NULL 
 * 
 * @details
 * @note        nDatalen 必须为 AES_KEY_LENGTH/8 的整倍数，如果不满足长度要求需要进行长度填充
 * @warning
 */
void AES_Decrypt( const unsigned char * pCipherText, unsigned char *pPlainText,
                            unsigned int nDatalen, const unsigned char * pIV );

// static void RotationWord(unsigned char * pWord);

// static void XOR_Bytes(unsigned char * pData1, const unsigned char * pData2, unsigned char nCount);

// static void SubBytes(unsigned char * pState, unsigned char nCount, BOOL bInvert);

// static void ShiftRows(unsigned char *pState, BOOL bInvert);

// static unsigned char GF_Mult_By02(unsigned char num);

// static void MixColumns(unsigned char *pState, BOOL bInvert);

// static void Block_Encrypt(unsigned char *pState);

// static void Block_Decrypt(unsigned char *pState);

void print_hex(uint8_t * data, size_t length);

void test(void);





                            

#endif /* __AES_H__ */










