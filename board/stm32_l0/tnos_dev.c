/***********************************************************
 * 版权信息:
 * 文件名称: tnos_dev.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: cpu相关
 * 版本历史:
 ***********************************************************/

#include "stm32l0xx_hal.h"
#include "tnos.h"
#include "tnos_dev.h"
#include "xprintf.h"
#include "xtimer.h"

/******************
 * TIM3 定时器说明
 *  自动重载做ttimer获取当前时间功能, 比较做tnos延迟功能
 ********************/

#define TIMER_MS_BIT 0 //ms的分辨率


//定时器重新加载的值
//定时器重载时间设置为2秒,方便 秒计时和定时器1s一次控制
#define TIMER_ARR_S (2)    //重载的秒数,无特性情况不允许修改
#define TIMER_ARR          ((TIMER_ARR_S)*1000*BIT(TIMER_MS_BIT) - 1) //定时器重载值



static char gs_buf_fmt[200];          //打印格式化缓冲区(一次最大的内容)
static char gs_buf_send_irq[2000];    //打印中断输出缓冲区(最大缓存的内容)


/***********************************************************
 * 功能描述：毫秒定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tim_ms_init(void)
{
}


/***********************************************************
 * 功能描述：获取时间(没有加锁)
 * 输入参数：无
 * 输出参数：  ptick 获取的时间,不允许传输NULL
 * 返 回 值：  无
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register u32 s = (((u32)g_tick.s)>>1)<<1; //&0xFFFFFFFE
    register u32 ms = TIM3->CNT>>TIMER_MS_BIT;

    if (ms < g_tick.ms) //定时器溢出
    {
        s += 2;
    }

    if (ms >= 1000)
    {
        ms -= 1000;
        s += 1;
    }

    g_tick.s = s;
    g_tick.ms = ms;

    if (ptick != NULL)
    {
        ptick->s = s;
        ptick->ms = ms;
    }
}

/***********************************************************
 * 功能描述：系统设置定时器溢出时间 (外部调用需要实现)
 * 输入参数：ms 定时时间,如果传入的ms超过定时器最大值 设置未定时器最大值
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tim_ms_set(u32 ms)
{
    if (ms != 0)
    {
        TIM3->SR = 0;

        if (ms < (TIMER_ARR_S*1000 - 500))
        {
#if (TIMER_MS_BIT != 0)
            ms = ms<<TIMER_MS_BIT;
#endif
        }
        else
        {
            ms = (TIMER_ARR_S*1000 - 500)<<TIMER_MS_BIT;
        }

        ms += TIM3->CNT;

        if (ms >= TIMER_ARR + 1)
        {
            ms -= TIMER_ARR + 1;
        }

        TIM3->CCR1 = ms;
    }
}


/***********************************************************
 * 功能描述：定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    TIM3->PSC = 32000>>TIMER_MS_BIT;
    TIM3->ARR = TIMER_ARR;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->DIER = TIM_DIER_CC1IE;
    TIM3->CCR1 = TIMER_ARR/2;
    TIM3->CR1 = TIM_CR1_CEN; //向上计数,循环计算器

    NVIC_EnableIRQ(TIM3_IRQn);
}

/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void TIM3_IRQHandler(void)
{
    tnos_tick_proess();
}

/***********************************************************
 * 功能描述：打印初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
void xprintf_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    UART_HandleTypeDef UartHandle = {0};

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4); //打开串口4时钟

    //PC10 PC11能设置为USART4
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11 | GPIO_PIN_10, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    UartHandle.Instance        = USART4;
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    HAL_UART_Init(&UartHandle);

    NVIC_EnableIRQ(USART4_5_IRQn);


    xprintf_init_com(gs_buf_fmt, sizeof(gs_buf_fmt), gs_buf_send_irq, sizeof(gs_buf_send_irq));
}

/***********************************************************
 * 功能描述：中断 串口0
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  ms时间
 ***********************************************************/
void USART4_5_IRQHandler(void)
{
    char val;
    uint32_t isrflags = USART4->ISR;

    if (isrflags & USART_ISR_TXE)
    {
        if (xprintf_get_char(&val))
        {
            USART4->TDR = val;
        }
        else
        {
            CLEAR_BIT(USART4->CR1, USART_CR1_TXEIE);
        }
    }
}

/***********************************************************
 * 功能描述：普通方式打印输出字符(需要自己实现)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf_put_char(char val)
{
    while((USART4->ISR&0X40)==0);

    USART4->TDR = val;
}

/***********************************************************
 * 功能描述：有数据需要中断发送通知
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf_msg_put_irq(void)
{
    if (READ_BIT(USART4->CR1, USART_CR1_TXEIE) == 0) //中断关闭(发送一个数据唤醒)
    {
        SET_BIT(USART4->CR1, USART_CR1_TXEIE);
    }
}
