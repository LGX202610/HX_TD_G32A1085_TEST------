#ifndef __FIFO_H
#define __FIFO_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 通用环形FIFO结构体，支持任意数据类型
 */
typedef struct {
    void*       buf;        // 用户提供缓冲区起始地址
    size_t      elem_size;  // 单个元素占用字节数 sizeof(你的类型)
    size_t      capacity;   // FIFO最大可存放元素个数
	
    size_t      head;       // 写索引
    size_t      tail;       // 读索引
    size_t      count;      // 当前有效元素计数
} FIFO_t;






bool fifo_init(FIFO_t* fifo, void* buf, size_t elem_size, size_t capacity);

bool fifo_write(FIFO_t* fifo, const void* src);

bool fifo_read(FIFO_t* fifo, void* dst);

bool fifo_peek(const FIFO_t* fifo, void* dst);

void fifo_clear(FIFO_t* fifo);

size_t fifo_get_count(const FIFO_t* fifo);

size_t fifo_get_capacity(const FIFO_t* fifo);

bool fifo_is_empty(const FIFO_t* fifo);

bool fifo_is_full(const FIFO_t* fifo);




#ifdef __cplusplus
}
#endif

#endif