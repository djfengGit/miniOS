

#include <types.h>
#include <x86.h>
#include <console.h>

int kern_init(void) __attribute__((noreturn));

int kern_init(void)
{
    /* 将 BSS 段初始化为 0 。可执行文件的 BSS 段初始化是由加载器完成的，
        kernel 的加载器是bootloader 
        此处的 extern 声明，未链接到任何头文件，链接到 link.ld
        edata end 是 link.ld 中对 BSS 段的描述 */
	extern char edata[], end[];
	memset(edata, 0, end - edata);
   
    console_clear();
    console_write_color("Hello, this is miniOS kernel!\n", enum_rc_black, enum_rc_green);

    while (1);
}

