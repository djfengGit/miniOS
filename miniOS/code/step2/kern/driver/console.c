/*
 * --------------------------------------------------
 *
 *	Filename: console.c
 *  Description: VGA文本模式显示，相关函数实现
 *  
 *  Vision: 1.0
 *  Created: 2018-04-10
 *  Revison: null
 #  Compiler: gcc C99标准
 *
 * --------------------------------------------------
 	遗留问题：
 		1、考虑如何不使用全局变量，当后期退出文本显示模式时，
 		如何清除全局变量占用的内存
 		2、console_putc_color()
 		注意BackSpace(7)退格键和EDL(127)删除键
	注意：
		学习C语言和汇编时写过显示 八、十、十六进制 数字的程序
		现在竟然忘了诶！！！
		人不能忘本啊
 * --------------------------------------------------
 */

#include "console.h"

/*
 * VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，内核可以通过它来控制屏幕上字符或者图形的显示。
 * 在默认的文本模式(Text-Mode)下(25行 80列)，VGA控制器保留了一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区，
 * 若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 *
 */

// VGA 的显示缓冲的起点是 0xB8000
static uint16_t *video_memory = (uint16_t *)0xB8000;

// 屏幕"光标"的坐标
static uint8_t cursor_x = 0; /* range: 0 ~ 79 */
static uint8_t cursor_y = 0; /* range: 0 ~ 24 */

// 移动光标
static void move_cursor()
{
	// 屏幕是 80 字节宽
	uint16_t cursorLocation = cursor_y * 80 + cursor_x;
	
	// VGA 内部的寄存器多达300多个，显然无法一一映射到I/O端口的地址空间。
	// 对此 VGA 控制器的解决方案是，将一个端口作为内部寄存器的索引：0x3D4，
	// 再通过 0x3D5 端口来设置相应寄存器的值。
	// 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。

	outb(0x3D4, 14);                  	// 告诉 VGA 我们要设置光标的高字节
	outb(0x3D5, cursorLocation >> 8); 	// 发送高 8 位
	outb(0x3D4, 15);                  	// 告诉 VGA 我们要设置光标的低字节
	outb(0x3D5, cursorLocation);     	// 发送低 8 位
}

// 屏幕滚动操作
static void scroll()
{
	// attribute_byte 被构造出一个黑底白字的描述格式
	uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20
	int16_t i = 0;

	// cursor_y 到 25 的时候，就该换行了
	if (cursor_y >= 25) {
		// 将所有行的显示数据复制到上一行，第一行永远消失了...
		for (i = 0 * 80; i < 24 * 80; i++) {
		      video_memory[i] = video_memory[i+80];
		}

		// 最后的一行数据现在填充空格，不显示任何字符
		for (i = 24 * 80; i < 25 * 80; i++) {
		      video_memory[i] = blank;
		}

		// 向上移动了一行，所以 cursor_y 现在是 24
		cursor_y = 24;
	}
}

// 清屏操作
void console_clear()
{
	uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (attribute_byte << 8);
	int16_t i = 0;

	for (i = 0 * 80; i < 25 * 80; i++) {
	      video_memory[i] = blank;
	}

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

// 屏幕输出一个字符(带颜色)
void console_putc_color(char c, enum_real_color_t back, enum_real_color_t fore)
{
	uint8_t back_color = (uint8_t)back;
	uint8_t fore_color = (uint8_t)fore;

	uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
	uint16_t attribute = attribute_byte << 8;

	// 0x08 是 退格键 的 ASCII 码
	// 0x09 是 tab 键 的 ASCII 码
	if (c == 0x08 && cursor_x > 0) {
	      cursor_x--;			/* 退格键是不是考虑换行 */
	} else if (c == 0x09) {
	      cursor_x = (cursor_x+8) & ~(8-1);
	} else if (c == '\r') {
	      cursor_x = 0;
	} else if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
	} else if (c >= ' ') {
		video_memory[cursor_y*80 + cursor_x] = c | attribute;
		cursor_x++;
	}
	/*

	else if (c == 127) {
	
	}
	*/

	// 每 80 个字符一行，满80就必须换行了
	if (cursor_x >= 80) {
		cursor_x = 0;
		cursor_y ++;
	}

	// 如果需要的话滚动屏幕显示
	scroll();

	// 移动硬件的输入光标
	move_cursor();
}

// 屏幕打印一个以 \0 结尾的字符串(默认黑底白字)
void console_write(char *cstr)
{
	while (*cstr != '\0') {
	      console_putc_color(*cstr++, enum_rc_black, enum_rc_white);
	}
}

// 屏幕打印一个以 \0 结尾的字符串(带颜色)
void console_write_color(char *cstr, enum_real_color_t back, enum_real_color_t fore)
{
	while (*cstr) {
	      console_putc_color(*cstr++, back, fore);
	}
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, enum_real_color_t back, enum_real_color_t fore)
{
	int16_t tmp = 0;
	bool noZeroes = TRUE;
	int16_t i = 0;

	console_write_color("0x", back, fore);

	for (i = 28; i >= 0; i -= 4) {
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != FALSE) {
		      continue;		// 从高位向低位找到第一个不为 0 的数字，之前的0不显示
		}
		noZeroes = FALSE;	// 已找到第一个不为0的数字
		if (tmp >= 0xA) {
		      console_putc_color(tmp-0xA+'a', back, fore); // 小写形式显示
		} else {
		      console_putc_color(tmp+'0', back, fore);
		}
	}
}

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, enum_real_color_t back, enum_real_color_t fore)
{
	if (n == 0) {
		console_putc_color('0', back, fore);
		return;
	}

	uint32_t acc = n;
	char c[32] = {0};
	int8_t i = 0;
	while (acc > 0) {			// 从低位向高位找各个显示位上的数字
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = 0;	// '\0'

	char c2[32] = {0};
	c2[i--] = 0;

	int8_t j = 0;
	while (i >= 0) {		// 转换为从高位到低位显示
	      c2[i--] = c[j++];
	}

	console_write_color(c2, back, fore);
}
