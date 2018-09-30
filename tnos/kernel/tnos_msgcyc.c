/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_cycbuf.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ ѭ��������
 * �汾��ʷ:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_MSGCYC_ENABLE != 0)


/***********************************************************
 * ������������Ϣѭ���� �������
 * ���������pmsgc    ��Ϣѭ����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
static void do_msgcyc_clean(tnos_msgcyc_t *pmsgc)
{
    pmsgc->pos_w = pmsgc->pos_r = 0;
    tnos_singal_clean(&pmsgc->singal);
}

/***********************************************************
 * �������� ��Ϣѭ���� ��ʼ��
 * ���������pmsgc    ��Ϣѭ����
 *          pbuf      ��������ʼλ��
 *          buf_size  ��������С
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 *          �����Ϣ��СΪ buf_size - 4 - 1
 ***********************************************************/
s32 tnos_msgcyc_init(tnos_msgcyc_t *pmsgc, void *pbuf, u32 buf_size)
{
    u32 addr, less;

    TNOS_ASSERT((pmsgc != NULL) && (pbuf != NULL)
                && (buf_size > (sizeof(tnos_msgcyc_data_t) + 4 + 4)));

    irq_disable();
    memset(pmsgc, 0, sizeof(tnos_msgcyc_t));
    addr = ALIGN_UP((u32)pbuf, 4);
    less = addr - (u32)pbuf;

    if (less + 8 >= buf_size)
    {
        irq_enable();

        return TNOS_ERR_ARG;
    }

    if (less != 0) //��ʼ��ַ4�ֽڶ���
    {
        pbuf = (u8 *)pbuf + less;
        buf_size -= less;
    }

    pmsgc->pbuf = (u8*)pbuf;
    pmsgc->buf_size = ALIGN_DOWN(buf_size, 4);
    tnos_singal_init(&pmsgc->singal, 0);
    do_msgcyc_clean(pmsgc);
    irq_enable();

    return TNOS_ERR_NONE;
}

/***********************************************************
 * ������������Ϣѭ���� �������
 * ���������pmsgc    ��Ϣѭ����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
void tnos_msgcyc_clean(tnos_msgcyc_t *pmsgc)
{
    irq_disable();
    do_msgcyc_clean(pmsgc);
    irq_enable();
}

/***********************************************************
 * ������������Ϣѭ���� ��ȡ�յĴ�С
 * ���������pmsgc    ��Ϣѭ����
 * ��������� ��
 * �� �� ֵ���յĴ�С
 ***********************************************************/
u32 tnos_msgcyc_get_empty_num(tnos_msgcyc_t *pmsgc)
{
    s32 less;
    s32 space_bk, space_end;

    less = 0;
    irq_disable();
    if (pmsgc->buf_size != 0)
    {
        if (pmsgc->pos_w >= pmsgc->pos_r)
        {
            space_bk = (s32)(pmsgc->buf_size - pmsgc->pos_w) - (s32)sizeof(tnos_msgcyc_data_t) - 4;
            space_end = pmsgc->pos_r - (s32)sizeof(tnos_msgcyc_data_t) - 4;
            less = MAX(space_bk, space_end);

            if (less < 0)
            {
                less = 0;
            }
        }
        else
        {
            space_bk = (s32)(pmsgc->pos_r - pmsgc->pos_w) - (s32)sizeof(tnos_msgcyc_data_t) - 4;
            less = MAX(space_bk, 0);
        }
    }
    irq_enable();

    return (u32)less;
}

/***********************************************************
 * ������������Ϣѭ���� ��������(�Ƚ��ȳ�)
 * ���������pmsgc    ��Ϣѭ����
 *          pbuf   �������ݻ�����
 *          len    �������ݳ���
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ� ����:ʧ�� TNOS_ERR_LESS:�ռ䲻��
 ***********************************************************/
s32 tnos_msgcyc_send(tnos_msgcyc_t *pmsgc, const void *pbuf, u32 len)
{
    u32 len_space;
    tnos_msgcyc_data_t *pd;

    TNOS_ASSERT((pmsgc != NULL) && ((pbuf != NULL) && (len != 0)));

    if (len == 0)
    {
        return 0;
    }

    if (pmsgc->buf_size == 0) //û�г�ʼ��
    {
        return TNOS_ERR_OTHER;
    }

    len_space = ALIGN_UP(len + sizeof(tnos_msgcyc_data_t), 4);

    irq_disable();
    if (pmsgc->pos_w >= pmsgc->pos_r)
    {
        u32 less = pmsgc->buf_size - pmsgc->pos_w;

        if (less < len_space)
        {
            if (pmsgc->pos_r < len_space + 4) //4��ֹ��д��ͬ
            {
                irq_enable();
                return TNOS_ERR_LESS;
            }

            pd = (tnos_msgcyc_data_t *)&(pmsgc->pbuf[pmsgc->pos_w]);
            pd->len = 0;              //ʣ�಻����λ�ö���
            pd->pos_next = 0;
        }
        else if ((less == len_space) && (pmsgc->pos_r == 0))
        {
            irq_enable();
            return TNOS_ERR_LESS;
        }
    }
    else
    {
        u32 less = pmsgc->pos_r - pmsgc->pos_w;

        if (less < len_space + 4) //�����и���λ
        {
            irq_enable();
            return TNOS_ERR_LESS;
        }
    }

    pd = (tnos_msgcyc_data_t *)&(pmsgc->pbuf[pmsgc->pos_w]);
    memcpy(pd + 1, pbuf, len);
    pd->len = len;
    pd->pos_next += len_space;

    if (pd->pos_next >= pmsgc->buf_size)
    {
        pd->pos_next = 0;
    }

    pmsgc->pos_w = pd->pos_next;
    tnos_singal_send(&pmsgc->singal);
    irq_enable();

    return 0;
}


