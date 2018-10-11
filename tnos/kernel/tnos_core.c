/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_core.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统核心文件
 * 版本历史:
 ***********************************************************/

#include "tnos_core.h"
#include "xprintf.h"
#include "xutils.h"

#if (TNOS_PRO_MAX > 4)
#error "TNOS_PRO_MAX max 4"
#endif

//#define TNOS_HAVE_DBG

#ifdef TNOS_HAVE_DBG
    #define TNOS_DBG             DBG
    #define TNOS_DBG_USE         1
    #define TNOS_DBG_LIST        printf_list

#else
    #define TNOS_DBG(...)
     #define TNOS_DBG_USE        0
    #define TNOS_DBG_LIST(name)
#endif



tnos_tcb_t gs_idle_tcb;                   //空闲任务tcb
tnos_tcb_t *gs_ptnos_tcb_cur, *gs_ptnos_tcb_ready; //当前任务tcb, 准备运行任务tcb

#if (TNOS_STK_CHECK != 0)
static u32 gs_idle_sec;
#endif
static u32 gs_idle_stk[TNOS_IDLE_STK_NUM]; //空闲任务堆栈

static list_t gs_list_head_delay; //等待运行队列头部
static list_t gs_list_rdy[2][TNOS_PRO_MAX]; //就绪队列, 就绪等待队列
static list_t *gs_prdy, *gs_prdy_wait;     //就绪队列指针, 就绪等待队列指针
static volatile s16  gs_tnos_shed_lock_cnt = 0; //调度锁的次数
static volatile s16 gs_irq_cnt = 0; //中断嵌套的个数
static u8 gs_rdy_grop, gs_rdy_wait_grop;  //就绪队列组, 就绪等待队列指针组
static BOOL8 gs_is_signal_send; //是否有信号发送(中断判断是否切换任务)



//最高任务所在的位置
const static u8 gs_higth_rdy_tab[] =
{
     0xFF, 0, 1, 0,    2, 0, 1, 0,
     3, 0, 1, 0,       2, 0, 1, 0
};


/***********************************************************
 * 功能描述：禁止中断(支持中断嵌套)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void irq_disable(void)
{
    hw_irq_disable();
    ++gs_irq_cnt;
}
/***********************************************************
 * 功能描述：允许中断(支持中断嵌套)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:中断嵌套不匹配导致中断永久性关闭!!!
 ***********************************************************/
void irq_enable(void)
{
    if (--gs_irq_cnt == 0)
    {
        hw_irq_enable();
    }

    if (gs_irq_cnt < 0)
    {
        gs_irq_cnt = 0;

        DBG("BUG: irq lock!!!!!");
		
		
        TNOS_ASSERT(0);
    }
}


