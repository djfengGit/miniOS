- 尝试写出一个 Makefile lind.ld 链接器脚本，失败了
- hulex 使用的 bootloader 是 grub，自己尝试写bootloader，但和C语言链接编译失败
    - 没有搞明白 ucore 的 Markfile 的原理

- 思考：
    - bootloader 不一定要是 elf 格式，内核是就可以了
    - gas汇编器如何使得到的目标文件是 elf 格式？
    - 各个 .c 文件的依赖关系是自己定义还是编译器自动寻找
    
- 计划
    - 采用 ucore 的 Makefile 实现一个 bootloader  