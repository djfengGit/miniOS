

#include <types.h>
#include <x86.h>
#include <console.h>

int kern_init(void) __attribute__((noreturn));

int kern_init(void)
{
 /*   extern char edata[], end[];
    memset(edata, 0, end - edata);

    cons_init();                // init the console

    const char *message = "(THU.CST) os is loading ...";
    cprintf("[%d]%s\n", strlen(message), message);
*/

	/*extern char edata[], end[];
	memset(edata, 0, end - edata);
   */
    console_clear();
    console_write_color("Hello, this is miniOS kernel!\n", enum_rc_black, enum_rc_green);


    while (1);
}