/***********************************************************
 * 功能描述：变量初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_val_init(void)
{
    u32 i;

    for (i = 0; i < TNOS_PRO_MAX; i++)
    {
        list_init(&gs_list_rdy[0][i]);
        list_init(&gs_list_rdy[1][i]);
    }

    gs_prdy = gs_list_rdy[0];
    gs_prdy_wait = gs_list_rdy[1];
    gs_rdy_grop = gs_rdy_wait_grop = 0;
    gs_is_signal_send = FALSE;

    list_init(&gs_list_head_delay);
    gs_tnos_shed_lock_cnt = 0xFF; //调度器被锁的次数
    gs_ptnos_tcb_ready = &gs_idle_tcb; //当前运行的tcb
    gs_ptnos_tcb_cur = NULL;
    tnos_task_create(&gs_idle_tcb, TNOS_TASK_IDLE_NAME, TNOS_PRO_LOW, 2, idle_task, NULL, gs_idle_stk, ARRAY_SIZE(gs_idle_stk));

#if (TNOS_STK_NAME != 0)
    tnos_task_set_name(&gs_idle_tcb, "idle");
#endif

#if (TNOS_STK_CHECK != 0)
    gs_idle_sec = 0;
#endif

}


/***********************************************************
 * 功能描述：系统开始运行(调用后不返回!!!)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_run(void)
{
    irq_disable();
    tnos_tim_ms_set(100);
    tnos_start_rdy();

    tnos_task_sw();
    irq_enable();

    while (1)
    {
    }
}

/***********************************************************
 * 功能描述：tnos 打印系统信息
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_printf(void)
{
    xprintf("\n\n");
    xprintf("RTOS  is  Operating System");
    xprintf("  SVN:%d.%d.%d", TNOS_VER_MAJOR, TNOS_VER_SUB, TNOS_VER_REVISE);
    xprintf("  2017 - 2018 Copyright by rtos team");
}


/***********************************************************
 * 功能描述：系统启动
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_startup(void)
{
    tnos_board_sys_pro_init();

    tnos_val_init();

    xprintf_init(); //打印初始化

    tnos_printf();

    ttimer_init(); //定时器初始化

    tnos_tim_ms_init(); //系统ms延迟初始化

    tnos_task_init();

    tnos_board_other_init(); //板子硬件初始化

    tnos_app_init();

    tnos_run();
}


#if (TNOS_DBG_USE != 0)
/***********************************************************
 * 功能描述：打印运行列表
 * 输入参数：
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void printf_list(const char *pname)
{
    u32 i;
    list_t *plist;
    tnos_tcb_t *ptcb;

    TNOS_DBG("%s", pname);

    for (i = 0; i < TNOS_PRO_MAX; i++)
    {
        list_t *plist_head = &gs_prdy[i];

        for (plist = plist_head->next; plist != plist_head; plist = plist->next)
        {
            ptcb = LIST_ENTRY(plist, tnos_tcb_t, list_run);
            TNOS_DBG(" name[%u][%s]", i, ptcb->name);
        }
    }

#if 1
    for (i = 0; i < TNOS_PRO_MAX; i++)
    {
        list_t *plist_head = &gs_prdy[i];

        for (plist = plist_head->prev; plist != plist_head; plist = plist->prev)
        {
            ptcb = LIST_ENTRY(plist, tnos_tcb_t, list_run);
            TNOS_DBG(" name end[%u][%s]", i, ptcb->name);
        }
    }
#endif
}
#endif



#if 0
/***********************************************************
 * 功能描述：找到最小延迟超时时间  (存在有任务优先级低,时间小,切换不了线程,时间片不起作用)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  滴答值
 ***********************************************************/
static void tnos_get_delay_min(u32 *pnext_time, u32 mss, const time_tick_t *ptick_now)
{
    list_t *plist = gs_list_head_delay.next;

    if (plist != &gs_list_head_delay)
    {
        tnos_tcb_t *p = LIST_ENTRY(plist, tnos_tcb_t, list_delay);
        u32 less = pass_ticks(ptick_now, &p->tm.tick_timeout);

       if (mss < less)
       {
           *pnext_time = less;
       }
    }
    else //定时为空
    {
        *pnext_time = mss;
    }
}
#endif

/***********************************************************
 * 功能描述：将任务插入到超时队列中
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  滴答值
 ***********************************************************/
static void tnos_insert_delay(tnos_tcb_t *ptcb, u32 *pnext_time)
{
    list_t *plist;

#if (TNOS_DBG_USE != 0)
    TNOS_ASSERT(ptcb != &gs_idle_tcb);
#endif

    //按时间由小到大排列
    for (plist = gs_list_head_delay.next; plist != &gs_list_head_delay; plist = plist->next)
    {
        tnos_tcb_t *p = LIST_ENTRY(plist, tnos_tcb_t, list_delay);

        if (ptcb->tm.tick_timeout.s > p->tm.tick_timeout.s)
        {
            continue;
        }

        if ((ptcb->tm.tick_timeout.s == p->tm.tick_timeout.s) &&
            (ptcb->tm.tick_timeout.ms >= p->tm.tick_timeout.ms))
        {
            continue;
        }

        break;
    }

    if (plist == gs_list_head_delay.next) //当前任务为延迟最短的任务
    {
        *pnext_time = ttimer_get_delay(&ptcb->tm);
    }

    list_insert_before(plist, &ptcb->list_delay);
}

