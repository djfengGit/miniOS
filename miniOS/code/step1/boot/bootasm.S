#
#-------------------------------------------------
#
#   Filename: bootasm.s
#   Description: miniOS 小内核的 bootloader
#   
#   Vision: 1.0
#   Created: 2018-04-10
#   Revison: null
#   reference: XV6
#
#-------------------------------------------------
#


#include <asm.h>

# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.set CR0_PE_ON,         0x1                 # protected mode enable flag
.set PROT_MODE_CSEG,    0x8                 # kernel code segment selector
                                                # 指向 gdt 的第二项
.set PROT_MODE_DSEG,    0x10                # kernel data segment selector
                                                # 指向 gdt 的第三项

.section .text
.globl _start
_start:
.code16                                         # Assemble for 16-bit mode
    cli                                         # Disable interrupts -- necessary
    cld                                         # String operations increment
    # Set up the important data segment registers (DS, ES, SS).
    #  After the work of BIOS, (DS, ES, SS) are unknown for us.
    xorw %ax, %ax                               # Set segment number to zero
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    # Enable A20:
    #  For backwards compatibility with the earliest PCs, physical
    #  address line 20 is tied low, so that addresses higher than
    #  1MB wrap around to zero by default. This code undoes this.
seta20.1:
    inb $0x64, %al                              # Wait for not busy
    testb $0x2, %al
    jnz seta20.1

    movb $0xd1, %al                             # 0xd1 -> port 0x64
    outb %al, $0x64

seta20.2:
    inb $0x64, %al                              # Wait for not busy
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                             # 0xdf -> port 0x60
    outb %al, $0x60

    # Switch from real to protected mode, using a bootstrap GDT
    #  and segment translation that makes virtual addresses
    #  identical to physical addresses, so that the
    #  effective memory map does not change during the switch.
    lgdt gdt_descr
    movl %cr0, %eax
    orl $CR0_PE_ON, %eax
    movl %eax, %cr0

    # Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    ljmp $PROT_MODE_CSEG, $protcseg
    # 一个长跳转指令，使CS强制改变，已经进入32bit寻址模式

.code32
protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                   # data segment selector
    movw %ax, %ds                               # -> DS: Data Segment
    movw %ax, %es                               # -> ES: Extra Segment
    movw %ax, %fs                               # -> FS
    movw %ax, %gs                               # -> GS
    movw %ax, %ss                               # -> SS: Stack Segment

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    # 不理解可去学习X86的栈帧结构，栈从高地址向低地址生长
    movl $0x0, %ebp
    movl $_start, %esp
    # 此处入口不能为 main(),会出错。可能是使用了 sign.c 其中也有 main()  会使链接器出错
    call bootmain

    # If main returns (it shouldn't), loop.
stop:
    hlt                                         # 停机指令，什么也不做，可以降低 CPU 功耗
    jmp stop                                    #   

    # 数据段在内存中的位置就紧接在代码段之后，即全局描述符表的位置
.section .data
    # .p2align 相比于 .align 排除机器差异性，在所有机器上得到一致的效果
    # p2align[wl] abs-expr, abs-expr, abs-expr
    #  增加位置计数器(在当前的子段)使它指向规定的存储边界。第一个表达式参数(结果必须是
    #  纯粹的数字) 代表位置计数器移动后，计数器中连续为0的低序位数量。例如‘.align 3’
    #  向后移动位置指针直至8的倍数（指针的最低的3位为0）。如果地址已经是8倍数，则无需移动。
# Bootstrap GDT
.p2align 2                                          # force 4 byte alignment
gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel
#code / data segment 段基址0x0，段限长4G

gdt_descr:
    .word 0x17 #(gdt_descr-gdt-1)                                      # sizeof(gdt) - 1  3个表项，共24字节
    .long gdt                                       # address gdt
