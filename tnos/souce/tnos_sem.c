/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_sem.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ �ź���
 * �汾��ʷ:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_SEM_ENABLE != 0)


/***********************************************************
 * �����������ź�����ʼ
 * ���������psem �ź���
 *           cnt ��ʼ����ֵ
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_sem_init(tnos_sem_t *psem, u32 cnt)
{
    u32 reg;

    TNOS_ASSERT(psem != NULL);

    reg = irq_disable();
    tnos_singal_init(&psem->singal, cnt);
    irq_enable(reg);

    return TNOS_ERR_NONE;
}


/***********************************************************
 * �����������ź��� ����
 * ���������psem �ź���
 *           cnt ��ʼ����ֵ
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_sem_post(tnos_sem_t *psem)
{
    u32 reg;

    TNOS_ASSERT(psem != NULL);

    reg = irq_disable();
    tnos_singal_send(&psem->singal);
    irq_enable(reg);
}


/***********************************************************
 * �����������ź����ȴ�(�ж��в���������!!)
 * ���������psingal �źŽṹ��
 *          timeout_ms ��ʱmsʱ��
 * ��������� ��
 * �� �� ֵ��  ��ǰ�ź�������, > 0 ��, ����ʧ��
 ***********************************************************/
s32 tnos_sem_wait(tnos_sem_t *psem, u32 timeout_ms)
{
    s32 cnt;
    u32 reg;

    TNOS_ASSERT(psem != NULL);

    reg = irq_disable();
    cnt = tnos_singal_wait_ms(&psem->singal, timeout_ms, &reg);
    irq_enable(reg);

    return cnt;
}


/***********************************************************
 * �����������ź����ȴ�(�ж��в���������!!)
 * ���������psingal �źŽṹ��
 * ��������� ����ȴ�����
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_sem_clean(tnos_sem_t *psem)
{
    u32 reg;

    reg = irq_disable();
    tnos_singal_clear_cnt(&psem->singal);
    irq_enable(reg);
}


#endif