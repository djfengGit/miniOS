/*
 * --------------------------------------------------
 *
 *  Filename: 
 *  Description: 
 *  
 *  Vision: 1.0
 *  Created: 2018-04-10
 *  Revison: null
 #  Compiler: gcc C99标准
 *
 * --------------------------------------------------
 */

#include <types.h>
#include <x86.h>
#include <elf.h>

/* *********************************************************************
 * This a dirt simple boot loader, whose sole job is to boot
 * an ELF kernel image from the first IDE hard disk.
 *
 * DISK LAYOUT
 *  * This program(bootasm.S and bootmain.c) is the bootloader.
 *    It should be stored in the first sector of the disk.
 *
 *  * The 2nd sector onward holds the kernel image.
 *
 *  * The kernel image must be in ELF format.
 *
 * BOOT UP STEPS
 *  * when the CPU boots it loads the BIOS into memory and executes it
 *
 *  * the BIOS intializes devices, sets of the interrupt routines, and
 *    reads the first sector of the boot device(e.g., hard-drive)
 *    into memory and jumps to it.
 *
 *  * Assuming this boot loader is stored in the first sector of the
 *    hard-drive, this code takes over...
 *
 *  * control starts in bootasm.S -- which sets up protected mode,
 *    and a stack so C code then run, then calls bootmain()
 *
 *  * bootmain() in this file takes over, reads in the kernel and jumps to it.
 * */

#define SECTSIZE        512
#define ELFHDR          ((struct elfhdr *)0x10000)      // scratch space
/*
    0x10000 是 kernel 前 8 个扇区的临时加载地址 0x10000 - 0x10fff
    其实可以不加载8个扇区，只需要把elf头部和program header加载进来即可。
    但两者长度不固定
    也可以不加载在0x10000，加载完 kernel 后，这部分内存内的程序就无用了，可被覆盖。
*/


/* waitdisk - wait for disk ready */
static void waitdisk(void)
{
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void readsect(void *dst, uint32_t secno)
{
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void readseg(uintptr_t va, uint32_t count, uint32_t offset)
{
    uintptr_t end_va = va + count;

    // 实现方法很重要！找到偏移地址所在扇区的首地址，将其加载到内存中。
    // 这样可保证将 elf 文件 offset 处的数据准确地加载到虚拟内存地址 va 中。
    // round down to sector boundary
    va -= offset % SECTSIZE;    

    // translate from bytes to sectors; kernel starts at sector 1
    uint32_t secno = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for (; va < end_va; va += SECTSIZE, secno ++) {
        readsect((void *)va, secno);
    }
}  

/* bootmain - the entry of bootloader */
void bootmain(void)
{
    // read the 1st page off disk 页式内存管理，一页是 4KB，4KB = 512B * 8
    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph = NULL, *eph = NULL;

    // load each program segment (ignores ph flags)
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph ++) {
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }
    /* 注：ph 的值不是每次自增 1 ，而是 sizeof(struct proghdr) */

    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();

    /* 向端口 0x8a00（8470-8476）输出几个字。在真实硬件中，并没有设备连接到该端口，
       所以这段代码相当于什么也没有做.如果引导加载器是在 PC 模拟器上运行，
       那么端口 0x8a00 则会连接到模拟器并把控制权交还给模拟器本身。

       注意：一个真正的引导器应该尝试输出一些调试信息。
        e.g: kernel load error...
    */
bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);
}