/***********************************************************
 * 功能描述 :时间片用完插入等待中 (只有调度任务才能使用, 必须移除 list_run)
 * 输入参数：ptcb     任务参数
 *          pro       优先级
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_set_ready_wait(tnos_tcb_t *ptcb)
{
    TNOS_DBG("READ wait[%u] [%s]", ptcb->pro, ptcb->name);

#if (TNOS_DBG_USE != 0)
    TNOS_ASSERT(ptcb != &gs_idle_tcb);
#endif

    gs_rdy_wait_grop |= BIT(ptcb->pro);
    ptcb->ms_less = ptcb->mss;
    list_insert_before(&gs_prdy_wait[ptcb->pro], &ptcb->list_run);
}


//存在处于 等待队列中,之后插入运行导致重新分配时间片
/***********************************************************
 * 功能描述：设置任务准备运行(不能再延迟队列中,空闲任务不允许调用)
 * 输入参数：ptcb     任务参数
 *          pro       优先级
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_set_ready(tnos_tcb_t *ptcb, BOOL is_after)
{
    TNOS_DBG("READ[%u] [%s]", ptcb->pro, ptcb->name);

#if (TNOS_DBG_USE != 0)
    TNOS_ASSERT(ptcb != &gs_idle_tcb);
#endif

    if (ptcb->list_run.next == NULL) //在队列中不动(防止gs_rdy_grop 对优先级判断不对)
    {//防止在准备队列中被重新分配时间片
        ptcb->ms_less = ptcb->mss;
        gs_rdy_grop |= BIT(ptcb->pro);

        if (!is_after)
        {
            list_insert_before(&gs_prdy[ptcb->pro], &ptcb->list_run);
        }
        else
        {
            list_insert_after(&gs_prdy[ptcb->pro], &ptcb->list_run);
        }
    }
    else
    {
        TNOS_DBG("READ have");
    }
}

/***********************************************************
 * 功能描述：调度(必须禁止中断) (gs_ptnos_tcb_cur->tick_slice = 0; 强制调度)
 * 输入参数：tick_delay 当前任务需要延迟的tick数
 * 输出参数： pnext_time 如果有返回一定不为0
 * 返 回 值：  无
 ***********************************************************/
