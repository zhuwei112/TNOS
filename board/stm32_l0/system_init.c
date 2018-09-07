/***********************************************************
 * ��Ȩ��Ϣ:
 * �ļ�����: system_init.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����:
 * �汾��ʷ:
 ***********************************************************/
#include "stm32l0xx_hal.h"
#include "system_init.h"
#include "xprintf.h"
#include "tnos_dev.h"


/***********************************************************
 * ����������ϵͳʱ�ӳ�ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void sys_clock_init(void)//32M
{
    int timeout;

    LL_RCC_PLL_Disable();   //�ر�PLL

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

#if 1 //�ڲ�16M����

    LL_RCC_HSI_Enable();  //ʹ���ڲ�ʱ��

    LL_RCC_HSI_DisableDivider();

    timeout = 50000;

    while(timeout--)  //�ȴ��ڲ�ʱ���ȶ�
    {
        if(LL_RCC_HSI_IsReady())
        {
            break;
        }
    }

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_2);  //����PLLʱ�ӣ�Ϊ32MHZ

#else //�ⲿ8M����

    LL_RCC_HSE_Enable();   //ʹ���ⲿʱ��

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

    LL_RCC_PLL_Enable();  //ʹ��PLLʱ��

    timeout = 50000;

    while (timeout--)
    {
        if(LL_RCC_PLL_IsReady())  //�ȴ�PLLʱ���ȶ�
        {
            break;
        }
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);  //����ϵͳʱ��ԴΪPLLʱ��

    timeout = 50000;

    while (timeout--)
    {
        if (LL_RCC_SYS_CLKSOURCE_STATUS_PLL == LL_RCC_GetSysClkSource())
        {
            break;
        }
    };

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);  //AHB��Ƶ��Ϊ1
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);   //APB1��Ƶ��Ϊ1
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);   //APB2��Ƶ��Ϊ1

    // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
    LL_SetSystemCoreClock(32000000);

//    LL_RCC_ConfigMCO(RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_8);  //����ʱ�����
}

/***********************************************************
 * �����������ж����ȼ���ʼ��
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void nvic_config(void)
{
    NVIC_SetPriority(USART4_5_IRQn, 1);
    NVIC_SetPriority(TIM3_IRQn, 2);
}
//
///// �ض���c�⺯��printf��USART1
//int fputc(int ch, FILE *f)
//{
//    /* ����һ���ֽ����ݵ�USART1 */
//
//    USART1->DR = ch & 0xFF;
//
//    /* �ȴ�������� */
//    while (!(USART1->SR & USART_FLAG_TXE))
//    {
//    }
//
//    return (ch);
//}
//

/***********************************************************
 * �������ƣ�uart_config
 * ����������uart_config
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void uart_config(void)
{

}

/***********************************************************
 * �������ƣ�spi_config
 * ����������spi_config
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void spi_config(void)
{

}


/***********************************************************
 * �������ƣ�timer_config
 * ����������timer_config
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void timer_config(void)
{
//	/* Time base configuration */
//	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
//
//
//	TIM_TimeBaseStructure.TIM_Period = 18000 + 400;//����ΪTIM_Period+1	  //��ʱ1msʱ������
//	TIM_TimeBaseStructure.TIM_Prescaler = 3;//��Ƶ��λTIM_Prescaler+1
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
 * �������ƣ�io_config
 * ����������io_config
 * �����������
 * �����������
 * �� �� ֵ��  ��
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
//	GPIO_InitStructure.GPIO_Pin = PB0|PB1|PB5;//PB0:5100�ж�,PB1ң��
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
 * ��������������Ӳ��ϵͳʱ�Ӻ����ȼ���ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 * ע��:Ϊ�˺����ʼ����ӡ�Ͷ�ʱ����׼��
 ***********************************************************/
void tnos_board_sys_pro_init(void)
{
    sys_clock_init();

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);  //��GPIOA��ʱ��
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); //��GPIOB��ʱ��
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);//��GPIOC��ʱ��
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);//��GPIOD��ʱ��
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);//��GPIOE��ʱ��
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);//��GPIOH��ʱ��

    nvic_config();
}


/***********************************************************
 * ��������������Ӳ��������ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_board_other_init(void)
{
	io_config();

  	uart_config();

	timer_config();
}

