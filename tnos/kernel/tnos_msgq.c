/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_msgq.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统 消息队列
 * 版本历史:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_MSGQ_ENABLE != 0)


//使用的pos为保存pos - 1
//将位置转为地址
#define POS2ADDR(pos_next)   ((tnos_msgq_data_t *)(pmsgq->pbuf + (((pos_next) - 1)*(pmsgq->size + sizeof(tnos_msgq_data_t)))))

//将地址转为位置
#define ADDR2POS(addr)       ((((u32)(addr) - (u32)pmsgq->pbuf)/(pmsgq->size + sizeof(tnos_msgq_data_t))) + 1)

/***********************************************************
 * 功能描述：清空所有数据
 * 输入参数： pmsgq 消息队列
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
static void do_msgq_clean(tnos_msgq_t *pmsgq)
{
    pmsgq->pos_empty = pmsgq->pos_head = pmsgq->pos_tail = 0;
    tnos_singal_clear_cnt(&pmsgq->singal);

    if (pmsgq->size != 0)
    {
        u32 i;

        for (i = 1; i <= pmsgq->num_max; i++) //位置从1开始
        {
            tnos_msgq_data_t *pdata = POS2ADDR(i);

            pdata->pos_next = pmsgq->pos_empty;
            pmsgq->pos_empty = i;
        }
    }
}


/***********************************************************
 * 功能描述 消息队列 初始化
 * 输入参数：pmsgq    消息队列
 *          pbuf      缓冲区起始位置
 *          buf_size  缓冲区大小
 *          msg_size  一条消息最大大小
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
 * 注释:消息的个数为 (((buf_size+3)/4)*4)/(sizeof(tnos_msgq_data_t) + ((msg_size +3/4)*4)))
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

    if (less != 0) //起始地址4字节对齐
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
 * 功能描述：清空所有数据
 * 输入参数： pmsgq 消息队列
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
void tnos_msgq_clean(tnos_msgq_t *pmsgq)
{
    irq_disable();
    do_msgq_clean(pmsgq);
    irq_enable();
}

/***********************************************************
 * 功能描述：消息队列 发送数据指针获取 (法2:获取发送指针)
 * 输入参数：pmsgq 消息队列
 * 输出参数： pmsgq_data 队列数据指针
 * 返 回 值： 0:成功  其它:失败
 * 注意: (返回 == 0时,一定要调用 tnos_msgq_send_ptr_next ,否则队列泄露)
 *       写数据时:长度不能超过msg_size 否则队列崩溃
 ***********************************************************/
s32 tnos_msgq_send_ptr(tnos_msgq_t *pmsgq, tnos_msgq_data_t **pmsgq_data)
{
    u16 pos;
    tnos_msgq_data_t *pdata;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL));

    *pmsgq_data = NULL;

    if (pmsgq->size == 0) //没有初始化
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
* 功能描述：消息队列发送指针 发送数据(法2:发送指针发送数据)
* 输入参数：pmsgq 消息队列
*          ppdata 队列数据指针
*          data_len 发送数据的长度
*          is_first 是否(先进后出)
* 输出参数： 无
* 返 回 值：  0:成功 其它:错误
***********************************************************/
s32 tnos_msgq_send_ptr_next(tnos_msgq_t *pmsgq, tnos_msgq_data_t *pmsgq_data, u32 data_len, BOOL is_first)
{
    u16 pos;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    pos = ADDR2POS(pmsgq_data);

    irq_disable();
    if (data_len > pmsgq->size) //不允许发送长度超过缓冲区
    {
        pmsgq_data->pos_next = pmsgq->pos_empty;
        pmsgq->pos_empty = pos;
        irq_enable();

        return TNOS_ERR_ARG;
    }

    pmsgq_data->len = data_len;

    if (pmsgq_data->len == 0) //发送0长度数据,放到空队列中
    {
        pmsgq_data->pos_next = pmsgq->pos_empty;
        pmsgq->pos_empty = pos;
    }
    else //队列中的数据一定有长度
    {
        if (!is_first) //先进先出
        {
            //从尾部插入数据
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
        else //先进后出
        { //从头部插入
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
 * 功能描述：消息队列 发送数据
 * 输入参数：pmsgq 消息队列
 *          pbuf   发送数据缓冲区
 *          len    发送数据长度
 * 输出参数： 无
 * 返 回 值： 0:成功 其它:失败
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
 * 功能描述：消息队列 发送数据(先进先出)
 * 输入参数：pmsgq 消息队列
 *          pbuf   发送数据缓冲区
 *          len    发送数据长度
 * 输出参数： 无
 * 返 回 值： 0:成功 其它:失败
 ***********************************************************/
s32 tnos_msgq_send(tnos_msgq_t *pmsgq, const void *pbuf, u32 len)
{
    return do_msgq_send(pmsgq, pbuf, len, FALSE);
}

/***********************************************************
 * 功能描述：消息队列 发送数据(先进后出)
 * 输入参数：pmsgq 消息队列
 *          pbuf   发送数据缓冲区
 *          len    发送数据长度
 * 输出参数： 无
 * 返 回 值： 0:成功 其它:失败
 ***********************************************************/
s32 tnos_msgq_send_frist(tnos_msgq_t *pmsgq, const void *pbuf, u32 len)
{
    return do_msgq_send(pmsgq, pbuf, len, TRUE);
}

/***********************************************************
 * 功能描述：消息队列 接收数据指针获取 (法2:获取指针)
 * 输入参数：pmsgq 消息队列
 *          timeout_ms 超时时间
 * 输出参数： pmsgq_data 队列数据指针
 * 返 回 值：  接收数据的长度, 负数或者0 失败
 * 注意: (返回> 0时,一定要释放 tnos_msgq_rev_ptr_next)
 *       返回的数据起始部分  TNOS_MSGQ_GET_BUF获取
 ***********************************************************/
s32 tnos_msgq_rev_ptr(tnos_msgq_t *pmsgq, tnos_msgq_data_t **pmsgq_data, u32 timeout_ms)
{
    tnos_msgq_data_t *pdata;
    u16 pos;
    s32 num;
    u32 len;

    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    *pmsgq_data = NULL;

    if (pmsgq->size == 0) //没有初始化
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

    if (pos == 0) //出现BUG,才会有
    {
        irq_enable();

        return TNOS_ERR_BUG;
    }

    pdata = POS2ADDR(pos);

    if (pmsgq->pos_tail == pos) //最后一个数据
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

    if ((len == 0) || (len > pmsgq->size)) //队列中一定有数据长度且不会溢出最大长度
    {
        return TNOS_ERR_BUG;
    }

    return len;
}

/***********************************************************
* 功能描述：消息队列 接收数据指针释放(法2:释放接收数据)
* 输入参数：pmsgq 消息队列
*          pmsgq_data 队列数据指针
* 输出参数： 无
* 返 回 值：  无
***********************************************************/
void tnos_msgq_rev_ptr_next(tnos_msgq_t *pmsgq, tnos_msgq_data_t *pmsgq_data)
{
    TNOS_ASSERT((pmsgq != NULL) && (pmsgq_data != NULL))

    irq_disable();
    //加入到空队列中
    pmsgq_data->pos_next = pmsgq->pos_empty;
    pmsgq->pos_empty = ADDR2POS(pmsgq_data);
    irq_enable();
}

/***********************************************************
 * 功能描述：消息队列 接收数据
 * 输入参数：pmsgq 消息队列
 *          buf_len  接收数据的长度
 *          timeout_ms 超时时间
 * 输出参数： pbuf 接收数据缓冲区
 * 返 回 值：  接收数据的长度, 负数和0失败
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
