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

#if (TNOS_MSGQ_ENABLE != 0)


//ʹ�õ�posΪ����pos - 1
//��λ��תΪ��ַ
#define POS2ADDR(pos_next)   ((tnos_msgq_data_t *)(pmsgq->pbuf + (((pos_next) - 1)*(pmsgq->size + sizeof(tnos_msgq_data_t)))))

//����ַתΪλ��
#define ADDR2POS(addr)       ((((u32)(addr) - (u32)pmsgq->pbuf)/(pmsgq->size + sizeof(tnos_msgq_data_t))) + 1)

/***********************************************************
 * ���������������������
 * ��������� pmsgq ��Ϣ����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
static void do_msgq_clean(tnos_msgq_t *pmsgq)
{
    pmsgq->pos_empty = pmsgq->pos_head = pmsgq->pos_tail = 0;
    tnos_singal_clear_cnt(&pmsgq->singal);

    if (pmsgq->size != 0)
    {
        u32 i;

        for (i = 1; i <= pmsgq->num_max; i++) //λ�ô�1��ʼ
        {
            tnos_msgq_data_t *pdata = POS2ADDR(i);

            pdata->pos_next = pmsgq->pos_empty;
            pmsgq->pos_empty = i;
        }
    }
}


/***********************************************************
 * �������� ��Ϣ���� ��ʼ��
 * ���������pmsgq    ��Ϣ����
 *          pbuf      ��������ʼλ��
 *          buf_size  ��������С
 *          msg_size  һ����Ϣ����С
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 * ע��:��Ϣ�ĸ���Ϊ (((buf_size+3)/4)*4)/(sizeof(tnos_msgq_data_t) + ((msg_size +3/4)*4)))
 ***********************************************************/
s32 tnos_msgq_init(tnos_msgq_t *pmsgq, u32 msg_size, void *pbuf, u32 buf_size)
{
    u32 addr, less;

    TNOS_ASSERT((pmsgq != NULL) && (pbuf != NULL) && (buf_size > (sizeof(tnos_msgq_data_t) + msg_size)) && (msg_size != 0));

    irq_disable();
    memset(pmsgq, 0, sizeof(tnos_msgq_t));

    addr = ALIGN_UP((u32)pbuf, 4);
    less = addr - (u32)pbuf;

    if (less >= buf_size)
    {
        irq_enable();

        return TNOS_ERR_ARG;
    }

    if (less != 0) //��ʼ��ַ4�ֽڶ���
    {
        pbuf = (u8 *)pbuf + less;
        buf_size -= less;
    }

    msg_size = ALIGN_UP(msg_size, 4);
    pmsgq->num_max = buf_size/(msg_size + sizeof(tnos_msgq_data_t));

    if ((pmsgq->num_max == 0) || (pmsgq->num_max >= 0xFFFF))
    {
        irq_enable();

        return TNOS_ERR_ARG;
    }

    pmsgq->size = msg_size;
    pmsgq->pbuf = (u8*)pbuf;
    tnos_singal_init(&pmsgq->singal, 0);
    do_msgq_clean(pmsgq);
    irq_enable();

    return TNOS_ERR_NONE;
}

/***********************************************************
 * ���������������������
 * ��������� pmsgq ��Ϣ����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
void tnos_msgq_clean(tnos_msgq_t *pmsgq)
{
    irq_disable();
    do_msgq_clean(pmsgq);
    irq_enable();
}

/***********************************************************
 * ������������Ϣ���� ��������ָ���ȡ (��2:��ȡ����ָ��)
 * ���������pmsgq ��Ϣ����
 * ��������� pmsgq_data ��������ָ��
 * �� �� ֵ�� 0:�ɹ�  ����:ʧ��
 * ע��: (���� == 0ʱ,һ��Ҫ���� tnos_msgq_send_ptr_next ,�������й¶)
 *       д����ʱ:���Ȳ��ܳ���msg_size ������б���
 ***********************************************************/
