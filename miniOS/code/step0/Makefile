#
#-------------------------------------------------
#
#	Filename: Makefile
#	Description: miniOS 小内核的 Makefile
#	
#	Vision: 1.0
#	Created: 2018-04-10
#	Revison: null
#	
#	Author: yhfeng@hust.edu.cn
#
#-------------------------------------------------
#

# patsubst 处理所有在 C_SOURCES 字列中的字（一列文件名），如果它的结尾是 '.c'，就用 '.o' 把 '.c' 取代
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = as

#
#	-m32 ：生成32位的代码
#	-ggdb 和 -gstabs+ ：添加相关的调试信息
#	-nostdinc ：不包含C语言标准库里的头文件，因为那库函数都是操作系统定义的
#	-fno-builtin ：要求gcc不主动使用自己的内建函数，除非显式声明。gcc有很多内建函数来替一些C语言的库函数以提高效率，
#				   比如用puts函数替换只有一个字符串参数的printf函数。
#	-fno-stack-protector : 无栈溢出保护
#	-I include ：指定头文件目录为 include 文件夹
#
#	-T scripts/kernel.ld : 使用自定义的链接器脚本
#	-m elf_i386 ：生成i386平台下ELF格式的可执行文件
#	-nostdlib ：不链接C语言的标准库
#
C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I libs
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
#ASM_FLAGS = -f elf -g -F stabs  #nasm 参数
ASM_FLAGS = -g --gstabs+
	# 使用 gas 不能加入 -f 参数，会莫名其妙报错，可能是汇编器无法正确跳过注释和空行

#------------------------模块分割线------------------------------

all: $(S_OBJECTS) $(C_OBJECTS) link 
#update_image

#
#	.c.o : 表示所有的 .o 文件都是依赖于相应的 .c 文件的
#	$@ ：代表目标文件 target
#	$^ ：代表所有的依赖文件(components)
#	$< ：代表第一个依赖文件(components中最左边的那个)
#	@ ：用在“规则”行中，表示不显示命令本身，而只显示它的结果。规则行以TAB键开头，紧跟 @ 符号
#
.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) -o $*.o $<
#	$(ASM) $(ASM_FLAGS) $<

.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o miniOS_kernel

#------------------------模块分割线------------------------------

#	create bootblock
#bootfiles = $(call listf_cc,boot)
#$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

#bootblock = $(call totarget,bootblock)

#$(bootblock): $(call toobj,boot/bootasm.S) $(call toobj,$(bootfiles)) | $(call totarget,sign)
#	@echo + ld $@
#	$(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
#	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
#	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
#	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

#$(call create_target,bootblock)

#------------------------模块分割线------------------------------


.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) 
	#	miniOS_kernel

.PHONY:qemu
qemu:
	qemu -hda floppy.img -boot a
	