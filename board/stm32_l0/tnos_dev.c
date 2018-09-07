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


static char gs_buf_fmt[200];          //打印格式化缓冲区(一次最大的内容)
static char gs_buf_send_irq[2000];    //打印中断输出缓冲区(最大缓存的内容)


#define TICK_TIME_MS  (1000*4)
#define TICK_TIME_LOAD (1000*TICK_TIME_MS - 1)    //重载时间
#define TIME_MS_PSC    (32)                       //变为ms的分频,必须为2的n次幂

/***********************************************************
 * 功能描述：毫秒定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tim_ms_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    TIM3->CR1 = 0;
    TIM3->PSC = (16000)/TIME_MS_PSC;       //时钟分频
    TIM3->DIER = TIM_DIER_UIE;              //允许更新中断
    NVIC_EnableIRQ(TIM3_IRQn);
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

    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //只有上下溢出才能产生中断
    TIM3->SR = 0;
    TIM3->EGR |= TIM_EGR_UG; //重新初始化计数器，并产生一个更新事件

    delay = ms*TIME_MS_PSC;  //保证ms的分辨率( 2的N次幂方便计算)

    if (delay < 8)
    {
        delay = 8;
    }
    else if (delay > TIME_MS_PSC*1000)
    {
        delay = TIME_MS_PSC*1000;
    }

    TIM3->ARR = delay;
    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_CEN;
}

#if 1
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
#endif


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

#if 0 //测试定时器

static time_tick_t s_tick;
static u32 volatile gs_delay_ms = 0;
static u32 volatile gs_pass = 0;

/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void TIM3_IRQHandler(void)
{
    if(TIM3->SR)//溢出中断
    {
        TIM3->SR = 0;
        u32 pass = pass_ticks_now(&s_tick, NULL);

        if ((gs_delay_ms != 0) && (gs_delay_ms == pass))
        {
            DBG("ok[%u]", pass);
            gs_pass = 1;
        }
        else
        {
            DBG("err, need[%u], use[%u]", gs_delay_ms, pass);
        }
    }
}


void set_tick_timeout(u32 ms)
{
    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //只有上下溢出才能产生中断
    TIM3->SR = 0;
    TIM3->EGR |= TIM_EGR_UG; //重新初始化计数器，并产生一个更新事件

    u32 delay = ms*2*32;

    if (delay < 8)
    {
        delay = 8;
    }
    else if (delay > 65535)
    {
        delay = 65535;
    }

    TIM3->ARR = delay;
    DBG("set[%u]", ms);
    get_time_tick(&s_tick);
    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_CEN;

}

//通过重新设定定时器的值判断是否能多次更新定时器
void test(void)
{
    tnos_board_sys_pro_init();

    xprintf_init();

    ttimer_init(); //定时器初始化

    tnos_tim_ms_init();

    u32 ms, ms_tmr;

    DBG("s");

//    while (1)
//    {
//        DBG("s");
//        ttimer_delay_ms(1000);
//    }

    ms = 10;
    ms_tmr = 10;
    gs_delay_ms = gs_pass = ms_tmr;
    set_tick_timeout(gs_delay_ms);
    ttimer_delay_ms(ms);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);
    if (!gs_pass)
    {
        while (1)
        {
            DBG("ERR!");
        }
    }

    gs_delay_ms = gs_pass = 0;
    ms = 10;
    ms_tmr = 3;
    set_tick_timeout(ms + 2);
    ttimer_delay_ms(ms);
    gs_delay_ms = ms_tmr;
    set_tick_timeout(gs_delay_ms);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);
    if (!gs_pass)
    {
        while (1)
        {
            DBG("ERR!");
        }
    }

    gs_delay_ms = gs_pass = 0;
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
    gs_delay_ms = ms_tmr;
    set_tick_timeout(ms_tmr);
    DBG("ms[%u],ms_tmr[%u]", ms, ms_tmr);
    ttimer_delay_ms(500);
    if (!gs_pass)
    {
        while (1)
        {
            DBG("ERR!");
        }
    }

    while (1)
    {
    }
}

#endif

