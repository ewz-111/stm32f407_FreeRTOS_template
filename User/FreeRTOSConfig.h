/*
 * FreeRTOS配置文件
 * 适用于STM32F4系列单片机，保留核心功能
 */

#ifndef __FREERTOS_CONFIG_H
#define __FREERTOS_CONFIG_H

#include "./sys/sys.h"

/*-------------------------- FreeRTOS中断服务函数相关定义 --------------------------*/
#define xPortPendSVHandler                              PendSV_Handler
#define vPortSVCHandler                                 SVC_Handler

/*-------------------------- 中断优先级相关配置 --------------------------*/
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5                         // FreeRTOS可管理的最高中断优先级
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - __NVIC_PRIO_BITS) )

/*-------------------------- 硬件相关配置 -------------------------- */
#define configCPU_CLOCK_HZ                              (168000000UL)             // 系统时钟频率
#define configTICK_RATE_HZ                              (1000UL)                  // 滴答定时器频率（1ms中断一次）

/*-------------------------- 任务调度配置 --------------------------*/
#define configUSE_PREEMPTION                            1                         // 使用抢占式调度
#define configUSE_TIME_SLICING                          1                         // 启用时间片调度（同优先级任务轮转）
#define configMAX_PRIORITIES                            5                         // 最大任务优先级数量（0为最低优先级）
#define configMINIMAL_STACK_SIZE                        128                       // 空闲任务堆栈大小（单位：字，STM32为32位，1字=4字节）
#define configMAX_TASK_NAME_LEN                         16                        // 任务名称最大长度（包含结束符）
#define configTICK_TYPE_WIDTH_IN_BITS                   TICK_TYPE_WIDTH_32_BITS   // 滴答计数类型（32位足够大多数场景）

/*-------------------------- 内存管理配置 --------------------------*/
#define configSUPPORT_DYNAMIC_ALLOCATION                1                         // 支持动态内存分配（创建任务、队列等）
#define configTOTAL_HEAP_SIZE                           (1024 * 10)  // 10KB      // 内存堆总大小（根据实际需求调整，单位：字节）

/*-------------------------- 基础功能配置 --------------------------*/
#define configUSE_TASK_NOTIFICATIONS                    1                         // 启用任务通知功能（轻量级的任务间通信）
#define configUSE_QUEUES                                1                         // 启用队列功能（任务间消息传递）
#define configUSE_MUTEXES                               1                         // 启用互斥锁
#define configUSE_COUNTING_SEMAPHORES                   1                         // 启用信号量

/*-------------------------- 钩子函数配置 --------------------------*/
#define configUSE_IDLE_HOOK                             0                         // 1: 使能空闲任务钩子函数, 无默认需定义
#define configUSE_TICK_HOOK                             0                         // 1: 使能系统时钟节拍中断钩子函数, 无默认需定义
#define configCHECK_FOR_STACK_OVERFLOW                  0                         // 1: 使能栈溢出检测方法1, 2: 使能栈溢出检测方法2, 默认: 0
#define configUSE_MALLOC_FAILED_HOOK                    0                         // 1: 使能动态内存申请失败钩子函数, 默认: 0 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK              0                         // 1: 使能定时器服务任务首次执行前的钩子函数, 默认: 0
                                                                                  
/*-------------------------- 包含的API函数 --------------------------*/           
#define INCLUDE_vTaskPrioritySet                        1                         // 设置任务优先级
#define INCLUDE_uxTaskPriorityGet                       1                         // 获取任务优先级
#define INCLUDE_vTaskDelete                             1                         // 删除任务
#define INCLUDE_vTaskSuspend                            1                         // 挂起任务
#define INCLUDE_xResumeFromISR                          1                         // 恢复在中断中挂起的任务
#define INCLUDE_vTaskDelayUntil                         1                         // 任务绝对延时
#define INCLUDE_vTaskDelay                              1                         // 任务延时
#define INCLUDE_xTaskGetSchedulerState                  1                         // 获取任务调度器状态
#define INCLUDE_xTaskGetCurrentTaskHandle               1                         // 获取当前任务的任务句柄
#define INCLUDE_xTaskGetSchedulerState                  1                         // 获取任务调度器状态

/*-------------------------- 调试配置 --------------------------*/
#define configASSERT(x) if((x) == 0) { taskDISABLE_INTERRUPTS(); for(;;); }     // 断言宏（调试时触发，死机便于定位问题）



#endif





