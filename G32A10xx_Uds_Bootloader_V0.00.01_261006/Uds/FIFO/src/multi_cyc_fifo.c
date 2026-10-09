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
#include "multi_cyc_fifo.h"
/**************************************************************************
                    MACRO DEFINITION
                    内部私有宏定义
**************************************************************************/
// 全局缓冲区总字节 = FIFO管理头区80字节 + 上层配置的业务数据缓存区
#define TOTAL_SIZE_IN_BYTES (80u + ALL_FIFO_SPACE)
/**************************************************************************
                    ENUMERATION DEFINITION
                    FIFO自身运行状态私有枚举
**************************************************************************/
/**
* @brief 单个FIFO实例运行状态
*/
typedef enum
{
    FIFO_STATUS_IS_EMPTY = 0, // FIFO为空，无有效数据
    FIFO_STATUS_IS_BUSY  = 1, // FIFO存有部分数据，可读可写
    FIFO_STATUS_IS_FULL  = 2  // FIFO已满，无法写入新数据
} FifoStatusType;
/**************************************************************************
                    STRUCT DEFINITION
                    FIFO链表节点信息结构体
**************************************************************************/
/**
* @brief 单个循环FIFO节点管理信息结构体，多FIFO以单向链表串联
*/
typedef struct
{
    fifoIdType masterName;          // 当前FIFO唯一ID编号
    fifoSizeType FifoSize;         // 当前FIFO业务数据缓存总容量
    fifoSizeType positionToRead;    // 读指针偏移位置
    fifoSizeType positionToWrite;  // 写指针偏移位置
    FifoStatusType currentFifo;     // 当前FIFO满/空/忙状态
    unsigned char *FifoBeginPosition; // 当前FIFO数据存储区起始地址
    void *nextFifoPosition;         // 链表下一个FIFO节点指针
} FifoInfoType;
/**************************************************************************
                    GLOBAL VARIABLE
                    全局静态变量定义
**************************************************************************/
static fifoSizeType g_eraseFifoSize = TOTAL_SIZE_IN_BYTES; // 全局缓存剩余可用总字节数
static FifoInfoType *listHeaderPointer = (FifoInfoType *)0u; // FIFO单向链表头节点指针
static unsigned char g_FifoArray[TOTAL_SIZE_IN_BYTES] = {0}; // FIFO全局统一大缓冲区
/**************************************************************************
                    FUNCTION DECLARATION
                    内部静态函数前置声明
**************************************************************************/
/**
 * @brief 将新建FIFO节点挂载至单向链表尾部
 * @param nodeBlock 待挂载的FIFO节点指针
 * @param front     链表头指针二级指针
 * @param flag      输出操作错误码
 */
static void PlugNoteToList(FifoInfoType *nodeBlock, FifoInfoType **front, errorStateType *flag);

/**
 * @brief 根据FIFO ID遍历链表，查找对应FIFO节点
 * @param id        待查找FIFO编号
 * @param tempNode 输出查找到的节点指针
 * @param flag      输出操作错误码
 */
static void SearchFifo(fifoIdType id, FifoInfoType **tempNode, errorStateType *flag);
/**************************************************************************
                    LOCAL FUNCTION
                    内部静态函数实现
**************************************************************************/
static void PlugNoteToList(FifoInfoType *nodeBlock, FifoInfoType **front, errorStateType *flag)
{
    FifoInfoType *nextNode = (FifoInfoType *)0u;
#ifdef SAFE_REQUIRE_IS_O_3
    // 安全校验：错误码指针、链表头指针、节点指针不能为空
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((FifoInfoType **)0u == front || (FifoInfoType *)0u == nodeBlock)
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    // 链表为空，当前节点直接作为链表头
    if ((FifoInfoType *)0u == *front)
    {
        *front = nodeBlock;
        *flag = STATE_NO_ERROR;
        return;
    }
    nextNode = *front;
    // 遍历到链表末尾
    while ((void *)0u != nextNode->nextFifoPosition)
    {
        // 检测重复节点，禁止重复挂载
        if (nodeBlock == nextNode)
        {
            *flag = STATE_REG_SEC;
            return;
        }
        nextNode = (FifoInfoType *)(nextNode->nextFifoPosition);
    }
    // 将新节点挂在链表尾部
    nextNode->nextFifoPosition = (void *)nodeBlock;
    nodeBlock->nextFifoPosition = (errorStateType *)0u;
    *flag = STATE_NO_ERROR;
}

