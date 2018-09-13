/***********************************************************
 * 版权信息:
 * 文件名称: system_init.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能:
 * 版本历史:
 ***********************************************************/
#include "stm32l0xx_hal.h"
#include "system_init.h"
#include "xprintf.h"
#include "tnos_dev.h"


/***********************************************************
 * 功能描述：系统时钟初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void sys_clock_init(void)//32M
{
    int timeout;

    LL_RCC_PLL_Disable();   //关闭PLL

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

#if 1 //内部16M晶振

    LL_RCC_HSI_Enable();  //使能内部时钟

    LL_RCC_HSI_DisableDivider();

    timeout = 50000;

    while(timeout--)  //等待内部时钟稳定
    {
        if(LL_RCC_HSI_IsReady())
        {
            break;
        }
    }

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_2);  //配置PLL时钟，为32MHZ

#else //外部8M晶振

    LL_RCC_HSE_Enable();   //使能外部时钟

    timeout = 50000;

    while(timeout--)
    {
        if(LL_RCC_HSE_IsReady())
        {
            break;
        }
    }

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_8, LL_RCC_PLL_DIV_2);
#endif

    LL_RCC_PLL_Enable();  //使能PLL时钟

    timeout = 50000;

    while (timeout--)
    {
        if(LL_RCC_PLL_IsReady())  //等待PLL时钟稳定
        {
            break;
        }
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);  //设置系统时钟源为PLL时钟

    timeout = 50000;

    while (timeout--)
    {
        if (LL_RCC_SYS_CLKSOURCE_STATUS_PLL == LL_RCC_GetSysClkSource())
        {
            break;
        }
    };

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);  //AHB分频比为1
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);   //APB1分频比为1
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);   //APB2分频比为1

    // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
    LL_SetSystemCoreClock(32000000);

//    LL_RCC_ConfigMCO(RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_8);  //设置时钟输出
}

/***********************************************************
 * 功能描述：中断优先级初始化
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void nvic_config(void)
{
    NVIC_SetPriority(USART4_5_IRQn, 1);
    NVIC_SetPriority(TIM3_IRQn, 2);
}
//
///// 重定向c库函数printf到USART1
//int fputc(int ch, FILE *f)
//{
//    /* 发送一个字节数据到USART1 */
//
//    USART1->DR = ch & 0xFF;
//
//    /* 等待发送完毕 */
//    while (!(USART1->SR & USART_FLAG_TXE))
//    {
//    }
//
//    return (ch);
//}
//

/***********************************************************
 * 函数名称：uart_config
 * 功能描述：uart_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void uart_config(void)
{

}

/***********************************************************
 * 函数名称：spi_config
 * 功能描述：spi_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void spi_config(void)
{

}


/***********************************************************
 * 函数名称：timer_config
 * 功能描述：timer_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void timer_config(void)
{
//	/* Time base configuration */
//	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
//
//
//	TIM_TimeBaseStructure.TIM_Period = 18000 + 400;//计数为TIM_Period+1	  //延时1ms时的设置
//	TIM_TimeBaseStructure.TIM_Prescaler = 3;//分频数位TIM_Prescaler+1
//
//
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//	/* TIM enable counter */
//	TIM_Cmd(TIM2, ENABLE);
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );

}


/***********************************************************
 * 函数名称：io_config
 * 功能描述：io_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void io_config(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	EXTI_InitTypeDef	EXTI_InitStructure;
//
//  GPIO_InitStructure.GPIO_Pin  = PA0|PA1|PA2|PA3|PA8;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//
//	GPIO_InitStructure.GPIO_Pin = PB0|PB1|PB5;//PB0:5100中断,PB1遥控
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin  = PB12;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin  = PB2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin  = PB6|PB7;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//  GPIO_SetBits(GPIOB,PB6|PB7);
//
//
//	GPIO_InitStructure.GPIO_Pin  = PC0|PC1|PC2|PC3|PC4|PC5|PC6|PC7|PC8;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOA, R_W5100);
//
//
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);



}

/***********************************************************
 * 功能描述：板子硬件系统时钟和优先级初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:为了后面初始化打印和定时器做准备
 ***********************************************************/
void tnos_board_sys_pro_init(void)
{
    sys_clock_init();

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);  //打开GPIOA的时钟
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); //打开GPIOB的时钟
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);//打开GPIOC的时钟
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);//打开GPIOD的时钟
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);//打开GPIOE的时钟
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);//打开GPIOH的时钟

    nvic_config();
}


/***********************************************************
 * 功能描述：板子硬件其它初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_board_other_init(void)
{
	io_config();

  	uart_config();

	timer_config();
}

