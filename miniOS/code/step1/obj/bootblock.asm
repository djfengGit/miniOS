
obj/bootblock.o：     文件格式 elf32-i386


Disassembly of section .text:

00007c00 <_start>:

.section .text
.globl _start
_start:
.code16                                         # Assemble for 16-bit mode
    cli                                         # Disable interrupts -- necessary
    7c00:	fa                   	cli    
    cld                                         # String operations increment
    7c01:	fc                   	cld    
    # Set up the important data segment registers (DS, ES, SS).
    #  After the work of BIOS, (DS, ES, SS) are unknown for us.
    xorw %ax, %ax                               # Set segment number to zero
    7c02:	31 c0                	xor    %eax,%eax
    movw %ax, %ds
    7c04:	8e d8                	mov    %eax,%ds
    movw %ax, %es
    7c06:	8e c0                	mov    %eax,%es
    movw %ax, %ss
    7c08:	8e d0                	mov    %eax,%ss

00007c0a <seta20.1>:
    # Enable A20:
    #  For backwards compatibility with the earliest PCs, physical
    #  address line 20 is tied low, so that addresses higher than
    #  1MB wrap around to zero by default. This code undoes this.
seta20.1:
    inb $0x64, %al                              # Wait for not busy
    7c0a:	e4 64                	in     $0x64,%al
    testb $0x2, %al
    7c0c:	a8 02                	test   $0x2,%al
    jnz seta20.1
    7c0e:	75 fa                	jne    7c0a <seta20.1>

    movb $0xd1, %al                             # 0xd1 -> port 0x64
    7c10:	b0 d1                	mov    $0xd1,%al
    outb %al, $0x64
    7c12:	e6 64                	out    %al,$0x64

00007c14 <seta20.2>:

seta20.2:
    inb $0x64, %al                              # Wait for not busy
    7c14:	e4 64                	in     $0x64,%al
    testb $0x2, %al
    7c16:	a8 02                	test   $0x2,%al
    jnz seta20.2
    7c18:	75 fa                	jne    7c14 <seta20.2>

    movb $0xdf, %al                             # 0xdf -> port 0x60
    7c1a:	b0 df                	mov    $0xdf,%al
    outb %al, $0x60
    7c1c:	e6 60                	out    %al,$0x60

    # Switch from real to protected mode, using a bootstrap GDT
    #  and segment translation that makes virtual addresses
    #  identical to physical addresses, so that the
    #  effective memory map does not change during the switch.
    lgdt gdt_descr
    7c1e:	0f 01 16             	lgdtl  (%esi)
    7c21:	94                   	xchg   %eax,%esp
    7c22:	7d 0f                	jge    7c33 <protcseg+0x1>
    movl %cr0, %eax
    7c24:	20 c0                	and    %al,%al
    orl $CR0_PE_ON, %eax
    7c26:	66 83 c8 01          	or     $0x1,%ax
    movl %eax, %cr0
    7c2a:	0f 22 c0             	mov    %eax,%cr0

    # Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    ljmp $PROT_MODE_CSEG, $protcseg
    7c2d:	ea                   	.byte 0xea
    7c2e:	32 7c 08 00          	xor    0x0(%eax,%ecx,1),%bh

00007c32 <protcseg>:
    # 一个长跳转指令，使CS强制改变，已经进入32bit寻址模式

.code32
protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                   # data segment selector
    7c32:	66 b8 10 00          	mov    $0x10,%ax
    movw %ax, %ds                               # -> DS: Data Segment
    7c36:	8e d8                	mov    %eax,%ds
    movw %ax, %es                               # -> ES: Extra Segment
    7c38:	8e c0                	mov    %eax,%es
    movw %ax, %fs                               # -> FS
    7c3a:	8e e0                	mov    %eax,%fs
    movw %ax, %gs                               # -> GS
    7c3c:	8e e8                	mov    %eax,%gs
    movw %ax, %ss                               # -> SS: Stack Segment
    7c3e:	8e d0                	mov    %eax,%ss

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    # 不理解可去学习X86的栈帧结构，栈从高地址向低地址生长
    movl $0x0, %ebp
    7c40:	bd 00 00 00 00       	mov    $0x0,%ebp
    movl $_start, %esp
    7c45:	bc 00 7c 00 00       	mov    $0x7c00,%esp
    # 此处入口不能为 main(),会出错。可能是使用了 sign.c 其中也有 main()  会使链接器出错
    call bootmain
    7c4a:	e8 03 00 00 00       	call   7c52 <bootmain>

00007c4f <stop>:

    # If main returns (it shouldn't), loop.
stop:
    hlt                                         # 停机指令，什么也不做，可以降低 CPU 功耗
    7c4f:	f4                   	hlt    
    jmp stop                                    #   
    7c50:	eb fd                	jmp    7c4f <stop>

00007c52 <bootmain>:
#include <types.h>
#include <x86.h>

