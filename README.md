# LearnQt

2019/12/05</br>
服务器GUI主线程退出时，数据库线程没有退出，这里是一个bug。

2019-12-17</br>
使用线程的<font color="red">**requestInterruption**</font>函数发送终止信号，在线程run函数里查询<font color="red">**isInterruptionRequested**</font>的方式不好，<font color="red">**isInterruptionRequested**</font>这个函数阻塞太久，主线程退出了，线程的run函数很长一段时间都阻塞在这里，导致用户以为程序出现bug不能正常退出。

## 多线程

---

Qt使用多线程有两种方式：

* *继承QThread，重写QThread的run虚函数。*

这种方式最直接，也最直观，与Windows的多线程类似，在run函数中直接无限循环处理事务。但有一点需要注意的是继承QThread的线程对象只有在run函数内部使用的数据才属于该线程的，换句话说，线程对象不在run函数中的其他成员变量属于创建该线程对象的线程，比如主线程。

```c++
class Thread : public QThread
{
    public:
        Thread();
        virtual ~Thread();
    public:
        void run() override;// 此函数才是线程对象的入口函数
    public:
        //入口函数需要的一些参数
        //...
        //线程对象其他参数--属于主线程
};
```

可以发现该线程对象处理的对象未知，要么前向申明，让该线程对象认识需要处理的对象，要么处理全局变量。

* 继承QObject，使用对象的moveToThread函数

继承自QThread的线程对象最大的缺陷就是在定义的时候不知道它该作用在哪个对象上面。如果是全局变量还好，对于局部生成的对象则无能为力。在实际的工作中，我们往往需要动态的创建线程去执行一些耗时的操作，比如socket通信时，实时建立线程处理新的连接。

使用QObject的moveToThread的多线程方式基本思想是：

1. 把耗时的操作封装成对象，继承QObject;

2. 申明耗时操作函数及其需要的参数，该参数可从别的线程里的信号里获取；

3. 把该对象移动到线程里，即表明子线程拥有该对象；

4. 如果必要的话，定义槽函数对外发送处理结果；

5. 其他线程，比如主线程，把某个触发信号与对象的耗时操作函数关联起来，每次触发信号发生时，执行一次耗时操作，而不是以前的无限循环执行。

```c++
class Worker : public QObject
{
    Q_OBJECT
    public:
        //构造函数，析构函数

    public slots://耗时的槽函数
    //这里要注意的是，线程调用start函数并不会
    //执行该函数。该函数由外部信号触发，所以，
    //在函数内部不是无限循环，执行完就发送执行完毕信号，比如result信号
        void dowork(...);
    
    signals:
        void result(...); // 槽函数处理完对外发送的信号
};
```

把一个QObject对象移动到线程中时，该对象的所有资源都属于线程的，包括槽函数，该对象就是线程的子类（QThread继承自QObject）。一个QObject只能移动给一个线程，即它只能有一个父类，不能移动给多个线程。