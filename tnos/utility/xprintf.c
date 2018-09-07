/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: xprintf.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 定义
 * 版本历史:
 ***********************************************************/

#include "xutils.h"
#include "xprintf.h"
#include "xtimer.h"
#include "tnos_dev.h"

static printf_ctrl_t  gs_printf_ctrl;


/***********************************************************
 * 功能描述：获取一个打印输出内容
 * 输入参数：wu
 * 输出参数： pval
 * 返 回 值：  FALSE: 没有内容, 其它:有
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
 * 功能描述：打印输出
 * 输入参数：pfmt 格式化字符串
 *          arg   可变参数
 *          use_irq 是否中断输出
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xvprintf(const char *pfmt, va_list va, BOOL use_irq)
{
    int len;

    irq_disable();

    len = xvsnprintf(gs_printf_ctrl.pbuf_fmt, gs_printf_ctrl.pbuf_fmt_len - 3, pfmt, va);

    if (len > 0)
    {
		const char *prd;
		
        gs_printf_ctrl.pbuf_fmt[len++] = '\r';
        gs_printf_ctrl.pbuf_fmt[len] = '\n';
        prd = gs_printf_ctrl.pbuf_fmt;

        if (use_irq)
        {
            char *pnext = (char *)gs_printf_ctrl.pw;

            while (len--)
            {
                if ((u32)(++pnext) >= (u32)gs_printf_ctrl.pend)
                {
                    pnext = gs_printf_ctrl.pst;
                }

                if (pnext != gs_printf_ctrl.pr)
                {
                    *gs_printf_ctrl.pw = *prd++;
                    gs_printf_ctrl.pw = pnext;
                }
                else //缓存不够
                {
                    break;
                }
            }

            xprintf_msg_put_irq();
        }
        else
        {
            while (len--)
            {
                xprintf_put_char(*prd++);
            }
        }
    }

    irq_enable();
}

/***********************************************************
 * 功能描述：打印输出内容
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf(const char *pfmt, ...)
{
    va_list va;

	va_start(va, pfmt);
	xvprintf(pfmt, va, TRUE);
    va_end(va);
}

/***********************************************************
 * 功能描述：打印输出内容(普通方式发送)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf_w(const char *pfmt, ...)
{
    va_list va;

    va_start(va, pfmt);
    xvprintf(pfmt, va, FALSE);
    va_end(va);
}

/***********************************************************
 * 功能描述：打印初始化
 * 输入参数：pbuf_fmt     格式化缓冲区(一次输出最大长度)
 *          fmt_len      格式化输出缓冲区长度
 *          pbuf_send    发送缓冲区 (最大缓存数据长度)
 *          pbuf_send_len 发送缓冲区长度
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
void xprintf_init_com(char *pbuf_fmt, u16 fmt_len, char *pbuf_send, u16 pbuf_send_len)
{
    gs_printf_ctrl.pbuf_fmt = pbuf_fmt;
    gs_printf_ctrl.pbuf_fmt_len = fmt_len;
    gs_printf_ctrl.pst = pbuf_send;
    gs_printf_ctrl.pend = gs_printf_ctrl.pst + pbuf_send_len;
    gs_printf_ctrl.pw = gs_printf_ctrl.pr = gs_printf_ctrl.pst;
}



