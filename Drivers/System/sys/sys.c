#include "./sys/sys.h"


/**
 * @brief       系统时钟配置函数
 * @param       plln: 主PLL倍频系数(PLL_N)，取值范围：64~432
 * @param       pllm: 主PLL输入分频系数(PLL_M)，取值范围：2~63
 *                  作用：将输入时钟(HSI/HSE)分频后送入PLL
 * @param       pllp: 主PLL系统时钟分频系数(PLL_P)，仅支持：2/4/6/8
 *                  作用：将VCO输出分频后作为系统时钟(SYSCLK)
 * @param       pllq: 主PLL外设时钟分频系数(PLL_Q)，取值范围：2~15
 *                  作用：将VCO输出分频后用于USB/SDIO/RNG等外设
 * @note
 *              核心计算公式：
 *              - VCO频率 = 输入时钟频率 × (plln / pllm)
 *              - 系统时钟(SYSCLK) = VCO频率 / pllp
 *              - PLL_Q输出时钟 = VCO频率 / pllq
 *              
 *              典型配置(外部8MHz晶振)：
 *              plln=336, pllm=8, pllp=2, pllq=7
 *              计算结果：
 *              - VCO频率 = 8MHz × (336/8) = 336MHz
 *              - 系统时钟 = 336MHz / 2 = 168MHz（F407最大支持值）
 *              - PLL_Q输出 = 336MHz / 7 = 48MHz（满足USB外设需求）
 *              
 *              总线时钟分配关系：
 *              - AHB总线时钟(HCLK) = 系统时钟(SYSCLK) / 1 = 168MHz
 *              - APB1总线时钟(PCLK1) = HCLK / 4 = 42MHz（最大不超过50MHz）
 *              - APB2总线时钟(PCLK2) = HCLK / 2 = 84MHz（最大不超过100MHz）
 * @attention   1. 输入时钟需先使能(如HSE_ON或HSI_ON)
 *              2. VCO频率不得超过336MHz(F407系列限制)
 *              3. 配置后需确保各总线时钟不超过手册规定最大值
 * @retval      错误代码：0-配置成功；1-配置失败
 */
uint8_t SystemClock_Config(void) 
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // 1. 配置振荡器（HSE=8MHz 外部晶振，启用PLL）
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;              // 使能外部高速晶振
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;          // 启用PLL
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;  // PLL时钟源为HSE
    RCC_OscInitStruct.PLL.PLLM = 8;                       // HSE(8MHz)分频为1MHz
    RCC_OscInitStruct.PLL.PLLN = 336;                     // 倍频到336MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;           // 分频为168MHz（CPU主频）
    RCC_OscInitStruct.PLL.PLLQ = 7;                       // 用于USB等外设的分频
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) 
    {
        return 1;  // 配置失败
    }

    // 2. 配置系统时钟和总线时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // 系统时钟=PLL输出(168MHz)
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;         // HCLK=168MHz（AHB总线）
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;          // PCLK1=42MHz（APB1总线，最大50MHz）
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;          // PCLK2=84MHz（APB2总线，最大100MHz）
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) 
    {
        return 1;  // 配置失败
    }
    
    return 0;
}
