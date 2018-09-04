/***********************************************************
 * ��Ȩ��Ϣ:
 * �ļ�����: system_init.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����:
 * �汾��ʷ:
 ***********************************************************/
#include "stm32f10x.h"
#include "core_cm3.h"
#include "system_init.h"
#include "xprintf.h"
#include "tnos_dev.h"

/***********************************************************
 * �����������ж����ȼ���ʼ��
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void nvic_config(void)
{
 	NVIC_InitTypeDef	NVIC_InitStructure;
//  �����ռ�� ��������
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//2λ��ռ 2λ˳�� ����Ҫ��ռ���ȼ�

 	/* �������ȼ���� */
 	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);

	//TIM4���ȼ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


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
//	GPIO_InitTypeDef 		GPIO_InitStructure;
//	SPI_InitTypeDef   	SPI_InitStructure;
//
//	/* Set Chip Select pin */
//	GPIO_InitStructure.GPIO_Pin=PA4;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	GPIO_SetBits(GPIOA, PA4);
//
//
//	GPIO_InitStructure.GPIO_Pin = PA5|PA6|PA7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//
//
//	/* Set SPI interface */
//	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
//	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
//	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
//	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
//	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
//	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
////	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
//	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
//	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
//	SPI_InitStructure.SPI_CRCPolynomial=7;
//
//	SPI_Init(SPI1,&SPI_InitStructure);
//
//	SPI_Cmd(SPI1,ENABLE);					//Enable  SPI1
//
//
//	GPIO_InitStructure.GPIO_Pin = PB13 | PB14 | PB15;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin=PB12;
//	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB, PB12);
//
//
//	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//˫��
//	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//��
//	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//8λ
//	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;//ʱ����ƽʱ
//	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;//��һ���زɼ�
//	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//�������Ƭѡ
//	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;//��Ƶ 256
//	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//�ȸ�λ���ٵ�λ
//	SPI_InitStructure.SPI_CRCPolynomial=7;//CRC����
//
//	SPI_Init(SPI2,&SPI_InitStructure);
//	SPI_Cmd(SPI2,ENABLE);					//Enable  SPI1
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                           | RCC_APB2Periph_GPIOC
                           | RCC_APB2Periph_SPI1
                           | RCC_APB2Periph_AFIO, ENABLE);

	io_config();

  	uart_config();

	timer_config();
}

