Report for Lab4
计34 茹逸中 2013011375

**练习1：**

实现过程：

在新建一个结构时，需要先为该进程的结构申请一个空间并初始化各元素的值，将结构中各元素清零或置于初始状态。

思考题：

context数进程的上下文（即寄存器的值），trapframe是进程的中断信息(包括中断字、栈地址等等)。
context用于在进程切换时保存（上一个进程）和恢复（下一个）进程运行的上下文。
trapframe保存了进程在会发生进程切换中断前的信息，用于恢复进程在中断前的状态。
在本实验需要编写的代码中，由于没有涉及到进程切换相关的代码，只需将它们置零即可。

**练习2：**

实现过程：

根据提示，依次完成以下步骤：

1.调用alloc_proc，首先获得一块用户信息块。

2.为进程分配一个内核栈。

3.复制原进程的内存管理信息到新进程（但内核线程不必做此事）

4.复制原进程上下文到新进程

5.将新进程添加到进程列表

6.唤醒新进程

7.返回新进程号

思考题：

ucore不保证为每个新建的线程分配唯一id，但保证存活的进程id互不相同。

分析get_pid函数可以发现，在分配pid时，会跳过所有存活的进程的id。

**练习3：**

proc_run函数恢复了新进程的内核栈基地址和页目录基地址，然后跳转到switch_to函数

switch_to函数是汇编代码编写的函数，主要将上下文（寄存器的值）保存到from中，将to中的上下文恢复。

