1.请以键盘输入、到标准输出设备stdout的printf输出、串口输出、磁盘文件复制为例，描述ucore操作系统I/O从请求到完成的整个执行过程，并分析I/O过程的时间开销。

ucore操作系统中，cprintf输出的函数调用栈如下
a. cprintf(const char \*fmt,args) 提供给上层的输出函数，调用格式与c中printf的调用格式类似。

b. cprintf创建了一个参数列表后调用了vcprintf

c. vcprintf调用了vprintfmt，并规定使用"cputch"函数进行输出，规定输出到stdout

d. vprintfmt(void (\*putch)(int, void\*, int), int fd, void \*putdat, const char \*fmt, va_list ap)，分析格式字符串fmt，组成字符流使用给定的putch函数进行输出。putch函数决定了输出的位置。

若putch=cputch，cputch会调用cons_putc
cons_putc会分别调用lpt_putc、cga_putc、serial_putc将字符输出到并口、屏幕和串口
