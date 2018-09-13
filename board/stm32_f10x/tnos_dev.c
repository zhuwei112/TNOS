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

/******************
 * TIM34��ʱ��˵��
 *  �Զ�������ttimer��ȡ��ǰʱ�书��, �Ƚ���tnos�ӳٹ���
 ********************/


#define TIMER_MS_BIT 1 //ms�ķֱ���

//��ʱ�����¼��ص�ֵ
//��ʱ������ʱ������Ϊ2��,���� ���ʱ�Ͷ�ʱ��1sһ�ο���
#define TIMER_ARR_S (2) //���ص�����(�������������Ҫ�޸�)
#define TIMER_ARR ((TIMER_ARR_S)*1000*BIT(TIMER_MS_BIT) - 1)

#if (TIMER_ARR_S <= 1)
    #error TIMER_ARR_S mster > 1
#endif

static char gs_buf_fmt[200];          //��ӡ��ʽ��������(һ����������)
static char gs_buf_send_irq[2000];    //��ӡ�ж����������(��󻺴������)



/***********************************************************
 * �������������붨ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_tim_ms_init(void)
{
}


/***********************************************************
 * ������������ȡʱ��(û�м���)
 * �����������
 * ���������  ptick ��ȡ��ʱ��
 * �� �� ֵ��  ��
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register u32 s = (((u32)g_tick.s)>>1)<<1; //&0xFFFFFFFE
    register u32 ms = TIM4->CNT>>TIMER_MS_BIT;

    if (ms < g_tick.ms) //��ʱ�����
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
 * ����������ϵͳ���ö�ʱ�����ʱ�� (�ⲿ������Ҫʵ��)
 * ���������ms ��ʱʱ��,��������ms������ʱ�����ֵ ����δ��ʱ�����ֵ
 * ��������� ��
 * �� �� ֵ��  ��
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
 * ������������ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void ttimer_init(void)
{
    RCC->APB1ENR |= RCC_APB1Periph_TIM4;

    TIM4->PSC = 72000>>TIMER_MS_BIT;//0x8ca;
    TIM4->ARR = TIMER_ARR;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->DIER= TIM_DIER_CC1IE;
    TIM4->CCR1 = TIMER_ARR/2;
    TIM4->CR1= TIM_CR1_CEN; //���ϼ���,ѭ��������
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
 * ������������ӡ��ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
void xprintf_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

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



