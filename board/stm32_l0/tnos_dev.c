/***********************************************************
 * ��Ȩ��Ϣ:
 * �ļ�����: tnos_dev.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: cpu���
 * �汾��ʷ:
 ***********************************************************/

#include "stm32l0xx_hal.h"
#include "tnos.h"
#include "tnos_dev.h"
#include "xprintf.h"
#include "xtimer.h"


static char gs_buf_fmt[200];          //��ӡ��ʽ��������(һ����������)
static char gs_buf_send_irq[2000];    //��ӡ�ж����������(��󻺴������)


#define TICK_TIME_MS  (1000*4)
#define TICK_TIME_LOAD (1000*TICK_TIME_MS - 1)    //����ʱ��
#define TIME_MS_PSC    (32)                       //��Ϊms�ķ�Ƶ,����Ϊ2��n����

/***********************************************************
 * �������������붨ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_tim_ms_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    TIM3->CR1 = 0;
    TIM3->PSC = (16000)/TIME_MS_PSC;       //ʱ�ӷ�Ƶ
    TIM3->DIER = TIM_DIER_UIE;              //��������ж�
    NVIC_EnableIRQ(TIM3_IRQn);
}


/***********************************************************
 * ����������ϵͳ���ö�ʱ�����ʱ�� (�ⲿ������Ҫʵ��)
 * ���������ms ��ʱʱ��,��������ms������ʱ�����ֵ ����δ��ʱ�����ֵ
 * ��������� ��
 * �� �� ֵ��  ��
 * ע��:�������msΪ1000(��֤��ʱ��1s�ܵ���һ��)
 ***********************************************************/
void tnos_tim_ms_set(u32 ms)
{
    u32 delay;

    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //ֻ������������ܲ����ж�
    TIM3->SR = 0;
    TIM3->EGR |= TIM_EGR_UG; //���³�ʼ����������������һ�������¼�

    delay = ms*TIME_MS_PSC;  //��֤ms�ķֱ���( 2��N���ݷ������)

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
 * ����������ϵͳ��ʼ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void TIM3_IRQHandler(void)
{
    tnos_tick_proess();
}
#endif


/***********************************************************
 * ������������ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void ttimer_init(void)
{
    SysTick->CTRL = 0;        //ֹͣ����������ֹ�ж�
    SysTick->LOAD = TICK_TIME_LOAD; //8��Ƶ  9tick һ��us
    SysTick->VAL = TICK_TIME_LOAD;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;     //�������� 8��Ƶ
}

/***********************************************************
 * ������������ȡʱ��(û�м���)
 * �����������
 * ���������  ptick ��ȡ��ʱ��,��������NULL
 * �� �� ֵ��  ��
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register s32 s = g_tick.s;
    register u32 val = SysTick->VAL; //�����Ȼ�ȡval,���ж��Ƿ����

    while (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) //��ֹ�Ѿ���ʱ,����ֵ�ĩβʱ��鿴,������ʱ
    {
        ++s;
        val = SysTick->VAL; //��ֹ���жϺ�ʱ����ʱ�����
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
 * �����������ӳ�usʱ��
 * ���������us �ȴ�usʱ��
 * ���������  ��
 * �� �� ֵ��  ��
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

                if (val_now <= val_old) //������Ϊ���¼�������
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
 * ������������ӡ��ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
void xprintf_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    UART_HandleTypeDef UartHandle = {0};

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4); //�򿪴���4ʱ��

    //PC10 PC11������ΪUSART4
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
 * �����������ж� ����0
 * �����������
 * ��������� ��
 * �� �� ֵ��  msʱ��
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
 * ������������ͨ��ʽ��ӡ����ַ�(��Ҫ�Լ�ʵ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf_put_char(char val)
{
    while((USART4->ISR&0X40)==0);

    USART4->TDR = val;
}

/***********************************************************
 * ������������������Ҫ�жϷ���֪ͨ
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf_msg_put_irq(void)
{
    if (READ_BIT(USART4->CR1, USART_CR1_TXEIE) == 0) //�жϹر�(����һ�����ݻ���)
    {
        SET_BIT(USART4->CR1, USART_CR1_TXEIE);
    }
}

#if 0 //���Զ�ʱ��

static time_tick_t s_tick;
static u32 volatile gs_delay_ms = 0;
static u32 volatile gs_pass = 0;

/***********************************************************
 * ����������ϵͳ��ʼ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void TIM3_IRQHandler(void)
{
    if(TIM3->SR)//����ж�
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
    TIM3->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //ֻ������������ܲ����ж�
    TIM3->SR = 0;
    TIM3->EGR |= TIM_EGR_UG; //���³�ʼ����������������һ�������¼�

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

//ͨ�������趨��ʱ����ֵ�ж��Ƿ��ܶ�θ��¶�ʱ��
void test(void)
{
    tnos_board_sys_pro_init();

    xprintf_init();

    ttimer_init(); //��ʱ����ʼ��

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

