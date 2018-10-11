/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_msgq.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ ��Ϣ����
 * �汾��ʷ:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_CYC_ENABLE != 0)
/***********************************************************
 * �������� ѭ�������� ��ʼ��
 * ���������pthis    ��Ϣѭ����
 *          pbuf      ��������ʼλ��
 *          buf_size  ��������С
 * ��������� ��
�� �� ֵ����
 ***********************************************************/
void tnos_cyc_init(tnos_cyc_t *pthis, void *pbuf, u32 buf_size)
{
    TNOS_ASSERT((pbuf != NULL) && (buf_size > 2));

    irq_disable();
    tnos_singal_init(&pthis->singal, 0);
    pthis->pbuf = (u8 *)pbuf;
    pthis->size = buf_size;
    pthis->r_pos = pthis->w_pos = 0;
    irq_enable();
}

/***********************************************************
 * �������� ѭ�������� ���
 * ���������pthis    ��Ϣѭ����
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_cyc_clean(tnos_cyc_t *pthis)
{
    irq_disable();
    pthis->r_pos = pthis->w_pos = 0;
    tnos_singal_cnt_clean(&pthis->singal);
    irq_enable();
}

/***********************************************************
 * �������� ѭ��������ʣ���С
 * ���������pthis    ��Ϣѭ����
 * ��������� ��
 * �� �� ֵ��  ʣ����ô�С
 ***********************************************************/
u32 tnos_cyc_less(tnos_cyc_t *pthis)
{
    u32 cnt;

    irq_disable();
    if (pthis->w_pos < pthis->r_pos)
    {
        cnt = pthis->r_pos - pthis->w_pos - 1;
    }
    else
    {
        cnt = pthis->size - pthis->w_pos + pthis->r_pos - 1;
    }
    irq_enable();
    return cnt;
}

/***********************************************************
 * ����������ѭ�������� �Ѿ�д��ĸ���
 * ��������� pthis  ����
 * ��������� ��
 * �� �� ֵ�� ʣ����ô�С
 ***********************************************************/
u32 tnos_cyc_have(tnos_cyc_t *pthis)
{
    u32 cnt;

    irq_disable();
    if (pthis->w_pos >= pthis->r_pos)
    {
        cnt = pthis->w_pos - pthis->r_pos;
    }
    else
    {
        cnt = pthis->size - pthis->r_pos + pthis->w_pos;
    }
    irq_enable();

    return cnt;
}

/***********************************************************
 * ����������ѭ�������� д������
 * ��������� pthis  ����
 *           pbuf   д���λ��
 *           len    д�볤��
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ�  ����:ʧ�� (ʣ��ռ䲻������д��)
 ***********************************************************/
s32 tnos_cyc_send(tnos_cyc_t *pthis, const void *pbuf, u32 len)
{
    TNOS_ASSERT((pbuf != NULL) && (len != 0));

    irq_disable();
    if (tnos_cyc_less(pthis) < len)
    {
        irq_enable();

        return -1;
    }

    if (len != 0)
    {
        u32 less, w_size;

        if (pthis->w_pos < pthis->r_pos)
        {
            memcpy(pthis->pbuf + pthis->w_pos, pbuf, len);
            pthis->w_pos += len;
        }
        else
        {
            less = pthis->size - pthis->w_pos;
            w_size = MIN(less, len);
            memcpy(pthis->pbuf + pthis->w_pos, pbuf, w_size);
            pthis->w_pos += w_size;

            if (pthis->w_pos >= pthis->size)
            {
                pthis->w_pos = 0;
            }

            if (w_size < len)
            {
                len -= w_size;
                memcpy(pthis->pbuf, (const u8 *)pbuf + w_size, len);
                pthis->w_pos = len;
            }
        }

        tnos_singal_send(&pthis->singal);
    }
    irq_enable();

    return 0;
}

