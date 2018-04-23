#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include <types.h>

/* 清空cpu流水线，使cpu重新从内存读取指令和数据 */
#define barrier() __asm__ __volatile__ ("" ::: "memory")

/* 读取和写入I/O端口的C语言内联汇编实现 */
static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline uint16_t inw(uint16_t port) __attribute__((always_inline));
static inline void insl(uint32_t port, void *addr, int cnt) __attribute__((always_inline));
static inline void outb(uint16_t port, uint8_t data) __attribute__((always_inline));
static inline void outw(uint16_t port, uint16_t data) __attribute__((always_inline));

/* 从指定端口读取一个字节长的数据 */
static inline uint8_t
inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "d" (port) : "memory");
    return data;
}

/* 从指定端口读取一个字长（两个字节长）的数据 */
static inline uint16_t
inw(uint16_t port) {
	uint16_t data = 0;
	asm volatile ("inw %1, %0" : "=a" (data) : "d" (port) : "memory");
	return data;
}

static inline void
insl(uint32_t port, void *addr, int cnt) {
    asm volatile (
        "cld;"
        "repne; insl;"
        : "=D" (addr), "=c" (cnt)
        : "d" (port), "0" (addr), "1" (cnt)
        : "memory", "cc");
}

/* 向指定端口写入一个字节长的数据 */
static inline void
outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" :: "a" (data), "d" (port) : "memory");
}

/* 向指定端口写入一个字长（两个字节长）的数据 */
static inline void
outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %0, %1" :: "a" (data), "d" (port) : "memory");
}

#endif /* !__LIBS_X86_H__ */

