/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: xtimer.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 定时器封装
 * 版本历史:
 ***********************************************************/


#include "xtimer.h"

time_tick_t g_tick = {0};



/***********************************************************
 * 功能描述：比较时间大小
 * 输入参数：tnew 新的时间
 *          told  旧的时间
 * 输出参数： 无
 * 返 回 值：  1: tnew > told     0 : tnew = told    -1: tnew < told
 ***********************************************************/
s32 ticks_cmp(time_tick_t *tnew, time_tick_t *told)
{
    if (tnew->s > told->s)
    { // >
        return 1;
    }

    if (tnew->s == told->s)
    {
        if  (tnew->ms > told->ms)
        {
            return 1;
        }

        if  (tnew->ms == told->ms)
        {
            return 0;
        }
    }

    return -1;
}


/***********************************************************
 * 功能描述：延迟ms时间
 * 输入参数： 等待ms时间
 * 输出参数：  无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_delay_ms(u32 ms)
{
    ttimer_t tm;

    ttimer_set(&tm, ms);
    ttimer_wait_timeout(&tm);
}



/***********************************************************
 * 功能描述：查询是否已经超时
 * 输入参数： ptick_now 时间(比较的时间)(不能为NULL)
 * 输出参数： 无
 * 返 回 值：  FALSE 无, 其他超时 (停止后返回FALSE)
 ***********************************************************/
BOOL ttimer_is_timeout2(ttimer_t *pthis, time_tick_t *ptick_now)
{
#if (TIMER_USE_STOP != 0)
    if (pthis->ctrl & BIT(TTIMER_BIT_STOP))
    {
    	return FALSE;
    }
#endif

	if (!(pthis->ctrl & BIT(TTIMER_BIT_TIMEOUT)))
	{
		s32 sec = ptick_now->s - pthis->tick_timeout.s;

		if (sec == 0)
		{
			if (ptick_now->ms < pthis->tick_timeout.ms)
			{
				return FALSE;
			}
		}
		else if (sec < 0) //时间不够
		{
			return FALSE;
		}

		pthis->ctrl |= BIT(TTIMER_BIT_TIMEOUT);
	}

	return TRUE;
}

/***********************************************************
 * 功能描述：查询是否已经超时(如果等待为us时间且< 500,不能用(不精确))
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值：  FALSE 无, 其他超时 (停止后返回FALSE)
 ***********************************************************/
BOOL ttimer_is_timeout(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    return ttimer_is_timeout2(pthis, &tick_now);
}

/***********************************************************
 * 功能描述：查询是否已经超时
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值：  FALSE 无, 其他超时 (停止后返回FALSE)
 ***********************************************************/
void ttimer_wait_timeout(ttimer_t *pthis)
{
	while (!ttimer_is_timeout(pthis))
	{
	}
}

/***********************************************************
 * 功能描述：剩余的事件
 * 输入参数： ptick_now 比较时间 (不能为NULL)
 * 输出参数： 无
 * 返 回 值：  0 已超时或已经停止,  正数 剩余时间
 ***********************************************************/
u32 ttimer_time_less2(ttimer_t *pthis, time_tick_t *ptick_now)
{
#if (TIMER_USE_STOP != 0)
    if (pthis->ctrl & BIT(TTIMER_BIT_STOP))
    {
    	return 1;
    }
#endif

    if (!(pthis->ctrl & BIT(TTIMER_BIT_TIMEOUT)))
    {
    	s32 sec = pthis->tick_timeout.s - ptick_now->s;
    	s32 ms = pthis->tick_timeout.ms - ptick_now->ms;

    	if (sec > 0)
    	{
    		s32 ret = sec*1000 + ms;

    		return ret;
    	}

    	if ((sec == 0) && (ms > 0))
    	{
    		return ms;
    	}

    	pthis->ctrl |= BIT(TTIMER_BIT_TIMEOUT);
    }

    return 0;
}


