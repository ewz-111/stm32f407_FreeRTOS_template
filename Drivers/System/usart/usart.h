#ifndef __USART_H_
#define __USART_H_

#include "stdio.h"
#include "./sys/sys.h"

/* 宏定义 */
#define USART_TX_GPIO_PORT              GPIOA
#define USART_TX_GPIO_PIN               GPIO_PIN_2
#define USART_TX_GPIO_AF                GPIO_AF7_USART2
#define USART_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 发送引脚时钟使能 */

#define USART_RX_GPIO_PORT              GPIOA
#define USART_RX_GPIO_PIN               GPIO_PIN_3
#define USART_RX_GPIO_AF                GPIO_AF7_USART2
#define USART_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* 接收引脚时钟使能 */

#define USART_UX                        USART2
#define USART_UX_IRQn                   USART2_IRQn
#define USART_UX_IRQHandler             USART2_IRQHandler
#define USART_UX_CLK_ENABLE()           do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0)  /* USART2 时钟使能 */


#define CIRC_BUFF_SIZE      256     // 循环缓冲区大小
#define RX_SIZE             20      // 单次处理数据的最大长度
#define PACKET_HEADER       0xAA    // 包头
#define PACKET_FOOTER       0x55    // 包尾


/* 函数声明 */
void usart_init(uint32_t baudrate);
void receive_packet(void);
void USART2_SendChar(uint8_t ch);
void USART2_SendPacket(uint8_t *data, uint16_t len);
void USART2_SendString(uint8_t *data, uint8_t length);

/* 变量声明*/






#endif