static void tnos_do_sched(time_tick_t *ptick_now, u32 delay_ms, u32 *pnext_time)
{
    register tnos_tcb_t *pcur;
    register tnos_tcb_t *pnext; //下一个运行的任务
//    BOOL is_old_shed;

	pcur = gs_ptnos_tcb_cur;

	TNOS_DBG("");
    TNOS_DBG("cur name[%s],rb[%u]t[%u.%03u]", pcur->name, gs_rdy_grop, ptick_now->s, ptick_now->ms);

	//当前任务为非空闲任务才比较时间
    //高优先级和同优先级
	//时间片分配: 当运行队列为 空时分配时间片
	//           任务变就绪,且不再运行和运行等待队列中,增加时间片

//    is_old_shed = FALSE;

    if (pcur != gs_ptnos_tcb_ready) //还没调度完成,重新转回到队列中
    {
		register tnos_tcb_t *pready;

//        is_old_shed = TRUE;
        pready = gs_ptnos_tcb_ready;

        if (pready != &gs_idle_tcb) //切换到空闲任务不需要管
        {
            TNOS_DBG("delay wait");

            if (pready->list_run.next == NULL) //不在运行队列中
            {
                u32 mss;

                TNOS_DBG("  delay wait ok");
                list_remove(&pready->list_delay);
                mss = pready->mss;
                tnos_set_ready(pready, TRUE);
                pready->mss = mss;
            }
        }
    }
    else  if (pcur != &gs_idle_tcb)
    {
        u8 ms_less = ttimer_time_less2(&pcur->tm, ptick_now);

        if (ms_less != 0)  //高优先级和同优先级可以打断低优先级
        {
            if (pcur->pro < gs_higth_rdy_tab[gs_rdy_grop]) //可能存在空链表被打断
            {
                TNOS_DBG("low");
                return;
            }

            TNOS_DBG("higher");
        }

        list_remove(&pcur->list_delay); //防止当前任务在延迟队列中

        if (delay_ms == 0)
        {
            if (ms_less != 0) //被高优先级中断
            {
                TNOS_DBG("  cur ready");
                tnos_set_ready(pcur, FALSE);
                pcur->ms_less = ms_less;
            }
            else //时间片用完, 防止放后面,没任务时不给当前任务分时间片
            {
                TNOS_DBG("  cur wait");
                if (pcur->list_run.next == NULL) //不在运行队列中
                {
                    TNOS_DBG("  cur wait ok");
                    tnos_set_ready_wait(pcur);
                }
            }
        }
        else  //不需要延迟(被打断),直接设置为准备
        {//(定时中断一定不会进入)
            TNOS_DBG("delay[%u]", delay_ms);
            pcur->is_timeout_del = FALSE;
            ttimer_set2(&pcur->tm, delay_ms, ptick_now);
            tnos_insert_delay(pcur, pnext_time);
        }
    }
    
    TNOS_DBG_LIST("s in");

    while (1)
    {
    	u8 pos = gs_higth_rdy_tab[gs_rdy_grop];

    	if (pos < TNOS_PRO_MAX)
    	{
    	    u32 ms_less;
    		list_t *plist_head = &gs_prdy[pos];
    		list_t *plist = plist_head->next;

    		if (plist == plist_head) //空的 (基本不会到这里,出现则有BUG, gs_rdy_grop 必须和 list_run保持同步)
    		{
                gs_rdy_grop &= ~BIT(pos);
                TNOS_DBG("empty 1");
                continue;
    		}

#if (TNOS_STK_CHECK != 0)
            if (gs_idle_sec != g_tick.s) //强制给空闲任务
            {
                gs_idle_sec = g_tick.s;
                pnext = &gs_idle_tcb;  //没有任何就绪任务,改为空闲任务
                *pnext_time = 2;
                break;
            }
#endif

    		if (plist->next == plist_head) //只有一个
    		{
                plist->next = NULL;     //快速移除当前队列
    			list_init(plist_head);
    			gs_rdy_grop &= ~BIT(pos);
    			TNOS_DBG("one 1");
    		}
    		else
    		{
    			list_remove(plist);
    			TNOS_DBG("l rm");
    		}

    		pnext = LIST_ENTRY(plist, tnos_tcb_t, list_run);
    		pnext->is_timeout_del = TRUE;
            list_remove(&pnext->list_delay); //防止当前任务在延迟队列中

            ms_less = pnext->ms_less;

    		if (pcur != pnext) //不同任务直接调度
    		{
    			ttimer_set2(&pnext->tm, ms_less, ptick_now);
    			tnos_insert_delay(pnext, pnext_time);
    			break;
    		}

    		//同一个任务,奖励 1/2时间片,最多运行1个时间片
    		if (ms_less < (pnext->mss>>1))
    		{
    		    ms_less += (pnext->mss>>1);
    		}
    		else
    		{
    		    ms_less = pnext->mss;
    		}

            pnext->ms_less = ms_less;
    		ttimer_set2(&pnext->tm, ms_less, ptick_now);
    		tnos_insert_delay(pnext, pnext_time);
    		gs_ptnos_tcb_ready = gs_ptnos_tcb_cur; //同一个任务不切换
    		return;
        }

    	if (gs_rdy_wait_grop != 0)  //等待运行队列中有数据,重新分配时间片
        {
    	    list_t *ptmp;

    	    gs_rdy_grop = gs_rdy_wait_grop;
    	    gs_rdy_wait_grop = 0;

    	    ptmp = gs_prdy; //切换位置
    	    gs_prdy = gs_prdy_wait;
    	    gs_prdy_wait = ptmp;
            TNOS_DBG("retime");
            continue;
        }

#if (TNOS_STK_CHECK != 0)
    	gs_idle_sec = g_tick.s;
#endif

        if (pcur == &gs_idle_tcb) //空闲切空闲
        {
            return;
        }

        pnext = &gs_idle_tcb;  //没有任何就绪任务,改为空闲任务
        break;
    }

    gs_ptnos_tcb_ready = pnext;

#if TNOS_STK_CHECK
    pcur->ms_cpu_once += pass_ticks(&pcur->tick_start, ptick_now);
    pnext->tick_start.s = ptick_now->s;
    pnext->tick_start.ms = ptick_now->ms;
#endif

    TNOS_DBG_LIST("shell out");

	TNOS_DBG("s %s to %s", pcur->name, gs_ptnos_tcb_ready->name);
//	if (!is_old_shed)
	{
	    tnos_task_sw();
	}
}