/***********************************************************
 * 功能描述：剩余的事件
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值：  0 已超时或已经停止,  正数 剩余时间
 ***********************************************************/
u32 ttimer_time_less(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    return ttimer_time_less2(pthis, &tick_now);
}

/***********************************************************
 * 功能描述：过去的时间
 * 输入参数： ptick_old 等待开始时间
 *           ptick_new 最后的时间
 * 输出参数： 无
 * 返 回 值：  过去的ms数
 ***********************************************************/
u32 pass_ticks(const time_tick_t *ptick_old, time_tick_t *ptick_new)
{
	s32 ret;
    s32 sec = ptick_new->s - ptick_old->s;
    s32 ms = ptick_new->ms - ptick_old->ms;

    if ((sec < 0) || ((sec == 0) && (ms <= 0)))
    {
        return 0;
    }

    ret = sec*1000 + ms;

    return ret;
}


/***********************************************************
 * 功能描述：到现在过去的时间ms
 * 输入参数： ptick 等待开始时间
 * 输出参数： ptick_now 现在的时间 (NULL 不返回)
 * 返 回 值：  过去的ms数
 ***********************************************************/
 u32 pass_ticks_now(const time_tick_t *ptick, time_tick_t *ptick_now)
{
	u32 ret;
	 time_tick_t tick_now;

	 get_time_tick(&tick_now);
     ret = pass_ticks(ptick, &tick_now);

     if (ptick_now != NULL)
     {
         ptick_now->s = tick_now.s;
         ptick_now->ms = tick_now.ms;
     }

     return ret;
}

 /***********************************************************
  * 功能描述：增加 us 时间
  * 输入参数：
  * 输出参数： 无
  * 返 回 值：  无
  ***********************************************************/
INLINE__ void time_add(time_tick_t *ptick, u32 ms)
{
	u32 ms_in = ptick->ms + ms;

	ptick->ms = ms_in%1000;
	ptick->s += ms_in/1000;
}

/***********************************************************
 * 功能描述：设置延迟时间
 * 输入参数： ms 延迟时间(最大6天)
 *           ptick_now 时间 (不能为NULL)
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_set2(ttimer_t *pthis, u32 ms, time_tick_t *ptick_now)
{
    if (ms != 0)
    {
        pthis->tick_timeout.s = ptick_now->s;
        pthis->tick_timeout.ms = ptick_now->ms;

        if (ms > TTIMER_MASK_DELAY)
        {
            ms = TTIMER_MASK_DELAY;
        }

        pthis->ctrl = ms;
        time_add(&pthis->tick_timeout, ms);
    }
    else
    {
        pthis->ctrl = BIT(TTIMER_BIT_TIMEOUT);
    }
}

/***********************************************************
 * 功能描述：设置延迟时间
 * 输入参数： ms 延迟时间
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_set(ttimer_t *pthis, u32 ms)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);

    ttimer_set2(pthis, ms, &tick_now);
}

/***********************************************************
 * 功能描述：设置超时时间为上一次非停止的set时间(当前时间重新计算时间)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：   无
 ***********************************************************/
void ttimer_repeat(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    ttimer_set2(pthis, pthis->ctrl & TTIMER_MASK_DELAY, &tick_now);
}

/***********************************************************
 * 功能描述：设置超时时间为上一次非停止的set时间(上次超时时间增加时间)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：   无
 ***********************************************************/
void ttimer_repeat_abs(ttimer_t *pthis)
{
	u32 ms = pthis->ctrl & TTIMER_MASK_DELAY;

	pthis->ctrl &= ms;
	time_add(&pthis->tick_timeout, ms);
}




#undef TEST_
//#define TEST_


#ifdef TEST_

/*-------------------------------------
 *test
---------------------------------------*/
void ttimer_test(void)
{
}
#else
/*-------------------------------------
 *test
---------------------------------------*/
void ttimer_test(void)
{
}
#endif





