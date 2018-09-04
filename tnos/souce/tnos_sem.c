/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_sem.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统 信号量
 * 版本历史:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_SEM_ENABLE != 0)


/***********************************************************
 * 功能描述：信号量初始
 * 输入参数：psem 信号量
 *           cnt 初始计数值
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
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
 * 功能描述：信号量 发送
 * 输入参数：psem 信号量
 *           cnt 初始计数值
 * 输出参数： 无
 * 返 回 值：  无
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
 * 功能描述：信号量等待(中断中不允许调用!!)
 * 输入参数：psingal 信号结构体
 *          timeout_ms 超时ms时间
 * 输出参数： 无
 * 返 回 值：  当前信号量计数, > 0 有, 其它失败
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
 * 功能描述：信号量等待(中断中不允许调用!!)
 * 输入参数：psingal 信号结构体
 * 输出参数： 情况等待次数
 * 返 回 值：  无
 ***********************************************************/
void tnos_sem_clean(tnos_sem_t *psem)
{
    u32 reg;

    reg = irq_disable();
    tnos_singal_clear_cnt(&psem->singal);
    irq_enable(reg);
}


#endif