/***********************************************************
 * 功能描述：滴答时间运行处理(中断运行,不允许外部关中断)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tick_proess(void)
{
    list_t *plist;
    time_tick_t tick_now;
    u32 next_time = 1000;

    TNOS_DBG("t");

    irq_disable();
    get_time_tick2(&tick_now);

    plist = gs_list_head_delay.next;

    while (plist != &gs_list_head_delay)    //延时队列中超时的加入到准备队列中(按时间顺序排列完成)
    {
        list_t *plist_next;
        tnos_tcb_t *ptcb = LIST_ENTRY(plist, tnos_tcb_t, list_delay);
        u32 less = ttimer_time_less2(&ptcb->tm, &tick_now);

        if (less != 0)
        {
            next_time = less;
            break;
        }

        plist_next = plist->next;
        list_remove(plist);    //插入准备队列的最后
        plist = plist_next;

        if (!ptcb->is_timeout_del)
        {
            TNOS_DBG_LIST("time change in");

            tnos_set_ready(ptcb, FALSE);

            TNOS_DBG("r[%s]", ptcb->name);
            TNOS_DBG_LIST("time change out");
        }
        else
        {
            TNOS_DBG("r remove[%s]", ptcb->name);
        }
    }

    if (gs_tnos_shed_lock_cnt <= 0)
    {
        tnos_do_sched(&tick_now, 0, &next_time);
    }

    tnos_tim_ms_set(next_time);
    irq_enable();


    TNOS_DBG("  n[%u]", next_time);
}


/***********************************************************
 * 功能描述：设置任务名字
 * 输入参数：ptcb     任务参数
 *           pname    任务名字
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_task_set_name(tnos_tcb_t *ptcb, const char *pname)
{
#if (TNOS_STK_NAME != 0)
    ptcb->pname = pname;
#endif
}

/***********************************************************
 * 功能描述：创建任务
 * 输入参数：ptcb     任务参数
 *          pro  任务的优先级
 *          task     任务函数名
 *          parg     任务传递参数
 *          stk_addr 堆栈起始地址
 *          stk_size 堆栈大小
 * 输出参数： 无
 * 返 回 值：  0成功, 其它失败
 ***********************************************************/
s32 tnos_task_create(tnos_tcb_t *ptcb, const char *pname, tnos_pro_t pro, u8 mss,
                     ptnos_task task, void *parg, u32 *stk_addr, u16 stk_size)
{
    TNOS_ASSERT(((ptcb != NULL) && (pname != NULL) && ((u32)pro < TNOS_PRO_MAX) && (mss > 0)
                 && (task != NULL) && (stk_addr != NULL) && (stk_size > 32)));

    irq_disable();
    if (ptcb != &gs_idle_tcb)
    {
        tnos_tcb_t *ptcb_f = &gs_idle_tcb;

        while (1)
        {
            ptcb_f = ptcb_f->ptcb_next;

            if (ptcb_f == &gs_idle_tcb)
            {
                break;
            }

            if (ptcb_f == ptcb) //同一个任务栈赋值为2个任务
            {
                TNOS_ASSERT(ptcb_f == ptcb);
                irq_enable();

                return TNOS_ERR_ARG;
            }
        }
    }

    memset(ptcb, 0, sizeof(tnos_tcb_t));
    ptcb->pstk = tnos_task_stk_init(task, parg, stk_addr, stk_size);

#if (TNOS_STK_CHECK != 0)
    ptcb->stk_addr = stk_addr;
    ptcb->stk_size = stk_size;
#endif

    ptcb->ms_less = ptcb->mss = mss;
    XSTRNCPY(ptcb->name, pname);
    ptcb->pro = (u8)pro;
    ttimer_set_timeout(&ptcb->tm);

    if (ptcb != &gs_idle_tcb)
    {
        ptcb->ptcb_next = gs_idle_tcb.ptcb_next;
        gs_idle_tcb.ptcb_next = ptcb;
        tnos_set_ready(ptcb, FALSE);
    }
    else
    {
        gs_idle_tcb.ptcb_next = &gs_idle_tcb;
    }

    irq_enable();

    return TNOS_ERR_NONE;
}

