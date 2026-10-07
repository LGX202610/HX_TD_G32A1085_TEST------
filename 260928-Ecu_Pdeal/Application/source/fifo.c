#include "fifo.h"
#include <string.h>



/**
 * @brief FIFO初始化
 * @param fifo  fifo实例指针
 * @param buf   用户提供缓冲区(静态数组/malloc内存)
 * @param elem_size 单个元素字节大小 sizeof(type)
 * @param capacity 最多存放多少个元素（注意不是字节！是元素数量）
 * @return true成功，false参数非法
 */
bool fifo_init(FIFO_t* fifo, void* buf, size_t elem_size, size_t capacity)
{
    if (fifo == NULL || buf == NULL || elem_size == 0 || capacity == 0) {
        return false;
    }

    fifo->buf        = buf;
    fifo->elem_size  = elem_size;
    fifo->capacity   = capacity;
    fifo->head       = 0U;
    fifo->tail       = 0U;
    fifo->count      = 0U;
    return true;
}


/**
 * @brief 写入1个元素到FIFO
 * @param fifo 实例
 * @param src 源数据指针，可以是任意结构体/变量地址
 * @return true写入成功；false FIFO满或者参数错误
 */
bool fifo_write(FIFO_t* fifo, const void* src)
{
    if (fifo == NULL || src == NULL || fifo_is_full(fifo)) {
        return false;
    }

    // void*不能做指针运算，强制char*按字节偏移
    char* dest_ptr = (char*)fifo->buf + fifo->head * fifo->elem_size;
    memcpy(dest_ptr, src, fifo->elem_size);

    fifo->head = (fifo->head + 1U) % fifo->capacity;
    fifo->count++;
    return true;
}


/**
 * @brief 读出1个元素，元素从队列移除
 * @param fifo 实例
 * @param dst 接收读出数据的缓冲区
 * @return true读取成功；false队空
 */
bool fifo_read(FIFO_t* fifo, void* dst)
{
    if (fifo == NULL || dst == NULL || fifo_is_empty(fifo)) {
        return false;
    }

    char* src_ptr = (char*)fifo->buf + fifo->tail * fifo->elem_size;
    memcpy(dst, src_ptr, fifo->elem_size);

    fifo->tail = (fifo->tail + 1U) % fifo->capacity;
    fifo->count--;
    return true;
}


/**
 * @brief 偷看队首元素，**不弹出**，不改变fifo状态
 * @param fifo 实例
 * @param dst 接收队首数据
 * @return true成功；false队列为空
 */
bool fifo_peek(const FIFO_t* fifo, void* dst)
{
    if (fifo == NULL || dst == NULL || fifo_is_empty(fifo)) {
        return false;
    }
    char* src_ptr = (char*)fifo->buf + fifo->tail * fifo->elem_size;
    memcpy(dst, src_ptr, fifo->elem_size);
    return true;
}


/**
 * @brief 清空FIFO，不会修改缓冲区内存，只重置索引计数器
 */
void fifo_clear(FIFO_t* fifo)
{
    if (fifo == NULL) return;
    fifo->head  = 0U;
    fifo->tail  = 0U;
    fifo->count = 0U;
}


/**
 * @brief 获取当前FIFO内有效元素个数
 */
size_t fifo_get_count(const FIFO_t* fifo)
{
    return (fifo != NULL) ? fifo->count : 0U;
}


/**
 * @brief 获取FIFO最大容纳元素数量
 */
size_t fifo_get_capacity(const FIFO_t* fifo)
{
    return (fifo != NULL) ? fifo->capacity : 0U;
}


/**
 * @brief 判断FIFO是否为空
 */
bool fifo_is_empty(const FIFO_t* fifo)
{
    return (fifo == NULL) ? true : (fifo->count == 0U);
}


/**
 * @brief 判断FIFO是否已满
 */
bool fifo_is_full(const FIFO_t* fifo)
{
    return (fifo == NULL) ? true : (fifo->count >= fifo->capacity);
}



