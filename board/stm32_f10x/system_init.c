/***********************************************************
 * 版权信息:
 * 文件名称: system_init.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能:
 * 版本历史:
 ***********************************************************/
#include "stm32f10x.h"
#include "core_cm3.h"
#include "system_init.h"
#include "xprintf.h"
#include "tnos_dev.h"

/***********************************************************
 * 功能描述：中断优先级初始化
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void nvic_config(void)
{
 	NVIC_InitTypeDef	NVIC_InitStructure;
//  最低抢占给 调度器用
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//2位抢占 2位顺序 不需要抢占优先级

 	/* 串口优先级最低 */
 	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);

	//TIM4优先级最低
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


}
//
///// 重定向c库函数printf到USART1
//int fputc(int ch, FILE *f)
//{
//    /* 发送一个字节数据到USART1 */
//
//    USART1->DR = ch & 0xFF;
//
//    /* 等待发送完毕 */
//    while (!(USART1->SR & USART_FLAG_TXE))
//    {
//    }
//
//    return (ch);
//}
//

/***********************************************************
 * 函数名称：uart_config
 * 功能描述：uart_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void uart_config(void)
{

}

/***********************************************************
 * 函数名称：spi_config
 * 功能描述：spi_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
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
//	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//双向
//	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;//主
//	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;//8位
//	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;//时钟线平时
//	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;//第一边沿采集
//	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;//软件控制片选
//	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;//分频 256
//	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//先高位、再低位
//	SPI_InitStructure.SPI_CRCPolynomial=7;//CRC计算
//
//	SPI_Init(SPI2,&SPI_InitStructure);
//	SPI_Cmd(SPI2,ENABLE);					//Enable  SPI1
}


/***********************************************************
 * 函数名称：timer_config
 * 功能描述：timer_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void timer_config(void)
{
//	/* Time base configuration */
//	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
//
//
//	TIM_TimeBaseStructure.TIM_Period = 18000 + 400;//计数为TIM_Period+1	  //延时1ms时的设置
//	TIM_TimeBaseStructure.TIM_Prescaler = 3;//分频数位TIM_Prescaler+1
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
 * 函数名称：io_config
 * 功能描述：io_config
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
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
//	GPIO_InitStructure.GPIO_Pin = PB0|PB1|PB5;//PB0:5100中断,PB1遥控
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
 * 功能描述：板子硬件系统时钟和优先级初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:为了后面初始化打印和定时器做准备
 ***********************************************************/
void tnos_board_sys_pro_init(void)
{
    nvic_config();
}


/***********************************************************
 * 功能描述：板子硬件其它初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
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

