This project is a learning project for MCU FreeRTOS

chip:STM32F411CEU6

configure the peripherals by STM32CubeMX IDE

KEIL5

构建自己的工程,在WeAct开发板上。工程包括一个按键控制刂线程和一个控制LED灯的线程以及一个按键中
断处理函数;

中断服务程序

按键线程：

    按键状态机判断

LED线程：

    根据接收的数据进行翻转

队列：

1. ISR到按键线程的队列

2. 按键到LED的队列