static void SearchFifo(fifoIdType id, FifoInfoType **tempNode, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
#ifdef SAFE_REQUIRE_IS_O_3
    // 安全空指针防护
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((FifoInfoType **)0u == tempNode)
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    block = listHeaderPointer;
    // 循环遍历链表匹配FIFO ID
    while ((FifoInfoType *)0u != block)
    {
        if (id == block->masterName)
        {
            *flag = STATE_NO_ERROR;
            *tempNode = block;
            return;
        }
        block = (FifoInfoType *)block->nextFifoPosition;
    }
    // 遍历完毕无匹配ID，返回未找到节点错误
    *flag = STATE_NO_NODE;
}
/**************************************************************************
                    GLOBAL FUNCTION
                    对外全局API函数实现
**************************************************************************/
void GainAccessReadSize(fifoIdType id, fifoSizeType *space, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
#ifdef SAFE_REQUIRE_IS_O_3
    // 入参空指针安全校验
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((fifoSizeType *)0u == space)
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    // 查找目标FIFO节点
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 根据FIFO状态计算可读数据长度
    if (FIFO_STATUS_IS_BUSY == block->currentFifo)
    {
        // 读指针大于写指针：数据分段存储，分两段计算
        *space = (block->positionToRead > block->positionToWrite) ?
                 (block->FifoSize - block->positionToRead + block->positionToWrite) :
                 (block->positionToWrite - block->positionToRead);
    }
    else if (FIFO_STATUS_IS_FULL == block->currentFifo)
    {
        // FIFO已满，全部容量均可读取
        *space = block->FifoSize;
    }
    else
    {
        // FIFO为空，可读长度0
        *space = (fifoSizeType)0u;
    }
    *flag = STATE_NO_ERROR;
}

void OperateFifoLogic(fifoSizeType operateFifoSize, fifoSizeType id, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
    fifoSizeType shouldAccSpace = 0u;
    uint32 eraseFIFO = 0u;
#ifdef SAFE_REQUIRE_IS_O_3
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    // 内存地址4字节对齐校验
    eraseFIFO = (uint32)(&g_FifoArray[TOTAL_SIZE_IN_BYTES - g_eraseFifoSize]) & 0x03u;
    // 校验剩余缓存是否足够分配当前FIFO
    if ((operateFifoSize + 20u + eraseFIFO) > g_eraseFifoSize)
    {
        *flag = STATE_OVERFLOW;
        return;
    }
#endif
    // 查询该ID是否已存在FIFO，存在则返回重复注册错误
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR == *flag)
    {
        *flag = STATE_REG_SEC;
        return;
    }
    if (eraseFIFO)
    {
        g_eraseFifoSize -= eraseFIFO;
    }
    // 从全局缓冲区尾部分配一块内存作为新FIFO节点
    block = (FifoInfoType *)(&g_FifoArray[TOTAL_SIZE_IN_BYTES - g_eraseFifoSize]);
    block->masterName = id;
    block->nextFifoPosition = (void *)0u;
    block->FifoSize = operateFifoSize;
    block->positionToRead = 0u;
    block->positionToWrite = 0u;
    // FIFO数据存储区紧跟结构体头部后面
    block->FifoBeginPosition = (unsigned char *)((FifoInfoType *)(&g_FifoArray[TOTAL_SIZE_IN_BYTES - g_eraseFifoSize]) + 1u);
    block->currentFifo = FIFO_STATUS_IS_EMPTY;
    // 计算当前FIFO结构体+业务数据总占用字节
    shouldAccSpace = (fifoSizeType)((unsigned char *)((FifoInfoType *)(&g_FifoArray[TOTAL_SIZE_IN_BYTES - g_eraseFifoSize]) + 1u) -
                            (unsigned char *)(&g_FifoArray[TOTAL_SIZE_IN_BYTES - g_eraseFifoSize]));
    shouldAccSpace += operateFifoSize;
    // 全局剩余缓存减去本次分配大小
    g_eraseFifoSize -= shouldAccSpace;
    // 将新节点挂载到FIFO链表
    PlugNoteToList(block, &listHeaderPointer, flag);
}

void eraseFifoContent(fifoIdType id, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
#ifdef SAFE_REQUIRE_IS_O_3
    if ((errorStateType *)0u == flag)
    {
        return;
    }
#endif
    // 查找目标FIFO
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 关闭全局中断，防止读写指针被中断篡改
    DisableAllInterrupts();
    block->currentFifo = FIFO_STATUS_IS_EMPTY;
    block->positionToRead = block->positionToWrite;
    EnableAllInterrupts();
    *flag = STATE_NO_ERROR;
}