/***********************************************************
 * ����������ѭ�������� ��ȡ(ָ�벻�ƶ� mv���ƶ�)
 * ��������� pthis  ����
 *           pbuf   ����������ʼ
 *           len    ���ĳ���
 *           timeout_ms ��ʱʱ��
 * ��������� ��
 * �� �� ֵ����ȡ�ĳ���
 ***********************************************************/
u32 tnos_cyc_rev(tnos_cyc_t *pthis, void *pbuf, u32 len, u32 timeout_ms)
{
    u32 len_have;

    TNOS_ASSERT((pbuf != NULL) && (len != 0));

    irq_disable();
    len_have = tnos_cyc_have(pthis);

    if ((len_have == 0) && (timeout_ms != 0))
    {
        tnos_singal_wait_ms(&pthis->singal, timeout_ms);
        len_have = tnos_cyc_have(pthis);

        if (len_have != 0)
        {
            tnos_singal_cnt_add(&pthis->singal);
        }
    }

    if (len > len_have)
    {
        len = len_have;
    }

    if (len != 0)
    {
        if (pthis->r_pos <= pthis->w_pos)
        {
            memcpy(pbuf, pthis->pbuf + pthis->r_pos, len);
        }
        else
        {
            u32 less = pthis->size - pthis->r_pos;
            u32 r_size = MIN(less, len);

            memcpy(pbuf, pthis->pbuf + pthis->r_pos, r_size);

            if (r_size < len)
            {
                memcpy((u8 *)pbuf + r_size, pthis->pbuf, len - r_size);
            }
        }

        if (len == len_have)
        {
            tnos_cyc_clean(pthis);
        }
        else
        {
            u32 pos = (u32)pthis->r_pos + len;

            if (pos >= pthis->size)
            {
                pos -= pthis->size;
            }

            pthis->r_pos = pos;
        }
    }
    irq_enable();

    return len;
}

/***********************************************************
 * ����������ѭ�������� ��ȡ(ָ�벻�ƶ� mv���ƶ�)
 * ��������� pthis  ����
 * ��������� pbf �����ݵ�λ��
 * �� �� ֵ����
 ***********************************************************/
void tnos_cyc_rev_ptr(tnos_cyc_t *pthis, tnos_cyc_ptr_t *pbf, u32 timeout_ms)
{
    CLEAN_SIZE(pbf, sizeof(tnos_cyc_ptr_t));

    irq_disable();
    if (pthis->r_pos == pthis->w_pos)
    {
        if (timeout_ms == 0)
        {
            goto rev_exit;
        }

        tnos_singal_wait_ms(&pthis->singal, timeout_ms);

        if (pthis->r_pos == pthis->w_pos)
        {
            goto rev_exit;
        }

        tnos_singal_cnt_add(&pthis->singal);
    }

    pbf->pdata_1 = pthis->pbuf + pthis->r_pos;

    if (pthis->r_pos < pthis->w_pos) //ֻ��һ����
    {
        pbf->len_data_1 = pthis->w_pos - pthis->r_pos;
        pbf->len_data_2 = 0;
    }
    else //����2����
    {
        pbf->len_data_1 = pthis->size - pthis->r_pos;
        pbf->len_data_2 = pthis->w_pos;
        pbf->pdata_2 = pthis->pbuf;
    }

  rev_exit:
    irq_enable();
}


/***********************************************************
 * ����������ѭ�������� д������
 * ��������� pthis  ����
 *           len    д�볤��
 * ��������� ��
 * �� �� ֵ���Ƿ�Ϊ��
 ***********************************************************/
BOOL tnos_cyc_rev_mv(tnos_cyc_t *pthis, u32 len)
{
    irq_disable();
    if (len != 0)
    {
        if (len >= tnos_cyc_have(pthis))
        {
            tnos_cyc_clean(pthis);
            irq_enable();

            return TRUE;
        }

        pthis->r_pos += len;

        if (pthis->r_pos >= pthis->size)
        {
            pthis->r_pos -= pthis->size;
        }
    }
    irq_enable();

    return FALSE;
}


#endif
