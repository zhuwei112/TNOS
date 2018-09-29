/***********************************************************
 * ��Ȩ��Ϣ:
 * �ļ�����: main.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����:
 * �汾��ʷ:
 ***********************************************************/
#include "stm32f10x.h"
#include "core_cm3.h"
#include "xutils.h"
#include "xtimer.h"
#include "xprintf.h"
#include "system_init.h"
#include "tnos.h"

/***********************************************************
 * ����������
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
void main_test(void)
{
	DBG("1234");

	while (1)
	{


	}
}

#if 0

#include "tnos_cpu_c.h"

uint32_t set_r6(u32 val)
{
 register uint32_t __regAPSR          __ASM("r6");
 __regAPSR = val;
}

void te1(void)
{
    u32 i = 0;

     irq_disable();
    for (i = 0; i < 100; i++); {};

    for (i = 0; i < 100; i++); {};
    irq_enable() ;
}

/***********************************************************
 * �������ƣ�task_main
 * ����������������
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void task_main(void *p_arg)
{

    {
        u32 i = 0;

       irq_disable();
        OS_TASK_SW();
        for (i = 0; i < 100; i++); {};

        te1();

        for (i = 0; i < 100; i++); {};
        irq_enable() ;

    }

    while (1)
    {
        u32 a0, a1, a2, a3, a4, a5, a6, a7, a8 ,b0;

        a7 = 0x10101010;
        a6 = 0x20202020;
        a5 = 0x30303030;
        a4 = 0x40404040;

        a3 = 0x50505050;
        a2 = 0x60606060;
        a1 = 0x70707070;
        a0 = 0x80808080;

        u32 msp;
        msp = __get_MSP();

        set_r6(0x12345678);

        DBG("A[0X%08X\r\n", msp);

        tnos_sched();
        msp = __get_MSP();
        DBG("B[0X%08X]", msp);
        b0 = 1;
        ++a0;
        ++a1;
        ++a2;
        ++a3;
        ++a4;
        ++a5;
        ++a6;
        ++a7;
        ++a8;

        ttimer_delay_ms(1000);
    }
}

/***********************************************************
 * �������ƣ�task_main
 * ����������������
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
static void task_m(void *p_arg)
{
    while (1)
    {
        u32 a0, a1, a2, a3, a4, a5, a6, a7, a8 ,b0;

        a0 = 0x10101010;
        a1 = 0x20202020;
        a2 = 0x30303030;
        a3 = 0x40404040;

        a4 = 0x50505050;
        a5 = 0x60606060;
        a6 = 0x70707070;
        a7 = 0x80808080;

        u32 msp;
        msp = __get_MSP();
        DBG("a[0X%08X\r\n", msp);
        tnos_sched();
        msp = __get_MSP();
        DBG("b[0X%08X]", msp);
        b0 = 1;
        ++a0;
        ++a1;
        ++a2;
        ++a3;
        ++a4;
        ++a5;
        ++a6;
        ++a7;
        ++a8;
        ttimer_delay_ms(1000);
    }
}

#endif

void tnos_idle_hook(void)
{
}


//
//
//
///***********************************************************
// * ����������ϵͳ��ʼ����
// * �����������
// * ��������� ��
// * �� �� ֵ��  ��
// ***********************************************************/
//void TIM4_IRQHandler(void)
//{
//	static u32 cnt = 500;
//	TIM4->SR = 0;//����жϱ�־λ
////	TIM4->CR1 = 0;
//
//	if (cnt != 500)
//	{
//		cnt = 500;
//	}
//	else
//	{
//		cnt = 1000;
//	}
//
//	tnos_set_tick_timeout(cnt);
//	DBG("A");
//}
//
//

static volatile s32 gs_cnt = 0;



void irq_ds(void)
{

    __disable_irq();
    ++gs_cnt;
}


void irq_en(void)
{
    if (--gs_cnt == 0)
    {
        __enable_irq();
    }
}


#if 0

void test(void)
{
    DBG("test");
    tnos_delay_ms(1000);
    irq_ds();
    DBG("1");
    tnos_delay_ms(1000);
    irq_ds();
    DBG("2");
    tnos_delay_ms(1000);
    irq_ds();

    DBG("3");
    tnos_delay_ms(1000);
    irq_en();

    DBG("2");
    tnos_delay_ms(1000);
    irq_en();
    DBG("1");
    tnos_delay_ms(1000);
    irq_en();
    DBG("0");
    tnos_delay_ms(1000);
}
#endif


/***********************************************************
 * ����������
 * �����������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
int main(void)
{

    tnos_startup();

//    test();
//
//    while (1)
//    {
//    }

    return 0;
}

