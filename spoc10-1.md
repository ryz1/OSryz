1.���Լ������롢����׼����豸stdout��printf�������������������ļ�����Ϊ��������ucore����ϵͳI/O��������ɵ�����ִ�й��̣�������I/O���̵�ʱ�俪����

ucore����ϵͳ�У�cprintf����ĺ�������ջ����
a. cprintf(const char \*fmt,args) �ṩ���ϲ��������������ø�ʽ��c��printf�ĵ��ø�ʽ���ơ�

b. cprintf������һ�������б�������vcprintf

c. vcprintf������vprintfmt�����涨ʹ��"cputch"��������������涨�����stdout

d. vprintfmt(void (\*putch)(int, void\*, int), int fd, void \*putdat, const char \*fmt, va_list ap)��������ʽ�ַ���fmt������ַ���ʹ�ø�����putch�������������putch���������������λ�á�

��putch=cputch��cputch�����cons_putc
cons_putc��ֱ����lpt_putc��cga_putc��serial_putc���ַ���������ڡ���Ļ�ʹ���
