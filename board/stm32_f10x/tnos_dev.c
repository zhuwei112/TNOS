/***********************************************************
 * ��Ȩ��Ϣ:
 * �ļ�����: tnos_dev.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: cpu���
 * �汾��ʷ:
 ***********************************************************/

#include "stm32f10x.h"
#include "tnos.h"
#include "tnos_dev.h"
#include "xprintf.h"
#include "xtimer.h"


static char gs_buf_fmt[200];          //��ӡ��ʽ��������(һ����������)
static char gs_buf_send_irq[2000];    //��ӡ�ж����������(��󻺴������)


#define TICK_TIME_MS  (1000*9)
#define TICK_TIME_LOAD (1000*TICK_TIME_MS - 1)    //����ʱ��
#define TIME_MS_PSC    (64)                       //��Ϊms�ķ�Ƶ,����Ϊ2��n����

/***********************************************************
 * �������������붨ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_tim_ms_init(void)
{
//    NVIC_InitTypeDef    NVIC_InitStructure;
//
//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;  //��ռ���ȼ�0��
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xF;  //�����ȼ�3��
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

    TIM4->CR1 = 0;
    RCC->APB1ENR |= RCC_APB1Periph_TIM4;    //ʱ��ʹ��
    TIM4->PSC =  72000/TIME_MS_PSC;         //ʱ�ӷ�Ƶ
    TIM4->DIER = TIM_DIER_UIE;              //��������ж�
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

    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //ֻ������������ܲ����ж�
    TIM4->SR = 0;
    TIM4->EGR |= TIM_EGR_UG; //���³�ʼ����������������һ�������¼�

    delay = ms*TIME_MS_PSC;  //��֤ms�ķֱ���( 2��N���ݷ������)

    if (delay < 8)
    {
        delay = 8;
    }
    else if (delay > TIME_MS_PSC*1000)
    {
        delay = TIME_MS_PSC*1000;
    }

    TIM4->ARR = delay;
    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM | TIM_CR1_CEN;
}


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
 * ����������ϵͳ��ʼ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void TIM4_IRQHandler(void)
{
    tnos_tick_proess();
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
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
//    NVIC_InitTypeDef    NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xF;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    //����1��ʼ��(����������� 115200 N81)
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
 * �����������ж� ����0
 * �����������
 * ��������� ��
 * �� �� ֵ��  msʱ��
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
 * ������������ͨ��ʽ��ӡ����ַ�(��Ҫ�Լ�ʵ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf_put_char(char val)
{
    while((USART1->SR&0X40)==0);

    USART1->DR = val;
}

/***********************************************************
 * ������������������Ҫ�жϷ���֪ͨ
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf_msg_put_irq(void)
{
    if (READ_BIT(USART1->CR1, USART_CR1_TXEIE) == 0) //�жϹر�(����һ�����ݻ���)
    {
        SET_BIT(USART1->CR1, USART_CR1_TXEIE);
    }
}


#if 0 //���Զ�ʱ��

static time_tick_t s_tick;

/***********************************************************
 * ����������ϵͳ��ʼ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void TIM4_IRQHandler(void)
{
    if(TIM4->SR)//����ж�
    {
        TIM4->SR = 0;
        u32 pass = pass_ticks_now(&s_tick, NULL);

        DBG("A[%u]", pass);
    }
}


void set_tick_timeout(u32 ms)
{
    TIM4->CR1 = TIM_CR1_URS | TIM_CR1_OPM; //ֻ������������ܲ����ж�
    TIM4->SR = 0;
    TIM4->EGR |= TIM_EGR_UG; //���³�ʼ����������������һ�������¼�

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

//ͨ�������趨��ʱ����ֵ�ж��Ƿ��ܶ�θ��¶�ʱ��
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

