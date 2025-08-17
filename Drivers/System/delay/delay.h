#ifndef __DELAY_H_
#define __DELAY_H_

#include "./sys/sys.h"

/* 宏定义 */


/* 函数声明 */
void Delay_Init(uint8_t SYSCLK);
void Delay_us(uint32_t us);
void Delay_ms(uint16_t ms);

/* 变量声明 */



#endif
