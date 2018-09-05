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


static char gs_buf_fmt[200];          //打印格式化缓冲区(一次最大的内容)
static char gs_buf_send_irq[2000];    //打印中断输出缓冲区(最大缓存的内容)


#define TICK_TIME_MS  (1000*9)
#define TICK_TIME_LOAD (1000*TICK_TIME_MS - 1)    //重载时间

/***********************************************************
 * 功能描述：毫秒定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tim_ms_init(void)
{
//    NVIC_InitTypeDef    NVIC_InitStructure;
//
//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;  //先占优先级0级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xF;  //从优先级3级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

    TIM4->CR1 = 0;
    RCC->APB1ENR |= RCC_APB1Periph_TIM4;    //时钟使能
    TIM4->PSC = (72000/2)/32;               //时钟分频
    TIM4->DIER = TIM_DIER_UIE;              //允许更新中断
}


/***********************************************************
 * 功能描述：系统设置定时器溢出时间 (外部调用需要实现)
 * 输入参数：ms 定时时间,如果传入的ms超过定时器最大值 设置未定时器最大值
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:限制最大ms为1000(保证定时器1s能调用一次)
 ***********************************************************/
void tnos_tim_ms_set(u32 ms)
{
    u32 delay;

    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //只有上下溢出才能产生中断
    TIM4->SR = 0;
    TIM4->EGR |= TIM_EGR_UG; //重新初始化计数器，并产生一个更新事件

    delay = ms*2*32;  //2*32 保证ms的分辨率( 2的N次幂方便计算)

    if (delay < 8)
    {
        delay = 8;
    }
    else if (delay > 2*32*1000)
    {
        delay = 2*32*1000;
    }

    TIM4->ARR = delay;
    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_CEN;
}


/***********************************************************
 * 功能描述：定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_init(void)
{
    SysTick->CTRL = 0;        //停止计数器、禁止中断
    SysTick->LOAD = TICK_TIME_LOAD; //8分频  9tick 一个us
    SysTick->VAL = TICK_TIME_LOAD;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;     //启动计数 8分频
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
 * 功能描述：获取时间(没有加锁)
 * 输入参数：无
 * 输出参数：  ptick 获取的时间,不允许传输NULL
 * 返 回 值：  无
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register s32 s = g_tick.s;
    register u32 val = SysTick->VAL; //必须先获取val,再判断是否溢出

    while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) //防止已经超时,最近又到末尾时间查看,继续超时
    {
        ++s;
        val = SysTick->VAL; //防止刚判断后超时导致时间回退
    }

    val = (s32)TICK_TIME_LOAD - val;

    if (val > 0)
    {
        val /= TICK_TIME_MS;
    }
    else
    {
        val = 0;
    }

    g_tick.s = s;
    g_tick.ms = val;

    if (ptick != NULL)
    {
        ptick->s = s;
        ptick->ms = val;
    }
}


/***********************************************************
 * 功能描述：延迟us时间
 * 输入参数：us 等待us时间
 * 输出参数：  无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_delay_us(u32 us)
{
    u32 val_old = SysTick->VAL;

    if (us != 0)
    {
        if (us < 200*1000U) //100ms
        {
            u32 tick = (us*(TICK_TIME_MS/1000)) - 2;

            while (1)
            {
                u32 val_now = SysTick->VAL;

                if (val_now <= val_old) //计数器为向下计数类型
                {
                    if ((val_old - val_now) >= tick)
                    {
                        return;
                    }
                }
                else
                {
                    if ((TICK_TIME_LOAD - val_now + val_old) >= tick)
                    {
                        return;
                    }
                }
            }
        }
        else
        {
            ttimer_delay_ms(us/1000);
        }
    }
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
//    NVIC_InitTypeDef    NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xF;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

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

#if 0 //测试定时器

static time_tick_t s_tick;

/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void TIM4_IRQHandler(void)
{
    if(TIM4->SR)//溢出中断
    {
        TIM4->SR = 0;
        u32 pass = pass_ticks_now(&s_tick, NULL);

        DBG("A[%u]", pass);
    }
}


void set_tick_timeout(u32 ms)
{
    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //只有上下溢出才能产生中断
    TIM4->SR = 0;
    TIM4->EGR |= TIM_EGR_UG; //重新初始化计数器，并产生一个更新事件

    u32 delay = ms*2*32;

    if (delay < 8)
    {
        delay = 8;
    }
    else if (delay > 65535)
    {
        delay = 65535;
    }

    TIM4->ARR = delay;
    DBG("set[%u]", ms);
    get_time_tick(&s_tick);
    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_CEN;

}

//通过重新设定定时器的值判断是否能多次更新定时器
void test(void)
{
    board_init();

    tnos_tim_ms_init();

    u32 ms, ms_tmr;

    DBG("s");

    ms = 10;
    ms_tmr = 10;
    set_tick_timeout(ms_tmr);
    ttimer_delay_ms(ms);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);


    ms = 10;
    ms_tmr = 3;
    set_tick_timeout(ms + 2);
    ttimer_delay_ms(ms);
    set_tick_timeout(ms_tmr);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);


    ms = 10;
    ms_tmr = 3;
    set_tick_timeout(ms + 1);
    ttimer_delay_ms(ms);
    set_tick_timeout(ms + 2);
    ttimer_delay_ms(ms);
    set_tick_timeout(ms + 3);
    ttimer_delay_ms(ms);
    set_tick_timeout(ms + 4);
    ttimer_delay_ms(ms);
    set_tick_timeout(ms_tmr);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);

}

#endif

