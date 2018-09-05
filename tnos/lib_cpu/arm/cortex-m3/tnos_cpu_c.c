/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_cpu_c.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: cpu���
 * �汾��ʷ:
 ***********************************************************/
#include "tnos_core.h"
#include "xprintf.h"

#define TNOS_STK_FULL  0xFECB1DC7   //����������

#ifndef  D_NVIC_INT_CTRL
#define  D_NVIC_INT_CTRL      *((u32 *)0xE000ED04)
#endif

#ifndef  D_NVIC_PENDSVSET
#define  D_NVIC_PENDSVSET     0x10000000
#endif


/***********************************************************
 * ������������ʼ�������ջ
 * ���������ptask      ������
 *          p_arg      �������
 *          pstk_addr ��ջ�ĵ�ַ
 * ��������� ��
 * �� �� ֵ��  �����һ�����õĵ�ַ
 ***********************************************************/
u32 * tnos_task_stk_init(ptnos_task ptask, void *p_arg, u32 *pstk_addr, u32 stk_size)
{
    u32 *p_stk;
    u32 i;
    u32 end = stk_size - 16;

    for (i = 0; i < end; i++)
    {
        pstk_addr[i] = TNOS_STK_FULL;
    }

    for (; i < stk_size; i++)
    {
        pstk_addr[i] = 0;
    }

    p_stk = &pstk_addr[end];
    p_stk[8] = (u32)p_arg;                  //R8  argument
    p_stk[13] = (u32)tnos_task_return;      //R13 LR
    p_stk[14] = (u32)ptask;                 //R14 Entry Point
    p_stk[15] = (u32)0x01000000u;           //R15 xPSR

    return p_stk;
}

/***********************************************************
 * ���������� �ѵĸ������
 * ��������� pstk_addr ��ջ�ĵ�ַ
 *           stk_size  �ѵĸ���
 * ��������� ��
 * �� �� ֵ�� ʣ����Ŀ
 ***********************************************************/
u32 tnos_taks_stk_check(u32 *pstk_addr, u32 stk_size)
{
    u32 i;

    if (stk_size <= 16)
    {
        return 0;
    }


    for (i = 0 ; i < stk_size - 16; i++)
    {
        if (pstk_addr[i] != TNOS_STK_FULL)
        {
            break;
        }
    }

    return i;
}

/***********************************************************
 * ���������������л�
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_task_sw(void)
{
    D_NVIC_INT_CTRL = D_NVIC_PENDSVSET; //����pendsvc
}




typedef struct
{
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;

    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r12;
    u32 lr;
    u32 pc;
    u32 psr;
}stack_reg_t;

typedef struct
{
    u32         exc_ret;
    stack_reg_t reg;
}exception_info_t;

/***********************************************************
 * �����������쳣����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void hardfault_handler_proc(exception_info_t * pex)
{
    DBGW("PSR: 0x%08x", pex->reg.psr);
    DBGW(" R0: 0x%08x", pex->reg.r0);
    DBGW(" R1: 0x%08x", pex->reg.r1);
    DBGW(" R2: 0x%08x", pex->reg.r2);
    DBGW(" R3: 0x%08x", pex->reg.r3);
    DBGW(" R4: 0x%08x", pex->reg.r4);
    DBGW(" R5: 0x%08x", pex->reg.r5);
    DBGW(" R6: 0x%08x", pex->reg.r6);
    DBGW(" R7: 0x%08x", pex->reg.r7);
    DBGW(" R8: 0x%08x", pex->reg.r8);
    DBGW(" R9: 0x%08x", pex->reg.r9);
    DBGW("R10: 0x%08x", pex->reg.r10);
    DBGW("R11: 0x%08x", pex->reg.r11);
    DBGW("R12: 0x%08x", pex->reg.r12);
    DBGW(" LR: 0x%08x", pex->reg.lr);
    DBGW(" PC: 0x%08x", pex->reg.pc);

    while (1)
    {
        ;
    }
}
