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

/******************
 * TIM3 ��ʱ��˵��
 *  �Զ�������ttimer��ȡ��ǰʱ�书��, �Ƚ���tnos�ӳٹ���
 ********************/

#define TIMER_MS_BIT 0 //ms�ķֱ���


//��ʱ�����¼��ص�ֵ
//��ʱ������ʱ������Ϊ2��,���� ���ʱ�Ͷ�ʱ��1sһ�ο���
#define TIMER_ARR_S (2)    //���ص�����,����������������޸�
#define TIMER_ARR          ((TIMER_ARR_S)*1000*BIT(TIMER_MS_BIT) - 1) //��ʱ������ֵ



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
 * ���������  ptick ��ȡ��ʱ��,��������NULL
 * �� �� ֵ��  ��
 ***********************************************************/
void get_time_tick2(time_tick_t *ptick)
{
    register u32 s = (((u32)g_tick.s)>>1)<<1; //&0xFFFFFFFE
    register u32 ms = TIM3->CNT>>TIMER_MS_BIT;

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
 * ������������ʱ����ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void ttimer_init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    TIM3->PSC = 32000>>TIMER_MS_BIT;
    TIM3->ARR = TIMER_ARR;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->DIER = TIM_DIER_CC1IE;
    TIM3->CCR1 = TIMER_ARR/2;
    TIM3->CR1 = TIM_CR1_CEN; //���ϼ���,ѭ��������

    NVIC_EnableIRQ(TIM3_IRQn);
}

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
