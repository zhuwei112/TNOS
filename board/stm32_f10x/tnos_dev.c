/***********************************************************
 * 版权信息:
 * 文件名称: tnos_dev.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: cpu相关
 * 版本历史:
 ***********************************************************/

#include "stm32f10x.h"
#include "tnos.h"
#include "tnos_dev.h"
#include "xprintf.h"
#include "xtimer.h"

/******************
 * TIM34定时器说明
 *  自动重载做ttimer获取当前时间功能, 比较做tnos延迟功能
 ********************/


#define TIMER_MS_BIT 1 //ms的分辨率

//定时器重新加载的值
//定时器重载时间设置为2秒,方便 秒计时和定时器1s一次控制
#define TIMER_ARR_S (2) //重载的秒数(无特殊情况不需要修改)
#define TIMER_ARR ((TIMER_ARR_S)*1000*BIT(TIMER_MS_BIT) - 1)

#if (TIMER_ARR_S <= 1)
    #error TIMER_ARR_S mster > 1
#endif

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
 * 输出参数：  ptick 获取的时间
 * 返 回 值：  无
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register u32 s = (((u32)g_tick.s)>>1)<<1; //&0xFFFFFFFE
    register u32 ms = TIM4->CNT>>TIMER_MS_BIT;

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
    u32 cnt;

    if (ms != 0)
    {
        TIM4->SR = 0;

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

        ms += TIM4->CNT;

        if (ms >= TIMER_ARR + 1)
        {
            ms -= TIMER_ARR + 1;
        }

        TIM4->CCR1 = ms;
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
    RCC->APB1ENR |= RCC_APB1Periph_TIM4;

    TIM4->PSC = 72000>>TIMER_MS_BIT;//0x8ca;
    TIM4->ARR = TIMER_ARR;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->DIER= TIM_DIER_CC1IE;
    TIM4->CCR1 = TIMER_ARR/2;
    TIM4->CR1= TIM_CR1_CEN; //向上计数,循环计算器
}

/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void TIM4_IRQHandler(void)
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
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    //串口1初始化(调试输出串口 115200 N81)
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);


    xprintf_init_com(gs_buf_fmt, sizeof(gs_buf_fmt), gs_buf_send_irq, sizeof(gs_buf_send_irq));
}

/***********************************************************
 * 功能描述：中断 串口0
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  ms时间
 ***********************************************************/
void USART1_IRQHandler(void)
{
    char val;

    USART_ClearITPendingBit(USART1, USART_IT_TC);

    if (xprintf_get_char(&val))
    {
        USART1->DR = val;
    }
    else
    {
        CLEAR_BIT(USART1->CR1, USART_CR1_TXEIE);
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
    while((USART1->SR&0X40)==0);

    USART1->DR = val;
}

/***********************************************************
 * 功能描述：有数据需要中断发送通知
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf_msg_put_irq(void)
{
    if (READ_BIT(USART1->CR1, USART_CR1_TXEIE) == 0) //中断关闭(发送一个数据唤醒)
    {
        SET_BIT(USART1->CR1, USART_CR1_TXEIE);
    }
}



