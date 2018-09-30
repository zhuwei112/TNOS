/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_mutex.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统 互斥锁
 * 版本历史:
 ***********************************************************/
#include "tnos_core.h"

#if (TNOS_MUTEX_ENABLE != 0)


/***********************************************************
 * 功能描述：互斥锁 初始化
 * 输入参数：pmutex 互斥锁
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
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
 * 功能描述：互斥锁 尝试上锁 (支持锁嵌套,可多次锁,中断不能调用)
 * 输入参数：pmutex 互斥锁
 *          timeout_ms 超时时间
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
 ***********************************************************/
s32 tnos_mutex_trylock(tnos_mutex_t *pmutex, u32 timeout_ms)
{
    u32 i;

    TNOS_ASSERT(pmutex != NULL);

    for (i = 0; i < 2; i++)
    {
        irq_disable();
        tnos_singal_clean(&pmutex->singal);

        if ((pmutex->lock_cnt == 0) || (pmutex->ptcb_send == gs_ptnos_tcb_cur)) //没有锁或者当前当前任务锁定
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
 * 功能描述：互斥锁 上锁 (支持锁嵌套,可多次锁,中断不能调用)
 * 输入参数：pmutex 互斥锁
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_mutex_lock(tnos_mutex_t *pmutex)
{
    TNOS_ASSERT(pmutex != NULL);

    while (tnos_mutex_trylock(pmutex, 1000) != TNOS_ERR_NONE)
    {
    }
}


/***********************************************************
 * 功能描述：互斥锁 解锁 (支持锁嵌套,中断不能调用)
 * 输入参数：pmutex 互斥锁
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_mutex_unlock(tnos_mutex_t *pmutex)
{
    TNOS_ASSERT(pmutex != NULL);

    irq_disable();
    if (pmutex->lock_cnt > 0) //解锁一次退出
    {
        --pmutex->lock_cnt;
    }

    tnos_singal_send(&pmutex->singal);
    irq_enable();
}

#endif