s32 tnos_msgq_send_ptr(tnos_msgq_t *pmsgq, tnos_msgq_data_t **pmsgq_data)
{
    u16 pos;
    tnos_msgq_data_t *pdata;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL));

    *pmsgq_data = NULL;

    if (pmsgq->size == 0) //û�г�ʼ��
    {
        return TNOS_ERR_OTHER;
    }

    irq_disable();
    if (pmsgq->pos_empty == 0)
    {
        irq_enable();
        return TNOS_ERR_FULL;
    }

    pos = pmsgq->pos_empty;
    pdata = POS2ADDR(pos);
    pmsgq->pos_empty = pdata->pos_next;
    irq_enable();

    *pmsgq_data = pdata;
    return TNOS_ERR_NONE;
}

/***********************************************************
* ������������Ϣ���з���ָ�� ��������(��2:����ָ�뷢������)
* ���������pmsgq ��Ϣ����
*          ppdata ��������ָ��
*          data_len �������ݵĳ���
*          is_first �Ƿ�(�Ƚ����)
* ��������� ��
* �� �� ֵ��  0:�ɹ� ����:����
***********************************************************/
s32 tnos_msgq_send_ptr_next(tnos_msgq_t *pmsgq, tnos_msgq_data_t *pmsgq_data, u32 data_len, BOOL is_first)
{
    u16 pos;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    pos = ADDR2POS(pmsgq_data);

    irq_disable();
    if (data_len > pmsgq->size) //�������ͳ��ȳ���������
    {
        pmsgq_data->pos_next = pmsgq->pos_empty;
        pmsgq->pos_empty = pos;
        irq_enable();

        return TNOS_ERR_ARG;
    }

    pmsgq_data->len = data_len;

    if (pmsgq_data->len == 0) //����0��������,�ŵ��ն�����
    {
        pmsgq_data->pos_next = pmsgq->pos_empty;
        pmsgq->pos_empty = pos;
    }
    else //�����е�����һ���г���
    {
        if (!is_first) //�Ƚ��ȳ�
        {
            //��β����������
            if (pmsgq->pos_tail != 0)
            {
                tnos_msgq_data_t *pdata_next = POS2ADDR(pmsgq->pos_tail);

                pdata_next->pos_next = pos;
            }

            pmsgq->pos_tail = pos;
            pmsgq_data->pos_next = NULL;

            if (pmsgq->pos_head == 0)
            {
                pmsgq->pos_head = pos;
            }
        }
        else //�Ƚ����
        { //��ͷ������
            pmsgq_data->pos_next = pmsgq->pos_head;
            pmsgq->pos_head = pos;

            if (pmsgq->pos_tail == 0)
            {
                pmsgq->pos_tail = pos;
            }
        }

        tnos_singal_send(&pmsgq->singal);
    }
    irq_enable();

    return TNOS_ERR_NONE;
}



/***********************************************************
 * ������������Ϣ���� ��������
 * ���������pmsgq ��Ϣ����
 *          pbuf   �������ݻ�����
 *          len    �������ݳ���
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ� ����:ʧ��
 ***********************************************************/
static s32 do_msgq_send(tnos_msgq_t *pmsgq, const void *pbuf, u32 len, BOOL is_first)
{
    TNOS_ASSERT((pmsgq != NULL) && (pbuf != NULL));

    if (len > pmsgq->size)
    {
        return TNOS_ERR_ARG;
    }

    if (len != 0)
    {
        tnos_msgq_data_t *pmsgq_data;
        s32 ret = tnos_msgq_send_ptr(pmsgq, &pmsgq_data);

        if (ret != 0)
        {
            return ret;
        }

        memcpy(TNOS_MSGQ_GET_BUF(void *, pmsgq_data), pbuf, len);
        return tnos_msgq_send_ptr_next(pmsgq, pmsgq_data, len, is_first);
    }

    return TNOS_ERR_NONE;
}

