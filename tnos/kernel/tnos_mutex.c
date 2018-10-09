/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_mutex.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ ������
 * �汾��ʷ:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_MUTEX_ENABLE != 0)


/***********************************************************
 * ���������������� ��ʼ��
 * ���������pmutex ������
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_mutex_init(tnos_mutex_t *pmutex)
{
    TNOS_ASSERT(pmutex != NULL);

    irq_disable();
    memset(pmutex, 0, sizeof(tnos_mutex_t));
    tnos_singal_init(&pmutex->singal, 0);
    irq_enable();

    return TNOS_ERR_NONE;
}

/***********************************************************
 * ���������������� �������� (֧����Ƕ��,�ɶ����,�жϲ��ܵ���)
 * ���������pmutex ������
 *          timeout_ms ��ʱʱ��
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_mutex_trylock(tnos_mutex_t *pmutex, u32 timeout_ms)
{
    u32 i;

    TNOS_ASSERT(pmutex != NULL);

    for (i = 0; i < 2; i++)
    {
        irq_disable();
        tnos_singal_cnt_clean(&pmutex->singal);

        if ((pmutex->lock_cnt == 0) || (pmutex->ptcb_send == gs_ptnos_tcb_cur)) //û�������ߵ�ǰ��ǰ��������
        {
            ++pmutex->lock_cnt;
            pmutex->ptcb_send = gs_ptnos_tcb_cur;
            irq_enable();

            return TNOS_ERR_NONE;
        }

        if (i == 0)
        {
            tnos_singal_wait_ms(&pmutex->singal, timeout_ms);
        }
        irq_enable();
    }

    return TNOS_ERR_TIMEOUT;
}

/***********************************************************
 * ���������������� ���� (֧����Ƕ��,�ɶ����,�жϲ��ܵ���)
 * ���������pmutex ������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_mutex_lock(tnos_mutex_t *pmutex)
{
    TNOS_ASSERT(pmutex != NULL);

    while (tnos_mutex_trylock(pmutex, 1000) != TNOS_ERR_NONE)
    {
    }
}


/***********************************************************
 * ���������������� ���� (֧����Ƕ��,�жϲ��ܵ���)
 * ���������pmutex ������
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_mutex_unlock(tnos_mutex_t *pmutex)
{
    TNOS_ASSERT(pmutex != NULL);

    irq_disable();
    if (pmutex->lock_cnt > 0) //����һ���˳�
    {
        --pmutex->lock_cnt;
    }

    tnos_singal_send(&pmutex->singal);
    irq_enable();
}

#endif



