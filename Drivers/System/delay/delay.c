#include "./delay/delay.h"
#include "FreeRTOSConfig.h"

static uint8_t g_fac_us = 0;  // 微秒延时倍乘数

/**
  * @brief  初始化延时函数
  * @param  SYSCLK: 系统时钟频率（单位MHz）
  *                 1us对应的SysTick时钟周期数（如 168MHz 时, 1s -> 168M个时钟周期, 1us -> 168 个时钟周期）
  *                 时钟周期数决定了重装载值
  * @retval 无
  */
void Delay_Init(uint8_t SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK); // 选择外部时钟：HCLK
    g_fac_us = SYSCLK; // 计算微秒延时倍乘数

    uint32_t reload;
    reload = SYSCLK * 1000000 / configTICK_RATE_HZ;     /* 根据configTICK_RATE_HZ设定溢出时间,reload为24位
                                                         * 寄存器,最大值:16777216,在168M下,约合0.09986s左右
                                                         */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;          // 开启SYSTICK中断
    SysTick->LOAD = reload;                             // 每1/configTICK_RATE_HZ秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;           // 开启SYSTICK
}

/**
  * @brief  微秒级延时
  * @param  us: 延时微秒数（范围：0~2^24/g_fac_us）
  * @retval 无
  */
void Delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t t_old, t_now, t_cnt = 0;
    uint32_t reload = SysTick->LOAD;         // LOAD的值
    ticks = us * g_fac_us;                   // 需要的节拍数
    t_old = SysTick->VAL;                    // 记录起始计数值
    
    while(1)
    {
        t_now = SysTick->VAL;                // 读取当前计数值
        if(t_now != t_old)
        {
            // 累计流逝的时钟周期（处理溢出）
            t_cnt += (t_now < t_old) ? (t_old - t_now) : (reload - t_now + t_old);
            t_old = t_now;
            if(t_cnt >= ticks) 
                break; // 达到目标延时，退出
        }
    }
}

/**
  * @brief  毫秒级延时
  * @param  ms: 延时毫秒数
  * @retval 无
  */
void Delay_ms(uint16_t ms)
{
    uint32_t i;
    for (i = 0; i < ms; i++)
    {
        Delay_us(1000); // 调用微秒延时函数实现毫秒延时
    }
}

