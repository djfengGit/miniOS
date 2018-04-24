
- 进度：
    + 实现了 elf 文件格式解析，bootloader 可加载内核，正确显示字符串
    + 内核入口函数为 kern_init()

- 更改：
    + 1、bootmain() 函数，加入了解析 elf 文件格式的代码，保证整个 bootblock 大小小于 512 字节
    + 2、Makefile 作了较大改动，可用 diff 命令对比 step2 与 step1 的 Makefile，可理解其中的区别
    + 3、新建 kern 文件夹，其内是主要的内核模块

- 思考：
    + 内核加载到 1M 内存以后，是否造成低于 0x10000 地址的内存的浪费？

- 计划：
    + 1、实现调试工具
        + 实现工具函数 printf()
        + 实现显示函数调用栈，写出一篇分析栈帧的文章，作为笔记保存
    + 2、实现中断描述符表
    + 3、实现进程创建

---

- 2018-04-24 16:47:10
- 改动：
    + 发现了 .md 文件名字错误，改正
    + tools 文件夹下加入了连接器脚本 link.ld，修改了 Makefile 相应处

- 进度：
    + 理解了 elf 文件格式，
    
- 记录：
    + bootloader 加载内核时，先加载最初8个扇区，原因：
        * elf 文件是由头部索引的，elf header 和 program header table 的长度不固定，elf header 指出了 program header table 的位置，program header table 指出了 elf 文件中各个 segment 的位置。加载 kernel ，需要找到各个 segment 的位置，必须保证所有的 elf header 和 program header table 被加载进内存。然而，program header table 的表项目数量和每个entry的大小是不固定的。加载8个扇区仅是一种较为稳妥的做法。
    + bootmain() 理解很重要，详见程序注释。

- 问题mark：
    + 进程镜像与 elf 文件格式的理解。
    + C语言全局变量，BSS段；将内存地址转换为函数执行；内联汇编（知晓但不熟悉，更写不出来）
