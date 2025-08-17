#include "./sys/sys.h"
#include "./usart/usart.h"
#include "./delay/delay.h"
#include "./LED/led.h"
#include "freertos_demo.h"

int main(void)
{
    // 系统初始化
    HAL_Init();
    SystemClock_Config();
    Delay_Init(168);        // 延时函数初始化
    usart_init(115200);     // 串口2初始化
    
    led_init();
    
    freertos_demo();        // 运行 FreeRTOS 例程
}