/* bootmain - the entry of bootloader */
void bootmain(void)
{
    7c52:	55                   	push   %ebp
    uint8_t *input = (uint8_t *)0xB8000;
    uint8_t color = (0 << 4) | (15 & 0x0F);

    *input++ = 'H'; *input++ = color;
    7c53:	c6 05 00 80 0b 00 48 	movb   $0x48,0xb8000
    7c5a:	c6 05 01 80 0b 00 0f 	movb   $0xf,0xb8001
    *input++ = 'e'; *input++ = color;
    7c61:	c6 05 02 80 0b 00 65 	movb   $0x65,0xb8002
    7c68:	c6 05 03 80 0b 00 0f 	movb   $0xf,0xb8003
    *input++ = 'l'; *input++ = color;
    7c6f:	c6 05 04 80 0b 00 6c 	movb   $0x6c,0xb8004
#include <types.h>
#include <x86.h>

/* bootmain - the entry of bootloader */
void bootmain(void)
{
    7c76:	89 e5                	mov    %esp,%ebp
    uint8_t *input = (uint8_t *)0xB8000;
    uint8_t color = (0 << 4) | (15 & 0x0F);

    *input++ = 'H'; *input++ = color;
    *input++ = 'e'; *input++ = color;
    *input++ = 'l'; *input++ = color;
    7c78:	c6 05 05 80 0b 00 0f 	movb   $0xf,0xb8005
    *input++ = 'l'; *input++ = color;
    7c7f:	c6 05 06 80 0b 00 6c 	movb   $0x6c,0xb8006
    7c86:	c6 05 07 80 0b 00 0f 	movb   $0xf,0xb8007
    *input++ = 'o'; *input++ = color;
    7c8d:	c6 05 08 80 0b 00 6f 	movb   $0x6f,0xb8008
    7c94:	c6 05 09 80 0b 00 0f 	movb   $0xf,0xb8009
    *input++ = ','; *input++ = color;
    7c9b:	c6 05 0a 80 0b 00 2c 	movb   $0x2c,0xb800a
    7ca2:	c6 05 0b 80 0b 00 0f 	movb   $0xf,0xb800b
    *input++ = ' '; *input++ = color;
    7ca9:	c6 05 0c 80 0b 00 20 	movb   $0x20,0xb800c
    7cb0:	c6 05 0d 80 0b 00 0f 	movb   $0xf,0xb800d
    *input++ = 'O'; *input++ = color;
    7cb7:	c6 05 0e 80 0b 00 4f 	movb   $0x4f,0xb800e
    7cbe:	c6 05 0f 80 0b 00 0f 	movb   $0xf,0xb800f
    *input++ = 'S'; *input++ = color;
    7cc5:	c6 05 10 80 0b 00 53 	movb   $0x53,0xb8010
    7ccc:	c6 05 11 80 0b 00 0f 	movb   $0xf,0xb8011
    *input++ = ' '; *input++ = color;
    7cd3:	c6 05 12 80 0b 00 20 	movb   $0x20,0xb8012
    7cda:	c6 05 13 80 0b 00 0f 	movb   $0xf,0xb8013
    *input++ = 'K'; *input++ = color;
    7ce1:	c6 05 14 80 0b 00 4b 	movb   $0x4b,0xb8014
    7ce8:	c6 05 15 80 0b 00 0f 	movb   $0xf,0xb8015
    *input++ = 'e'; *input++ = color;
    7cef:	c6 05 16 80 0b 00 65 	movb   $0x65,0xb8016
    7cf6:	c6 05 17 80 0b 00 0f 	movb   $0xf,0xb8017
    *input++ = 'r'; *input++ = color;
    7cfd:	c6 05 18 80 0b 00 72 	movb   $0x72,0xb8018
    7d04:	c6 05 19 80 0b 00 0f 	movb   $0xf,0xb8019
    *input++ = 'n'; *input++ = color;
    7d0b:	c6 05 1a 80 0b 00 6e 	movb   $0x6e,0xb801a
    7d12:	c6 05 1b 80 0b 00 0f 	movb   $0xf,0xb801b
    *input++ = 'e'; *input++ = color;
    7d19:	c6 05 1c 80 0b 00 65 	movb   $0x65,0xb801c
    7d20:	c6 05 1d 80 0b 00 0f 	movb   $0xf,0xb801d
    *input++ = 'l'; *input++ = color;
    7d27:	c6 05 1e 80 0b 00 6c 	movb   $0x6c,0xb801e
    7d2e:	c6 05 1f 80 0b 00 0f 	movb   $0xf,0xb801f
    *input++ = '!'; *input++ = color;
    7d35:	c6 05 20 80 0b 00 21 	movb   $0x21,0xb8020
    7d3c:	c6 05 21 80 0b 00 0f 	movb   $0xf,0xb8021
    7d43:	eb fe                	jmp    7d43 <bootmain+0xf1>
