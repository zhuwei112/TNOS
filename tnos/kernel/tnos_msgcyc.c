/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_cycbuf.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统 循环缓冲区
 * 版本历史:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_MSGCYC_ENABLE != 0)


/***********************************************************
 * 功能描述：消息循环区 清空数据
 * 输入参数：pmsgc    消息循环区
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
static void do_msgcyc_clean(tnos_msgcyc_t *pmsgc)
{
    pmsgc->pos_w = pmsgc->pos_r = 0;
    tnos_singal_clear_cnt(&pmsgc->singal);
}

/***********************************************************
 * 功能描述 消息循环区 初始化
 * 输入参数：pmsgc    消息循环区
 *          pbuf      缓冲区起始位置
 *          buf_size  缓冲区大小
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
 *          最大消息大小为 buf_size - 4 - 1
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

    if (less != 0) //起始地址4字节对齐
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
 * 功能描述：消息循环区 清空数据
 * 输入参数：pmsgc    消息循环区
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
void tnos_msgcyc_clean(tnos_msgcyc_t *pmsgc)
{
    irq_disable();
    do_msgcyc_clean(pmsgc);
    irq_enable();
}

/***********************************************************
 * 功能描述：消息循环区 获取空的大小
 * 输入参数：pmsgc    消息循环区
 * 输出参数： 无
 * 返 回 值：空的大小
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
 * 功能描述：消息循环区 发送数据(先进先出)
 * 输入参数：pmsgc    消息循环区
 *          pbuf   发送数据缓冲区
 *          len    发送数据长度
 * 输出参数： 无
 * 返 回 值： 0:成功 其它:失败 TNOS_ERR_LESS:空间不够
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

    if (pmsgc->buf_size == 0) //没有初始化
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
            if (pmsgc->pos_r < len_space + 4) //4防止读写相同
            {
                irq_enable();
                return TNOS_ERR_LESS;
            }

            pd = (tnos_msgcyc_data_t *)&(pmsgc->pbuf[pmsgc->pos_w]);
            pd->len = 0;              //剩余不够的位置丢弃
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

        if (less < len_space + 4) //至少有个空位
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
 * 功能描述：消息循环区 接收数据指针获取 (法2:获取指针)
 * 输入参数：pmsgqc消息循环区
 *          timeout_ms 超时时间
 * 输出参数： pbuf      接收后的数据位置(一定为4字节对齐)
 * 返 回 值：  接收数据的长度, 负数或者0 失败
 * 注意: (返回> 0时,一定要释放 tnos_msgq_rev_ptr_next)
 ***********************************************************/
s32 do_tnos_msgq_rev_ptr(tnos_msgcyc_t *pmsgc, u8 **pbuf, u32 timeout_ms)
{
    s32 num;

    TNOS_ASSERT((pmsgc != NULL) && (pmsgc != NULL))

    *pbuf = NULL;

    if (pmsgc->buf_size == 0) //没有初始化
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
            pmsgc->pos_r = pmsgc->pos_w = 0; //移到最开始,方便写分配空间
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
 * 功能描述：消息循环区 接收数据指针获取 (法2:获取指针)
 * 输入参数：pmsgqc消息循环区
 *          timeout_ms 超时时间
 * 输出参数： pbuf      接收后的数据位置(一定为4字节对齐)
 * 返 回 值：  接收数据的长度, 负数或者0 失败
 * 注意: (返回> 0时,一定要释放 tnos_msgcyc_rev_ptr_next)
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
* 功能描述：消息循环区 接收数据指针释放(法2:释放接收指针)
* 输入参数：pmsgq 消息队列
*          ppdata 队列数据指针
* 输出参数： 无
* 返 回 值：  无
***********************************************************/
void do_tnos_msgcyc_rev_ptr_next(tnos_msgcyc_t *pmsgc)
{
    BOOL is_twince = FALSE;

    while (1) //去掉写后面空的位置
    {
        tnos_msgcyc_data_t *pd;

        if (pmsgc->pos_r == pmsgc->pos_w)
        {
            pmsgc->pos_r = pmsgc->pos_w = 0; //移到最开始,方便写分配空间
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
* 功能描述：消息循环区 接收数据指针释放(法2:释放接收指针)
* 输入参数：pmsgq 消息队列
*          ppdata 队列数据指针
* 输出参数： 无
* 返 回 值：  无
***********************************************************/
void tnos_msgcyc_rev_ptr_next(tnos_msgcyc_t *pmsgc)
{
    irq_disable();
    do_tnos_msgcyc_rev_ptr_next(pmsgc);
    irq_enable();
}


/***********************************************************
 * 功能描述：消息循环区接收数据
 * 输入参数：pmsgc    消息循环区
 *          buf_len  接收数据的长度
 *          timeout_ms 超时时间
 * 输出参数： pbuf 接收数据缓冲区
 * 返 回 值：  接收数据的长度,  负数和0失败
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