/***********************************************************
 * ������������Ϣ���� ��������(�Ƚ��ȳ�)
 * ���������pmsgq ��Ϣ����
 *          pbuf   �������ݻ�����
 *          len    �������ݳ���
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_msgq_send(tnos_msgq_t *pmsgq, const void *pbuf, u32 len)
{
    return do_msgq_send(pmsgq, pbuf, len, FALSE);
}

/***********************************************************
 * ������������Ϣ���� ��������(�Ƚ����)
 * ���������pmsgq ��Ϣ����
 *          pbuf   �������ݻ�����
 *          len    �������ݳ���
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_msgq_send_frist(tnos_msgq_t *pmsgq, const void *pbuf, u32 len)
{
    return do_msgq_send(pmsgq, pbuf, len, TRUE);
}

/***********************************************************
 * ������������Ϣ���� ��������ָ���ȡ (��2:��ȡָ��)
 * ���������pmsgq ��Ϣ����
 *          timeout_ms ��ʱʱ��
 * ��������� pmsgq_data ��������ָ��
 * �� �� ֵ��  �������ݵĳ���, ��������0 ʧ��
 * ע��: (����> 0ʱ,һ��Ҫ�ͷ� tnos_msgq_rev_ptr_next)
 *       ���ص�������ʼ����  TNOS_MSGQ_GET_BUF��ȡ
 ***********************************************************/
s32 tnos_msgq_rev_ptr(tnos_msgq_t *pmsgq, tnos_msgq_data_t **pmsgq_data, u32 timeout_ms)
{
    tnos_msgq_data_t *pdata;
    u16 pos;
    s32 num;
    u32 len;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    *pmsgq_data = NULL;

    if (pmsgq->size == 0) //û�г�ʼ��
    {
        return TNOS_ERR_OTHER;
    }

    irq_disable();
    num = tnos_singal_wait_ms(&pmsgq->singal, timeout_ms);

    if (num <= 0)
    {
        irq_enable();

        return TNOS_ERR_TIMEOUT;
    }

    pos = pmsgq->pos_head;

    if (pos == 0) //����BUG,�Ż���
    {
        irq_enable();

        return TNOS_ERR_BUG;
    }

    pdata = POS2ADDR(pos);

    if (pmsgq->pos_tail == pos) //���һ������
    {
        pmsgq->pos_head = pmsgq->pos_tail = 0;
    }
    else
    {
        pmsgq->pos_head = pdata->pos_next;
    }
    irq_enable();

    *pmsgq_data = pdata;

    len = (*pmsgq_data)->len;

    if ((len == 0) || (len > pmsgq->size)) //������һ�������ݳ����Ҳ��������󳤶�
    {
        return TNOS_ERR_BUG;
    }

    return len;
}

/***********************************************************
* ������������Ϣ���� ��������ָ���ͷ�(��2:�ͷŽ�������)
* ���������pmsgq ��Ϣ����
*          pmsgq_data ��������ָ��
* ��������� ��
* �� �� ֵ��  ��
***********************************************************/
void tnos_msgq_rev_ptr_next(tnos_msgq_t *pmsgq, tnos_msgq_data_t *pmsgq_data)
{
    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    irq_disable();
    //���뵽�ն�����
    pmsgq_data->pos_next = pmsgq->pos_empty;
    pmsgq->pos_empty = ADDR2POS(pmsgq_data);
    irq_enable();
}

/***********************************************************
 * ������������Ϣ���� ��������
 * ���������pmsgq ��Ϣ����
 *          buf_len  �������ݵĳ���
 *          timeout_ms ��ʱʱ��
 * ��������� pbuf �������ݻ�����
 * �� �� ֵ��  �������ݵĳ���, ������0ʧ��
 ***********************************************************/
s32 tnos_msgq_rev(tnos_msgq_t *pmsgq, void *pbuf, u32 buf_len, u32 timeout_ms)
{
    tnos_msgq_data_t *pdata;
    s32 len;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq->size != 0) && (pbuf != NULL) && (buf_len != 0));

    len = tnos_msgq_rev_ptr(pmsgq, &pdata, timeout_ms);

    if (len <= 0)
    {
        return len;
    }

    if ((u32)len > buf_len)
    {
        len = buf_len;
    }

    memcpy(pbuf, TNOS_MSGQ_GET_BUF(void *, pdata), len);
    tnos_msgq_rev_ptr_next(pmsgq, pdata);

    return len;
}


#endif