/***********************************************************
 * ������������Ϣѭ���� ��������ָ���ȡ (��2:��ȡָ��)
 * ���������pmsgqc��Ϣѭ����
 *          timeout_ms ��ʱʱ��
 * ��������� pbuf      ���պ������λ��(һ��Ϊ4�ֽڶ���)
 * �� �� ֵ��  �������ݵĳ���, ��������0 ʧ��
 * ע��: (����> 0ʱ,һ��Ҫ�ͷ� tnos_msgq_rev_ptr_next)
 ***********************************************************/
s32 do_tnos_msgq_rev_ptr(tnos_msgcyc_t *pmsgc, u8 **pbuf, u32 timeout_ms)
{
    s32 num;

    TNOS_ASSERT((pmsgc != NULL) && (pmsgc != NULL))

    *pbuf = NULL;

    if (pmsgc->buf_size == 0) //û�г�ʼ��
    {
        return TNOS_ERR_OTHER;
    }

    num = tnos_singal_wait_ms(&pmsgc->singal, timeout_ms);

    if (num <= 0)
    {
        return TNOS_ERR_TIMEOUT;
    }

    while (1)
    {
        tnos_msgcyc_data_t *pd;

        if (pmsgc->pos_r == pmsgc->pos_w)
        {
            pmsgc->pos_r = pmsgc->pos_w = 0; //�Ƶ��ʼ,����д����ռ�
            break;
        }

        pd = (tnos_msgcyc_data_t *)&(pmsgc->pbuf[pmsgc->pos_r]);

        if (pd->len != 0)
        {
            *pbuf = (u8 *)(&pd[1]);

            return (u32)pd->len;
        }

        pmsgc->pos_r = pd->pos_next;
    }

    return 0;
}

/***********************************************************
 * ������������Ϣѭ���� ��������ָ���ȡ (��2:��ȡָ��)
 * ���������pmsgqc��Ϣѭ����
 *          timeout_ms ��ʱʱ��
 * ��������� pbuf      ���պ������λ��(һ��Ϊ4�ֽڶ���)
 * �� �� ֵ��  �������ݵĳ���, ��������0 ʧ��
 * ע��: (����> 0ʱ,һ��Ҫ�ͷ� tnos_msgcyc_rev_ptr_next)
 ***********************************************************/
s32 tnos_msgcyc_rev_ptr(tnos_msgcyc_t *pmsgc, u8 **pbuf, u32 timeout_ms)
{
    s32 ret;

    irq_disable();
    ret = do_tnos_msgq_rev_ptr(pmsgc, pbuf, timeout_ms);
    irq_enable();

    return ret;
}

/***********************************************************
* ������������Ϣѭ���� ��������ָ���ͷ�(��2:�ͷŽ���ָ��)
* ���������pmsgq ��Ϣ����
*          ppdata ��������ָ��
* ��������� ��
* �� �� ֵ��  ��
***********************************************************/
void do_tnos_msgcyc_rev_ptr_next(tnos_msgcyc_t *pmsgc)
{
    BOOL is_twince = FALSE;

    while (1) //ȥ��д����յ�λ��
    {
        tnos_msgcyc_data_t *pd;

        if (pmsgc->pos_r == pmsgc->pos_w)
        {
            pmsgc->pos_r = pmsgc->pos_w = 0; //�Ƶ��ʼ,����д����ռ�
            break;
        }

        pd = (tnos_msgcyc_data_t *)&(pmsgc->pbuf[pmsgc->pos_r]);

        if ((!is_twince) && (pd->len != 0))
        {
            break;
        }

        pmsgc->pos_r = pd->pos_next;
        is_twince = TRUE;
    }
}

/***********************************************************
* ������������Ϣѭ���� ��������ָ���ͷ�(��2:�ͷŽ���ָ��)
* ���������pmsgq ��Ϣ����
*          ppdata ��������ָ��
* ��������� ��
* �� �� ֵ��  ��
***********************************************************/
void tnos_msgcyc_rev_ptr_next(tnos_msgcyc_t *pmsgc)
{
    irq_disable();
    do_tnos_msgcyc_rev_ptr_next(pmsgc);
    irq_enable();
}


/***********************************************************
 * ������������Ϣѭ������������
 * ���������pmsgc    ��Ϣѭ����
 *          buf_len  �������ݵĳ���
 *          timeout_ms ��ʱʱ��
 * ��������� pbuf �������ݻ�����
 * �� �� ֵ��  �������ݵĳ���,  ������0ʧ��
 ***********************************************************/
s32 tnos_msgcyc_rev(tnos_msgcyc_t *pmsgc, void *pbuf, u32 buf_len, u32 timeout_ms)
{
    s32 len;
    u8 *pbuf_r;

    irq_disable();
    len = do_tnos_msgq_rev_ptr(pmsgc, &pbuf_r, timeout_ms);

    if (len > 0)
    {
        if ((u32)len > buf_len)
        {
            len = (s32)buf_len;
        }

        memcpy(pbuf, pbuf_r, len);
        do_tnos_msgcyc_rev_ptr_next(pmsgc);
    }
    irq_enable();

    return len;
}


#endif
