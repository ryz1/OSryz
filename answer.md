12.4.3

sleep系统调用：

在process.c的do_sleep函数中，先保存各寄存器的值，然后设置定时器，然后将进程的状态设置为sleep，并由计时器结束时唤醒。

进程sleep后会进入sched_class中

由wakeup_proc函数唤醒
