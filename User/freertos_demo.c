#include "freertos_demo.h"
#include "./LED/led.h"
#include "./usart/usart.h"
/********************************FreeRTOS*************************************/
#include "FreeRTOS.h"
#include "task.h"


#define START_TASK_PRIO 1               // 任务优先级
#define START_STK_SIZE 128              // 任务堆栈大小
TaskHandle_t StartTask_Handler;         // 任务句柄
void start_task(void *pvParameters);    // 任务函数

#define TASK1_PRIO 2                    // 任务优先级
#define TASK1_STK_SIZE 128              // 任务堆栈大小
TaskHandle_t Task1Task_Handler;         // 任务句柄
void task1(void *pvParameters);         // 任务函数

/**
* @brief FreeRTOS 例程入口函数
* @param 无
* @retval 无
*/
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t )start_task,                    // 任务函数
                (const char* )"start_task",                     // 任务名称
                (uint16_t )START_STK_SIZE,                      // 任务堆栈大小
                (void* )NULL,                                   // 传入给任务函数的参数
                (UBaseType_t )START_TASK_PRIO,                  // 任务优先级
                (TaskHandle_t* )&StartTask_Handler);            // 任务句柄
    vTaskStartScheduler();
}

/**
* @brief start_task
* @param pvParameters : 传入参数(未用到)
* @retval 无
*/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();                   // 进入临界区
    
    /* 创建任务 1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char* )"task1",
                (uint16_t )TASK1_STK_SIZE,
                (void* )NULL,
                (UBaseType_t )TASK1_PRIO,
                (TaskHandle_t* )&Task1Task_Handler);
    
    vTaskDelete(StartTask_Handler);
    
    taskEXIT_CRITICAL();                    // 退出临界区
}

void task1(void *pvParameters)
{
    while (1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}
