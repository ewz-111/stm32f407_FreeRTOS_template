#include "./usart/usart.h"
#include "./MALLOC/malloc.h"
#include "string.h"

/**
 * @brief   重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口
 * @param   ch: 要发送的字符
 * @param   f: 文件指针（未使用）
 * @return  发送的字符
 */
int fputc(int ch, FILE *f)
{
    while ((USART2->SR & 0X40) == 0);               /* 等待上一个字符发送完成 */

    USART2->DR = (uint8_t)ch;                       /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}

UART_HandleTypeDef g_uart2_handle;

/**
 * @brief 串口2初始化
 * @param baudrate 
 */
void usart_init(uint32_t baudrate)
{
    g_uart2_handle.Instance = USART_UX;                         
    g_uart2_handle.Init.BaudRate = baudrate;                    
    g_uart2_handle.Init.WordLength = UART_WORDLENGTH_8B;      
    g_uart2_handle.Init.StopBits = UART_STOPBITS_1;            
    g_uart2_handle.Init.Parity = UART_PARITY_NONE;             
    g_uart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;       
    g_uart2_handle.Init.Mode = UART_MODE_TX_RX;                 
    HAL_UART_Init(&g_uart2_handle);                            
    
    // 启用接收中断
    __HAL_UART_ENABLE_IT(&g_uart2_handle, UART_IT_RXNE);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if(huart->Instance == USART_UX)                             /* 如果是串口2，进行串口2 MSP初始化 */
    {
        USART_UX_CLK_ENABLE();                                  /* USART2 时钟使能 */
        USART_TX_GPIO_CLK_ENABLE();                             /* 发送引脚时钟使能 */
        USART_RX_GPIO_CLK_ENABLE();                             /* 接收引脚时钟使能 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* TX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;          /* 复用为USART2 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);   /* 初始化发送引脚 */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* RX引脚 */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;          /* 复用为USART2 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 初始化接收引脚 */

        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART2中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 抢占优先级3，子优先级3 */
    }
}

typedef struct
{
    uint8_t data[CIRC_BUFF_SIZE];   // 循环缓冲区
    volatile uint16_t head;         // 写指针
    volatile uint16_t tail;         // 读指针
}CircularBuffer;

CircularBuffer g_circ_struct;           // 初始化循环缓冲区
uint8_t g_rx_buff[RX_SIZE] = {0,};      // 接收缓冲区
volatile uint16_t g_rx_index = 0;       // 有效数据索引，最终值表示接收的个数
volatile uint8_t g_rx_isReady = 0;      // 接收完成标志

/**
 * @brief   向循环缓冲区添加数据
 * @param   data
 * @return  
 */
uint8_t AddToCircularBuffer(uint8_t data)
{
    /* 1.记录头指针下一个位置 */
    uint16_t next_head = (g_circ_struct.head + 1) & 0xFF;
    /* 2.判断循环缓冲区是否为满 */
    if(next_head == g_circ_struct.tail)     // 已满
    {
        return 0;
    }
    else    // 未满
    {
        g_circ_struct.data[g_circ_struct.head] = data;      // 写入数据
        g_circ_struct.head = next_head;         // 更新头指针
    }
    return 1;
}

/**
 * @brief   从循环缓冲区读取数据
 * @param   
 * @return  data: 读取到的数据
 */
uint8_t ReadFromCircularBuffer()
{
    uint8_t data;
    /* 1.读取数据 */
    data = g_circ_struct.data[g_circ_struct.tail];    
    /* 2.更新尾指针 */
    g_circ_struct.tail = (g_circ_struct.tail + 1) & 0xFF;

    return data;
}

// 串口2中断服务函数
void USART2_IRQHandler(void)        
{
    uint8_t rxdata;
    
    if (__HAL_UART_GET_FLAG(&g_uart2_handle, UART_FLAG_RXNE) == SET)                /* 检测UART_FLAG_RXNE，是否接收到数据 */
    {
        // 1.读取接收数据（自动清除RXNE标志）
        rxdata = g_uart2_handle.Instance->DR;

        // 1.读取接收数据（自动清除RXNE标志）
        rxdata = g_uart2_handle.Instance->DR;

        AddToCircularBuffer(rxdata);
    }
}

/**
 * @brief   数据包接收函数，仅包含有效数据
 * @param   
 * @return  
 */
void receive_packet(void)
{
    uint8_t data;
    static uint8_t status = 0;

    while (g_circ_struct.tail != g_circ_struct.head)
    {
        data = ReadFromCircularBuffer();
        switch (status)
        {
            case 0:
                if(data == PACKET_HEADER)
                {
                    g_rx_index = 0;
                    status = 1;
                }
                break;
            case 1:
                if(data == PACKET_FOOTER)
                {
                    g_rx_isReady = 1;         // 接收完毕
                    status = 0;
                }
                else
                {
                    if (g_rx_index < RX_SIZE) 
                    {
                        g_rx_buff[g_rx_index++] = data;
                    } 
                    else 
                    {
                        status = 0;
                        g_rx_index = 0;
                    }
                }
                break;
            
            default:
                break;
        }
    }
}

/*--------------------------- 发送函数 ---------------------------*/
// 发送单字符
// 使用示例：USART2_SendChar('a');
void USART2_SendChar(uint8_t ch) 
{
    uint8_t data = ch;
    HAL_UART_Transmit(&g_uart2_handle, &data, 1, 100);  // 长度为1
}

// 发送数据包
// 使用示例：USART2_SendPacket((uint8_t*)data, sizeof(data));
void USART2_SendPacket(uint8_t *data, uint16_t len) 
{
    // 1.数据缓冲区
    uint8_t g_tx_buffer[len+2];     
    
    // 2.填充包头
    g_tx_buffer[0] = PACKET_HEADER;     
    
    // 3.拷贝有效数据
    memcpy(&g_tx_buffer[1], data, len);
    
    // 4.填充包尾
    g_tx_buffer[len+1] = PACKET_FOOTER;
    
    // 5.发送数据包
    HAL_UART_Transmit(&g_uart2_handle, g_tx_buffer, sizeof(g_tx_buffer), 1000);
}

void USART2_SendString(uint8_t *data, uint8_t length)
{
    uint8_t i;
    for(i = 0; i < length; i++)
    {
        USART2_SendChar(*data++);
    }
}
