/*
 * --------------------------------------------------
 *
 *	Filename: console.h
 *  Description: VGA文本模式显示，头文件
 *  
 *  Vision: 1.0
 *  Created: 2018-04-10
 *  Revison: null
 #  Compiler: gcc C99标准
 *
 * --------------------------------------------------
 */

#ifndef __LIBS_CONSOLE_H__
#define __LIBS_CONSOLE_H__

#include "types.h"
#include "x86.h"

typedef
enum real_color {
	enum_rc_black = 0,
	enum_rc_blue = 1,
	enum_rc_green = 2,
	enum_rc_cyan = 3,
	enum_rc_red = 4,
	enum_rc_magenta = 5,
	enum_rc_brown = 6,
	enum_rc_light_grey = 7,
	enum_rc_dark_grey = 8,
	enum_rc_light_blue = 9,
	enum_rc_light_green = 10,
	enum_rc_light_cyan = 11,
	enum_rc_light_red = 12,
	enum_rc_light_magenta = 13,
	enum_rc_light_brown  = 14, 	// yellow
	enum_rc_white = 15
} enum_real_color_t;

// 清屏操作
void console_clear();

// 屏幕输出一个字符(带颜色)
void console_putc_color(char c, enum_real_color_t back, enum_real_color_t fore);

// 屏幕打印一个以 \0 结尾的字符串(默认黑底白字)
void console_write(char *cstr);

// 屏幕打印一个以 \0 结尾的字符串(带颜色)
void console_write_color(char *cstr, enum_real_color_t back, enum_real_color_t fore);

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, enum_real_color_t back, enum_real_color_t fore);

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, enum_real_color_t back, enum_real_color_t fore);

#endif	/* !__LIBS_CONSOLE_H__ */