void GainInfoDataInFifo(fifoIdType id, fifoSizeType shouldReadSize, unsigned char *buf, fifoSizeType *space, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
    fifoSizeType countCycle = 0u;
    fifoSizeType accReadSize = 0u;
#ifdef SAFE_REQUIRE_IS_O_3
    // 读取缓存、长度指针、读取长度参数合法性校验
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((unsigned char *)0u == buf ||
            (fifoSizeType *)0u == space ||
            (fifoSizeType)0u == shouldReadSize )
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    // 获取当前FIFO可读数据长度
    GainAccessReadSize(id, &accReadSize, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 实际读取长度取期望长度和可读长度的较小值
    accReadSize = accReadSize > shouldReadSize ? shouldReadSize : accReadSize;
    *space = accReadSize;
    // 循环读取指定字节数据到外部缓存
    for (countCycle = 0u; countCycle < accReadSize; countCycle++)
    {
        buf[countCycle] = (block->FifoBeginPosition)[block->positionToRead] ;
        do{
            // 读指针自增，超出容量则回绕至0
            (*(&(block->positionToRead)))++;
            if(*(&(block->positionToRead)) >= (block->FifoSize))
            {
                *(&(block->positionToRead)) -= (block->FifoSize);
            }
        }while(0);
    }
    
    do{
        DisableAllInterrupts();
        // 读写指针重合则FIFO置空，否则保持忙状态
        if((block)->positionToWrite == (block)->positionToRead)
        {
            (block)->currentFifo = FIFO_STATUS_IS_EMPTY;
        }
        else
        {
            (block)->currentFifo = FIFO_STATUS_IS_BUSY;
        }
        EnableAllInterrupts();
    }while(0u);
    *flag = STATE_NO_ERROR;
}

void ProgramToFifo(fifoIdType id, unsigned char *bufToWrite, fifoSizeType space, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
    fifoSizeType countCycle = 0u;
    fifoSizeType accWriteSpace = 0u;
#ifdef SAFE_REQUIRE_IS_O_3
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((unsigned char *)0u == bufToWrite)
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    // 查询剩余可写空间
    GainAccessProgramSize(id, &accWriteSpace, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 待写入长度大于剩余空间，返回溢出错误
    if (space > accWriteSpace)
    {
        *flag = STATE_OVERFLOW;
        return;
    }
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 循环写入数据至FIFO缓存
    for (countCycle = 0u; countCycle < space; countCycle++)
    {
        (block->FifoBeginPosition)[block->positionToWrite] = bufToWrite[countCycle];
        do{
            // 写指针自增，超出容量回绕至0
            (*(&(block->positionToWrite)))++;
            if(*(&(block->positionToWrite)) >= (block->FifoSize))
            {
                *(&(block->positionToWrite)) -= (block->FifoSize);
            }
        }while(0);
    }
    do{
        DisableAllInterrupts();
        // 读写指针重合代表FIFO写满
        if((block)->positionToWrite == (block)->positionToRead)
        {
            (block)->currentFifo = FIFO_STATUS_IS_FULL;
        }
        else
        {
            (block)->currentFifo = FIFO_STATUS_IS_BUSY;
        }
        EnableAllInterrupts();
    }while(0u);
    *flag = STATE_NO_ERROR;
}

void GainAccessProgramSize(fifoIdType id, fifoSizeType *space, errorStateType *flag)
{
    FifoInfoType *block = (FifoInfoType *)0u;
#ifdef SAFE_REQUIRE_IS_O_3
    if ((errorStateType *)0u == flag)
    {
        return;
    }
    if ((fifoSizeType *)0u == space)
    {
        *flag = STATE_NULL_POINT;
        return;
    }
#endif
    SearchFifo(id, &block, flag);
    if (STATE_NO_ERROR != *flag)
    {
        return;
    }
    // 根据FIFO状态计算剩余可写入空闲字节
    if (FIFO_STATUS_IS_BUSY == block->currentFifo)
    {
        *space = (block->positionToRead > block->positionToWrite) ?
                 (block->positionToRead - block->positionToWrite) :
                 (block->FifoSize + block->positionToRead - block->positionToWrite);
    }
    else if (FIFO_STATUS_IS_EMPTY == block->currentFifo)
    {
        // FIFO为空，全部容量均可写入
        *space = block->FifoSize;
    }
    else
    {
        // FIFO已满，无空闲空间
        *space = (fifoSizeType)0u;
    }
    *flag = STATE_NO_ERROR;
}