/***********************************************************
 * 功能描述：获取当前任务名字
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  名字
 ***********************************************************/
const char* tnos_name(void)
{
	return gs_ptnos_tcb_cur->name;
}


/***********************************************************
 * 功能描述：设置任务优先级
 * 输入参数：ptcb     任务参数
 *        pro      优先级
 * 输出参数： 无
 * 返 回 值：  0:成功 其它:失败
 ***********************************************************/
s32 tnos_set_pro(tnos_tcb_t *ptcb, tnos_pro_t pro)
{
    TNOS_ASSERT((ptcb != NULL) && ((u32)pro < TNOS_PRO_MAX));

    irq_disable();
	ptcb->pro = pro; //必须禁止中断!!!
    irq_enable();

	return TNOS_ERR_NONE;
}


/***********************************************************
 * 功能描述：设置时间片
 * 输入参数：ptcb     任务参数
 *        ms       时间片时间,单位ms
 * 输出参数： 无
 * 返 回 值： 0:成功 其它:失败
 ***********************************************************/
s32 tnos_set_mss(tnos_tcb_t *ptcb, u8 ms)
{
    TNOS_ASSERT(ptcb != NULL);

	if (ms == 0)
	{
		ms = 1;
	}

    irq_disable();
	ptcb->mss = ms; //必须禁止中断!!!
    irq_enable();

	return TNOS_ERR_NONE;
}

