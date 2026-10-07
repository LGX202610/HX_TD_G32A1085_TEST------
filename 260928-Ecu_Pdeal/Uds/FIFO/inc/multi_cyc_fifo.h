/*******************************************************************************
* Project Name      : CAN/LIN Protocol Stack
* Platform          : Arm
* Revision Number   : V1.0
* Compiled Version  : G32A1xxx_01-June-25
*
* Copyright (C) 2025 Geehy Semiconductor
*
* You may not use this file except in compliance with the GEEHY COPYRIGHT NOTICE
* (GEEHY SOFTWARE PACKAGE LICENSE).
*
* The program is only for reference, which is distributed in the hope that it
* will be useful and instructional for customers to develop their software.
* Unless required by applicable law or agreed to in writing, the program is
* distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY
* KIND, either express or implied. See the GEEHY SOFTWARE PACKAGE LICENSE for
* the governing permissions and limitations under the License.
*
*******************************************************************************/
#ifndef MULTI_CYC_FIFO_H_
#define MULTI_CYC_FIFO_H_
#include "includes.h"
/**************************************************************************
                    MACRO DEFINITION
                    宏定义区域
**************************************************************************/
// 根据通信协议配置全局FIFO总缓存字节长度
#ifdef ALLOW_LIN_TP
    #define ALL_FIFO_SPACE (450u)
#elif defined ALLOW_CAN_TP
    #define ALL_FIFO_SPACE (400u)
#else
    #define ALL_FIFO_SPACE (100u)
#endif

#ifndef TRUE
    #define TRUE (1u)  // 布尔真值定义
#endif
#ifndef FALSE
    #define FALSE (!TRUE) // 布尔假值定义
#endif

#define SAFE_REQUIRE_IS_O_3 // 安全校验宏，开启函数入参空指针防护逻辑
/**************************************************************************
                    ENUMERATION DEFINITION
                    错误状态枚举定义
**************************************************************************/
/**
* @brief  FIFO操作全流程错误码枚举
*/
typedef enum
{
    STATE_NO_ERROR          = 0x00u, // 无错误，操作正常完成
    STATE_LEES_MIN          = 0x01u, // 可用空间小于最小操作长度
    STATE_NO_NODE           = 0x02u, // 未查询到对应ID的FIFO节点
    STATE_OVERFLOW          = 0x03u, // FIFO缓冲区溢出，写入数据超限
    STATE_NULL_POINT        = 0x04u, // 传入空指针参数
    STATE_REG_SEC           = 0x05u, // 重复注册相同FIFO节点
    STATE_TIME_TYPE_ERROR   = 0x06u, // 定时器类型参数错误
    STATE_TIME_BUSY         = 0x07u, // 定时器资源被占用繁忙
    STATE_TIMEOUT           = 0x08u, // 操作等待超时
    STATE_WRITE_ERROR       = 0x09u, // FIFO写入数据失败
    STATE_READ_ERROR        = 0x0Au  // FIFO读取数据失败
} errorStateType;
/**************************************************************************
                    OTHER TYPE DEFINITION
                    自定义基础数据类型别名
**************************************************************************/
typedef unsigned short fifoIdType;    // FIFO唯一编号ID类型
typedef unsigned short fifoSizeType;  // FIFO长度、偏移量尺寸类型
/**************************************************************************
                    FUNCTION DECLARATION
                    对外全局API函数声明
**************************************************************************/
/**
 * @brief  获取指定FIFO当前可读有效数据字节长度
 * @param id        目标FIFO编号ID
 * @param space     输出参数：可读数据长度
 * @param flag      输出参数：操作错误状态码
 */
void GainAccessReadSize(fifoIdType id, fifoSizeType *space, errorStateType *flag);

/**
 * @brief  创建并注册一个指定大小的FIFO节点
 * @param operateFifoSize 新建FIFO单缓存容量大小
 * @param id               待创建FIFO唯一编号ID
 * @param flag             输出参数：操作错误状态码
 */
void OperateFifoLogic(fifoSizeType operateFifoSize, fifoSizeType id, errorStateType *flag);

/**
 * @brief  清空指定FIFO内全部数据，复位读写指针至空状态
 * @param id    目标FIFO编号ID
 * @param flag  输出参数：操作错误状态码
 */
void eraseFifoContent(fifoIdType id, errorStateType *flag);

/**
 * @brief  从FIFO读取指定长度数据到外部缓存
 * @param id            目标FIFO编号ID
 * @param shouldReadSize 期望读取字节数
 * @param buf           外部接收数据缓存指针
 * @param space         输出参数：实际成功读取字节数
 * @param flag          输出参数：操作错误状态码
 */
void GainInfoDataInFifo(fifoIdType id, fifoSizeType shouldReadSize, unsigned char *buf, fifoSizeType *space, errorStateType *flag);

/**
 * @brief  将外部缓存数据写入指定FIFO
 * @param id        目标FIFO编号ID
 * @param bufToWrite待写入数据缓存指针
 * @param space     待写入数据字节长度
 * @param flag      输出参数：操作错误状态码
 */
void ProgramToFifo(fifoIdType id, unsigned char *bufToWrite, fifoSizeType space, errorStateType *flag);

/**
 * @brief  获取指定FIFO当前剩余可写入空闲字节长度
 * @param id    目标FIFO编号ID
 * @param space 输出参数：剩余可写空闲长度
 * @param flag  输出参数：操作错误状态码
 */
void GainAccessProgramSize(fifoIdType id, fifoSizeType *space, errorStateType *flag);
#endif
