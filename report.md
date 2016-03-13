Report for Lab1
计34 茹逸中 2013011375
**练习1: 理解通过make生成执行文件的过程。**
**1.1 如何生成ucore.img**

注意到，生成ucore.img需要bootblock和kernel，即操作系统的启动模块和核心模块。

**生成bootblock的相关代码为**
```
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

$(call create_target,bootblock)
```

执行make "V="时观察到，生成bootblock需要bootasm.o、bootmain.o和sign
**生成bootasm.o和bootmain.o的相关代码为**

```
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))
```

实际代码为
```
gcc -Iboot/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootasm.S -o obj/boot/bootasm.o
gcc -Iboot/ -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootmain.c -o obj/boot/bootmain.o
```
bootasm.o的源文件为bootasm.S，bootmain.o的源文件为bootmain.c
一些参数的含义：
-Iboot/,-Ilibs/ 在./boot/和./libs/目录下搜索头文件
-fno-builtin 不承认不以__builtin_开头的函数为内建（built-in）函数
-Wall 提示所以warning
-ggdb 生成调试信息
-m32 生成32位程序
-gstabs 生成stabs格式的调试信息
-nostdinc 不使用标准库
-fno-stack-protector 不检测缓存区溢出

**生成sign的相关代码为**

```
$(call add_files_host,tools/sign.c,sign,sign)
$(call create_target_host,sign,sign)
```

实际代码为
```
gcc -Itools/ -g -Wall -O2 -c tools/sign.c -o obj/sign/tools/sign.o
gcc -g -Wall -O2 obj/sign/tools/sign.o -o bin/sign
```
即将源程序sign.c编译成sign.o,然后编译成sign

由此生成了bootblock的所有所需文件，生成bootblock.o的代码为
```
ld -m elf_i386 -nostdlib -N -e start -Ttext 0x7C00 obj/boot/bootasm.o obj/boot/bootmain.o -o obj/bootblock.o
```
一些参数的含义：
-m elf_i386 生成在elf_i386机器上的链接器
-N 设置代码段也可读写
-e start 设置程序入口为start
-Ttext 0x7C00 设置代码段的起始地址为0x7C00

然后使用objdump命令将bootblock.o的二进制代码拷贝到bootblock.out中
```
objcopy -S -O binary obj/bootblock.o obj/bootblock.out
```

使用sign工具处理后得到bootblock
```
bin/sign obj/bootblock.out bin/bootblock
```

接下来生成kernel
**生成kernel的相关代码为**
```
kernel = $(call totarget,kernel)

$(kernel): tools/kernel.ld

$(kernel): $(KOBJS)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -T tools/kernel.ld -o $@ $(KOBJS)
	@$(OBJDUMP) -S $@ > $(call asmfile,kernel)
	@$(OBJDUMP) -t $@ | $(SED) '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(call symfile,kernel)

$(call create_target,kernel)
```

生成kernel所需要的文件为kernel.ld和一些静态链接库。
其中kernel.ld已存在，生成这些静态链接库的相关代码为
```
$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))
```
接下来使用ld命令生成kernel，此时生成ucore.img的准备工作已完成

**生成ucore.img的相关代码为**
```
COREIMG	:= $(call totarget,ucore.img)

$(UCOREIMG): $(kernel) $(bootblock)
	$(V)dd if=/dev/zero of=$@ count=10000  //1
	$(V)dd if=$(bootblock) of=$@ conv=notrunc  //2
	$(V)dd if=$(kernel) of=$@ seek=1 conv=notrunc  //3

$(call create_target,ucore.img)
```

第一句话表示生成一个文件，共含有10000个块
第二句话表示将bootblock载入到文件的开头（即第一个块）
第三句话表示将kernel载入到文件中的第2个块及以后

**1.2 一个被系统认为是符合规范的硬盘主引导扇区的特征是什么?**
观察sign.c
一个主引导扇区有512个字节，且倒数第二个字节是0x55，最后一个字节是0xAA

**练习2：使用qemu执行并调试lab1中的软件。**
**2.1 从 CPU 加电后执行的第一条指令开始,单步跟踪 BIOS 的执行。**
为单步跟踪，在debug命令调用qemu指令中加入参数"-d in_asm -D debug.log"参数，将允许的汇编指令全部输出到debug.log中方便调试。
删除tools/gdbinit中的"continue"。

**2.2 在初始化位置0x7c00设置实地址断点,测试断点正常。**
在lab1根目录下运行make debug来进入gdb调试模式。
在gdb中输入命令 set architecture i8086进入i8086模式
设置断点：b *0x7c00
使程序开始运行：c
此时程序正确进入0x7c00处
通过x /10i $pc可以看到0x7c00及之后的10条汇编代码，分别是
```
cli
cld
xor %ax %ax
mov %ax %ds
mov %ax %es
mov %ax %ss
in $0x64 %al
...
```

**2.3 从0x7c00开始跟踪代码运行,将单步跟踪反汇编得到的代码与bootasm.S和 bootblock.asm进行比较。**
在2.2的基础上观察20个汇编指令，发现其与bootasm.S和bootblock.asm中的代码相同。

**2.4 自己找一个bootloader或内核中的代码位置，设置断点并进行测试。**
测试练习5中需要编写的print_stackframe函数，使用b print_stackframe命令，然后使用c命令后正确进入print_stackframe。此后可以使用s或者n命令进行单步测试。

**练习3：分析bootloader进入保护模式的过程。**

从0x7c00开始时，需要将flag和段寄存器清0。
由于保护模式需要用到全部的4GB(32位)内存，即要用到全部的32根地址线，因此需要开启A20。

