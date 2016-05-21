Report for Lab7
计34 茹逸中 2013011375

**练习1：**

1. 比较代码区别

lab7和lab6的区别主要体现在同步互斥的实现上。lab6采用锁的方式实现同步互斥，而lab7则采用信号量的方式。
为实现信号量同步互斥，lab7实现了sem结构，同时还实现了等待队列、计时器和禁用中断。
在运行sem的原子操作时，会禁用中断。在一个进程由于得不到信号量而阻塞时，会加入该信号量的等待队列并sleep，然后创建一个计时器，在计时器超时后，会返回该资源无法获得。

2. 给出内核级信号量的设计描述，并说其大致执行流程。

信号量的结构定义为

```
typedef struct {
    int value;
    wait_queue_t wait_queue;
} semaphore_t;
```

并实现了三个主要的函数

```
void up(semaphore_t *sem);
void down(semaphore_t *sem);
bool try_down(semaphore_t *sem);
```

结构中的value表示该资源剩余的数量，若value<0，则表示有进程在等待该资源，此时所有的等待进程都在等待队列wait_queue中。

如果一个进程需要申请该资源，则应调用down函数。如果该资源还有剩余(value > 0)，则给该进程分配一个该资源，value值减1。
否则，进程被阻塞，转为sleep状态，value减1并将该进程加入该资源的等待队列。
如果在申请资源时不希望因没有资源而阻塞，则应调用try_down函数，此时如果没有资源则不会阻塞，返回false。如果有资源返回true。

当一个进程需要释放资源时，调用up函数。up函数会将value值加1。如果value值加一后仍然<=0，则表示有进程在等待，将等待队列中的第一个进程唤醒。

3. 请在实验报告中给出给用户态进程/线程提供信号量机制的设计方案，并比较说明给内核级提供信号量机制的异同。

为用户态提供创建信号量、初始化信号量、获得信号量资源、尝试获得信号量资源、释放信号量资源的系统调用。这些系统调用在实现时都可以直接调用内核信号量操作的函数。

在用户态使用信号量机制时，需要先切换至内核态。因此需要采用系统调用来实现。其它方面都是相同的。

**练习2：**

1. 请在实验报告中给出内核级条件变量的设计描述，并说其大致执行流程。

主要实现了条件信号和条件等待两个函数。

```
void cond_signal (condvar_t *cvp) 
void cond_wait (condvar_t *cvp)
```

当一个进程调用cond_signal时，会先根据cvp->count是否大于0来判断有没有进程在等待这个条件信号。
如果不大于0，则表示没有进程在等待，则忽略该信号，函数退出。
如果大于0，则唤醒等待这个信号量的进程。由于管程限制了只有1个进程能在临界区中执行，因此唤醒其它进程后，当前进程需要进入睡眠状态。

当一个进程调用cond_wait时，因等待该条件而睡眠的进程数加一。
如果next_count小于等于0，表示目前没有进程发出该条件信号，那需要唤醒的是由于互斥条件限制而无法进入临界区的进程，所以要唤醒睡在mutex上的进程。然后进程A进入睡眠状态，等待信号。
如果next_count大于0，说明有大于等于1个进程执行cond_signal且睡着了，所以唤醒mtp的next进程，然后进程进入睡眠状态，等待信号。

2. 请在实验报告中给出给用户态进程/线程提供条件变量机制的设计方案，并比较说明给内核级提供条件变量机制的异同。

与在用户态提供信号量机制的设计类似，需要使用系统调用来调用相应的内核函数。

