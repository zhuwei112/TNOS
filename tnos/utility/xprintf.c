/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: xprintf.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ����
 * �汾��ʷ:
 ***********************************************************/

#include "xutils.h"
#include "xprintf.h"
#include "xtimer.h"
#include "tnos_dev.h"

static printf_ctrl_t  gs_printf_ctrl;


/***********************************************************
 * ������������ȡһ����ӡ�������
 * ���������wu
 * ��������� pval
 * �� �� ֵ��  FALSE: û������, ����:��
 ***********************************************************/
BOOL xprintf_get_char(char *pval)
{
    char *ppos = (char *)gs_printf_ctrl.pr;

    if (ppos != gs_printf_ctrl.pw)
    {
        *pval = *ppos++;
        gs_printf_ctrl.pr = ((u32)ppos < (u32)gs_printf_ctrl.pend) ? ppos : gs_printf_ctrl.pst;

        return TRUE;
    }

    return FALSE;
}

/***********************************************************
 * ������������ӡ���
 * ���������pfmt ��ʽ���ַ���
 *          arg   �ɱ����
 *          use_irq �Ƿ��ж����
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xvprintf(const char *pfmt, va_list va, BOOL use_irq)
{
    u32 reg;
    int len;

    reg = irq_disable();
    len = xvsnprintf(gs_printf_ctrl.pbuf_fmt, gs_printf_ctrl.pbuf_fmt_len - 3, pfmt, va);

    if (len > 0)
    {
        gs_printf_ctrl.pbuf_fmt[len++] = '\r';
        gs_printf_ctrl.pbuf_fmt[len++] = '\n';
        gs_printf_ctrl.pbuf_fmt[len] = 0;

        if (use_irq)
        {
            const char *prd = gs_printf_ctrl.pbuf_fmt;

            while (1)
            {
                char *pnext;
                char c = *prd++;

                if (c == 0)
                {
                    break;
                }

                pnext = (char *)gs_printf_ctrl.pw;

                if ((u32)(++pnext) >= (u32)gs_printf_ctrl.pend)
                {
                    pnext = gs_printf_ctrl.pst;
                }

                if (pnext != gs_printf_ctrl.pr)
                {
                    *gs_printf_ctrl.pw = c;
                    gs_printf_ctrl.pw = pnext;
                }
                else //���治��
                {
                }
            }

            xprintf_msg_put_irq();
        }
        else
        {
            while (1)
            {
                char c = *gs_printf_ctrl.pbuf_fmt++;

                if (c == 0)
                {
                    break;
                }

                xprintf_put_char(c);
            }
        }
    }

    irq_enable(reg);
}

/***********************************************************
 * ������������ӡ�������
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf(const char *pfmt, ...)
{
    va_list va;

	va_start(va, pfmt);
	xvprintf(pfmt, va, TRUE);
    va_end(va);
}

/***********************************************************
 * ������������ӡ�������(��ͨ��ʽ����)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void xprintf_w(const char *pfmt, ...)
{
    va_list va;

    va_start(va, pfmt);
    xvprintf(pfmt, va, FALSE);
    va_end(va);
}

/***********************************************************
 * ������������ӡ��ʼ��
 * ���������pbuf_fmt     ��ʽ��������(һ�������󳤶�)
 *          fmt_len      ��ʽ���������������
 *          pbuf_send    ���ͻ����� (��󻺴����ݳ���)
 *          pbuf_send_len ���ͻ���������
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************/
void xprintf_init_com(char *pbuf_fmt, u16 fmt_len, char *pbuf_send, u16 pbuf_send_len)
{
    gs_printf_ctrl.pbuf_fmt = pbuf_fmt;
    gs_printf_ctrl.pbuf_fmt_len = fmt_len;
    gs_printf_ctrl.pst = pbuf_send;
    gs_printf_ctrl.pend = gs_printf_ctrl.pst + pbuf_send_len;
    gs_printf_ctrl.pw = gs_printf_ctrl.pr = gs_printf_ctrl.pst;
}