第一步：开启A20
	seta20.1:               # 等待8042键盘控制器不忙
	    inb $0x64, %al      # 
	    testb $0x2, %al     #
	    jnz seta20.1        #
	
	    movb $0xd1, %al     # 发送写8042输出端口的指令
	    outb %al, $0x64     #
	
	seta20.1:               # 等待8042键盘控制器不忙
	    inb $0x64, %al      # 
	    testb $0x2, %al     #
	    jnz seta20.1        #
	
	    movb $0xdf, %al     # 打开A20
	    outb %al, $0x60     # 

第二步：初始化GDT表
		lgdt gdtdesc

第三步：将cr0寄存器的PE位置置1便进入了保护模式
		movl %cr0, %eax
		orl $CR0_PE_ON, %eax
		movl %eax, %cr0
		
第四步：更新cs基地址
	    ljmp $PROT_MODE_CSEG, $protcseg
	.code32
	protcseg:

第五步：设置段寄存器，并建立堆栈
	    movw $PROT_MODE_DSEG, %ax
	    movw %ax, %ds
	    movw %ax, %es
	    movw %ax, %fs
	    movw %ax, %gs
	    movw %ax, %ss
	    movl $0x0, %ebp
	    movl $start, %esp

由此进入了保护模式，调用bootmain
	    call bootmain

**练习4: 分析bootloader加载ELF格式的OS的过程。**

readsect从设备的第secno扇区读取数据到dst位置
	static void
	readsect(void *dst, uint32_t secno) {
	    waitdisk();
	
	    outb(0x1F2, 1);                         // 设置读取扇区的数目1
	    outb(0x1F3, secno & 0xFF);
	    outb(0x1F4, (secno >> 8) & 0xFF);
	    outb(0x1F5, (secno >> 16) & 0xFF);
	    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
	        // 制定扇区号，29-31位强制设为1，28位(=0)表示访问"Disk 0"，0-27位是28位的偏移量
	    outb(0x1F7, 0x20);                      // 0x20命令，读取扇区
	
	    waitdisk();

	    insl(0x1F0, dst, SECTSIZE / 4);         // 读取到dst位置
		
	}

readseg简单包装了readsect，可以从设备读取任意长度的内容。
	static void
	readseg(uintptr_t va, uint32_t count, uint32_t offset) {
	    uintptr_t end_va = va + count;
	
	    va -= offset % SECTSIZE;
	
	    uint32_t secno = (offset / SECTSIZE) + 1; 
	    // 加1因为0扇区被引导占用，ELF文件从1扇区开始
	
	    for (; va < end_va; va += SECTSIZE, secno ++) {
	        readsect((void *)va, secno);
	    }
	}


在bootmain函数中，
	void
	bootmain(void) {
	    // 首先读取ELF的头部
	    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);
	
	    // 通过储存在头部的幻数判断是否是合法的ELF文件
	    if (ELFHDR->e_magic != ELF_MAGIC) {
	        goto bad;
	    }
	
	    struct proghdr *ph, *eph;
	
	    // ELF头部有描述ELF文件应加载到内存什么位置的描述表，
	    // 先将描述表的头地址存在ph
	    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
	    eph = ph + ELFHDR->e_phnum;
	
	    // 按照描述表将ELF文件中数据载入内存
	    for (; ph < eph; ph ++) {
	        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
	    }
	    // ELF文件0x1000位置后面的0xd1ec比特被载入内存0x00100000
	    // ELF文件0xf000位置后面的0x1d20比特被载入内存0x0010e000

	    // 根据ELF头部储存的入口信息，找到内核的入口
	    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();
	
	bad:
	    outw(0x8A00, 0x8A00);
	    outw(0x8A00, 0x8E00);
	    while (1);
	}

**练习5：实现函数调用堆栈跟踪函数**
实现后输出与题目描述中基本相同。
最后一行是unkown信息，具体如下：
	ebp:0x00007bf8 eip:0x00007d68 args:0x00000000 0x00000000 0x00000000 0x00007c4f
	    <unknow>: -- 0x00007d67 --
这是函数调用的最后一层，即操作系统载入后的第一次函数调用，对应的是call bootmain
由于bootloader设置的设置堆栈是从0x7c00开始，一次函数调用需要8字节的栈空间(分别是ebp和eip占用4个字节)，所以bootmain中ebp为0x7bf8。

**练习6：完善中断初始化和处理**
**6.1 中断描述符表（也可简称为保护模式下的中断向量表）中一个表项占多少字节？其中哪几位代表中断处理代码的入口？**
中断向量表一个表项占用8字节，第2-3字节是段选择项，0-1字节和6-7字节拼成段位移项。

**扩展练习 Challenge 1**
完成了challenge1，主要修改trap.c和init.c。
在trap.c中，实现T_SWITCH_TOU和T_SWITCH_TOK两个异常。
在跳转到用户模式中，首先需要设置cs,ds,es和ss。然后将调用io的权限降低。
在跳转回内核模式中，也需要设置cs,ds,es和ss。
在init.c中，首先需要将用户态调用SWITCH_TOK中断的权限打开。
然后在lab1_switch_to_user中调用T_SWITCH_TOU中断，在lab1_switch_to_kernel中调用T_SWITCH_TOK中断。

**总结**
基本完成了练习1-练习6的任务。对比标准答案，调试方法和代码的实现方法略有不同，但都能实现最终的目的。
这次实验主要涵盖了内核态和用户态的转换、中断异常处理、操作系统的载入、堆栈操作等知识。