/***********************************************************
 * 功能描述：非中断调用调度
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
static void tnos_sched_noral(u32 delay_ms, BOOL set_timeout)
{
    if (set_timeout)
    {
        ttimer_set_timeout(&gs_ptnos_tcb_cur->tm);
    }

    if (gs_tnos_shed_lock_cnt <= 0)
    {
        time_tick_t tick_now;
        u32 next_time;

        get_time_tick2(&tick_now);
        next_time = 0;
        tnos_do_sched(&tick_now, delay_ms, &next_time);

        if (next_time != 0)
        {
            tnos_tim_ms_set(next_time);
        }
    }
}

/***********************************************************
 * 功能描述：调度
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_sched(void)
{
    irq_disable();
    tnos_sched_noral(0, TRUE);
    irq_enable();

    tnos_irq_delay();
}

/***********************************************************
 * 功能描述： 进入中断函数 (调用tnos相关函数的中断需要写)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_interrupt_enter(void)
{
    irq_disable();
    if (gs_tnos_shed_lock_cnt++ == 0)
    {
        gs_is_signal_send = FALSE; //清空信号标志
    }
    irq_enable();
}

/***********************************************************
 * 功能描述： 退出中断函数 (调用tnos相关函数的中断需要写)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_interrupt_exit(void)
{
    irq_disable();
    if (--gs_tnos_shed_lock_cnt == 0)
    {
        if (gs_is_signal_send) //发送过信号
        {
            tnos_sched_noral(0, FALSE);
        }
    }

    if (gs_tnos_shed_lock_cnt < 0)
    {
        DBGW("BUG: irq shed_unlock!!!!!");
        TNOS_ASSERT(0);
    }
    irq_enable();
}

/***********************************************************
 * 功能描述：空闲任务改变
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_idle_change(void)
{
    irq_disable();
    gs_tnos_shed_lock_cnt = 0;
    tnos_sched_noral(0, TRUE);
    irq_enable();
}

/***********************************************************
 * 功能描述：调度 禁止(任务调用)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_sched_lock(void)
{
    irq_disable();
    gs_tnos_shed_lock_cnt++;
    irq_enable();
}

/***********************************************************
 * 功能描述：调度 解锁(任务调用)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_sched_unlock(void)
{
    irq_disable();
    if (--gs_tnos_shed_lock_cnt == 0)
    {
        tnos_sched_noral(0, FALSE);
    }

    if (gs_tnos_shed_lock_cnt < 0)
    {
        DBGW("BUG: shed_unlock!!!!!");
        TNOS_ASSERT(0);
    }

    irq_enable();
}

/***********************************************************
 * 功能描述：等待ms时间(任务调用)
 * 输入参数：delay_ms  ms时间
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_delay_ms(u32 delay_ms)
{
    if (delay_ms != 0)
    {
        irq_disable();
        if (gs_ptnos_tcb_cur != &gs_idle_tcb)
        {
            tnos_sched_noral(delay_ms, TRUE);
            irq_enable();
        }
        else
        {
            irq_enable();

            ttimer_delay_ms(delay_ms);
        }
    }
}

/***********************************************************
 * 功能描述：信号初始化
 * 输入参数：psingal 信号结构体
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_singal_init(tnos_singal_t *psingal, u32 cnt)
{
    list_init(&psingal->list_wait_head);
    psingal->send_num = cnt;
}

/***********************************************************
 * 功能描述：清空信号,发送不会唤醒
 * 输入参数：psingal 信号结构体
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_singal_clean(tnos_singal_t *psingal)
{
    list_t *pnow, *pnext;

    psingal->send_num = 0;
    pnow = psingal->list_wait_head.next;

    while (pnow != &psingal->list_wait_head)
    {
        pnext = pnow->next;
        list_remove(pnow);
        pnow = pnext;
    }
}

/***********************************************************
 * 功能描述：发送信号 (需禁止中断才能进入)
 * 输入参数：psingal 信号结构体
 *          tick_delay 当前任务需要延迟的tick数
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_singal_send(tnos_singal_t *psingal)
{
    list_t *plist;

    tnos_singal_cnt_add(psingal);

    plist = psingal->list_wait_head.next;

    if (plist != &psingal->list_wait_head) //没有任务等待
    {
        tnos_tcb_t *ptcb = LIST_ENTRY(plist, tnos_singal_wait_t, list)->ptab;

        list_remove(plist);

        if (ptcb != &gs_idle_tcb)
        { //非当前任务,未在就绪队列中
            if (ptcb->list_run.next == NULL)
            {
                gs_is_signal_send = TRUE;
                list_remove(&ptcb->list_delay); //防止任务在定时队列中
                tnos_set_ready(ptcb, FALSE);
                tnos_sched_noral(0, FALSE);
            }
        }
    }
}


/***********************************************************
 * 功能描述：等待信号 (需禁止中断才能进入)
 * 输入参数：psingal 信号结构体
 *          timeout_tick 超时时间
 * 输出参数： 无
 * 返 回 值：超时, 或者 当前发送信号的个数
 ***********************************************************/
int tnos_singal_wait_ms(tnos_singal_t *psingal_in, u32 delay_ms)
{
    if ((delay_ms != 0) && (psingal_in->send_num == 0)) //没有延迟
    {
        if (gs_ptnos_tcb_cur != &gs_idle_tcb) //非空闲任务,调度
        {
            tnos_singal_wait_t singal_wait;

            singal_wait.ptab = gs_ptnos_tcb_cur;
            list_insert_before(&psingal_in->list_wait_head, &singal_wait.list);
            tnos_sched_noral(delay_ms, TRUE);
            irq_enable();

            tnos_irq_delay1();

            irq_disable();
            list_remove(&singal_wait.list);
        }
        else //空闲任务只能轮询
        {
            ttimer_t tm;

            ttimer_set(&tm, delay_ms);
            irq_enable();

            while (1)
            {
                if (ttimer_is_timeout(&tm))
                {
                    irq_disable();
                    break;
                }

                if (psingal_in->send_num != 0) //查看是否有信号发送
                {
                    irq_disable();
                    if (psingal_in->send_num != 0) //占用发送信号
                    {
                        break;
                    }
                    irq_enable();
                }
            }
        }
    }

    if (psingal_in->send_num != 0)
    {
        return psingal_in->send_num--;
    }

    return TNOS_ERR_TIMEOUT;
}

/***********************************************************
 * 功能描述：是否有信号发送
 * 输入参数：ppsingal_in 信号指针数组
 *          arary_num    数组的维数
 * 输出参数： 无
 * 返 回 值：  有信号的位置 >=0成功,   -1 失败
 ***********************************************************/
static s32 tnos_is_send(tnos_select_t *psel, u32 arary_num)
{
    register u32 i;

    for (i = 0; i < arary_num; psel++, i++)
    {
        if (psel->psingal->send_num != 0)
        {
            return 0;
        }
    }

    return TNOS_ERR_TIMEOUT;
}

/***********************************************************
 * 功能描述：监听多个信号(只检查信号有没有,不清空标志)
 * 输入参数：ppsingal_in 信号指针数组
 *          arary_num    数组的维数
 *          delay_ms     延迟时间
 * 输出参数： 无
 * 返 回 值：  有信号的位置 >=0成功,  其它失败, -1 超时,  -2:参数错误
 ***********************************************************/
s32 tnos_select(tnos_select_t *psel, u32 arary_num, u32 delay_ms)
{
    register tnos_select_t *p;
    s32 send_no;

#if (TNOS_DBG_ENABLE != 0)
    TNOS_ASSERT((psel != NULL) && ((u32)arary_num != 0));

    for (p = psel; p < psel + arary_num; p++)
    {
        TNOS_ASSERT(p->psingal != NULL);
    }
#endif

    irq_disable();
    send_no = tnos_is_send(psel, arary_num);

    if ((delay_ms != 0) && (send_no < 0)) //没有延迟
    {
        if (gs_ptnos_tcb_cur != &gs_idle_tcb) //非空闲任务,调度
        {
            u32 i;

            for (i = 0; i < arary_num; i++)
            {
                psel[i].singal_wait.ptab = gs_ptnos_tcb_cur;
                list_insert_before(&psel[i].psingal->list_wait_head, &psel[i].singal_wait.list);
            }

            tnos_sched_noral(delay_ms, TRUE);
            irq_enable();

            tnos_irq_delay1();

            irq_disable();
            for (i = 0; i < arary_num; i++)
            {
                list_remove(&psel[i].singal_wait.list);
            }

            send_no = tnos_is_send(psel, arary_num);
        }
        else //空闲任务只能轮询
        {
            ttimer_t tm;

            ttimer_set(&tm, delay_ms);
            irq_enable();

            while (1)
            {
                if (ttimer_is_timeout(&tm))
                {
                    irq_disable();
                    break;
                }

                if (send_no >= 0) //查看是否有信号发送
                {
                    irq_disable();
                    send_no = tnos_is_send(psel, arary_num);

                    if (send_no >= 0) //占用发送信号
                    {
                        break;
                    }
                    irq_enable();
                }
            }
        }
    }
    irq_enable();

    return send_no;
}

/***********************************************************
 * 功能描述：开中端后关中断给系统响应时间
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_irq_delay1(void)
{
    u32 i;

    for (i = 0; i < 8; i++)
    {
    }
}


/***********************************************************
 * 功能描述：任务返回(防止任务不为死循环退出引起错误)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_task_return(void)
{
//    printf("task return!!\n");

    while (1)
    {
        tnos_delay_ms(TIMER_S(1000));
    }
}


/***********************************************************
 * 功能描述：断言
 * 输入参数：pexct 输出字符串
 *          pfunc 函数名称
 *          line  行号
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_assert(const char* pexct, const char* pfunc, s32 line)
{
    DBG("err:[%s] assertion failed, func[%s],line[%d]\n", pexct, pfunc, line);

    while (1)
    {
        ;
    }
}
