/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_core.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ�����ļ�
 * �汾��ʷ:
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

#if (TNOS_STK_CHECK != 0)
static u32 gs_idle_sec;
#endif

tnos_tcb_t *gs_ptnos_tcb_cur, *gs_ptnos_tcb_ready; //��ǰ����tcb, ׼����������tcb


static u32 gs_idle_stk[TNOS_IDLE_STK_NUM]; //���������ջ
static tnos_tcb_t gs_idle_tcb;            //��������tcb

static list_t gs_list_head_delay; //�ȴ����ж���ͷ��
static list_t gs_list_rdy[2][TNOS_PRO_MAX]; //��������, �����ȴ�����
static list_t *gs_prdy, *gs_prdy_wait;     //��������ָ��, �����ȴ�����ָ��
static u8 gs_rdy_grop, gs_rdy_wait_grop;  //����������, �����ȴ�����ָ����
static volatile s16  gs_tnos_shed_lock_cnt; //�������Ĵ���

//����������ڵ�λ��
const static u8 gs_higth_rdy_tab[] =
{
     0xFF, 0, 1, 0,    2, 0, 1, 0,
     3, 0, 1, 0,       2, 0, 1, 0
};

/***********************************************************
 * �������������������ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void idle_task(void *parg)
{
#if (TNOS_STK_CHECK != 0)
    time_tick_t tick_ps, tick_ps_new;
#endif

    tnos_sched();

#if (TNOS_STK_CHECK != 0)
    get_time_tick(&tick_ps);
#endif

	while (1)
	{
#if (TNOS_STK_CHECK != 0)
	    u32 pass;
#endif

#if (TNOS_IDLE_HOOK != 0)
        tnos_idle_hook();
#endif

#if (TNOS_STK_CHECK != 0)
        MEMCPY2(tick_ps_new, &g_tick);
        pass = pass_ticks(&tick_ps, &tick_ps_new);

        if (pass >= TNOS_PS_PERIOD*1000) //�Ȳ�����,��ֹƵ������Ӱ��Ч��
        {
            u32 all = 0;
            u32 reg;

            reg = irq_disable();
            get_time_tick2(&tick_ps_new);
            pass = pass_ticks(&tick_ps, &tick_ps_new);

            if (pass >= TNOS_PS_PERIOD*1000)
            {
                tnos_tcb_t *ptcb;

                MEMCPY2(tick_ps, &tick_ps_new);
                ptcb = &gs_idle_tcb;

                while (1) //���������ѭ�����ж��������һ��tcb
                {
                    ptcb = ptcb->ptcb_next;

                    if (ptcb == &gs_idle_tcb)
                    {
                        break;
                    }

                    ptcb->ms_cpu_use = ptcb->ms_cpu_once;
                    ptcb->ms_cpu_once = 0;
                }

                all = pass;
            }
            irq_enable(reg);

            if (all != 0)
            {
                u32 use_less;
                tnos_tcb_t *ptcb;

                DBG("\r\n");

                use_less = 100;
                ptcb = &gs_idle_tcb;

                while (1)
                {
                    u32 use, stk_less;

                    ptcb = ptcb->ptcb_next;
                    stk_less = tnos_taks_stk_check(ptcb->stk_addr, ptcb->stk_size);

                    if (ptcb != &gs_idle_tcb)
                    {
                        use = ((u32)ptcb->ms_cpu_use*100)/all;

                        if (use_less > use)
                        {
                            use_less -= use;
                        }
                        else
                        {
                            use_less = 0;
                        }
                    }
                    else
                    {
                        use = use_less;
                    }

                    DBG("check:name[%s]\tcpu[%u]\t stk_less[%u]", ptcb->name, use, stk_less);

                    if (ptcb == &gs_idle_tcb)
                    {
                        break;
                    }
                }
            }
        }
#endif
    }
}


/***********************************************************
 * ����������������ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
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

    list_init(&gs_list_head_delay);
    gs_tnos_shed_lock_cnt = 0xFF; //�����������Ĵ���
    gs_ptnos_tcb_ready = gs_ptnos_tcb_cur = &gs_idle_tcb; //��ǰ���е�tcb

    tnos_task_create(&gs_idle_tcb, TNOS_TASK_IDLE_NAME, TNOS_PRO_LOW, 2, idle_task, NULL, gs_idle_stk, ARRAY_SIZE(gs_idle_stk));

#if (TNOS_STK_NAME != 0)
    tnos_task_set_name(&gs_idle_tcb, "idle");
#endif

#if (TNOS_STK_CHECK != 0)
    gs_idle_sec = 0;
#endif

}


/***********************************************************
 * ����������ϵͳ��ʼ����(���ú󲻷���!!!)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_run(void)
{
    irq_disable();
    tnos_tim_ms_set(100);
    gs_tnos_shed_lock_cnt = 0;
    tnos_start_rdy();
}

/***********************************************************
 * ����������tnos ��ӡϵͳ��Ϣ
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_printf(void)
{
    xprintf("\n\n");
    xprintf("RTOS  is  Operating System\n");
    xprintf("  %d.%d.%d\n", TNOS_VER_MAJOR, TNOS_VER_SUB, TNOS_VER_REVISE);
    xprintf(" 2017 - 2018 Copyright by rtos team\n");
}


/***********************************************************
 * ����������ϵͳ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_startup(void)
{
    tnos_board_sys_pro_init();

    tnos_val_init();

    xprintf_init(); //��ӡ��ʼ��

    tnos_printf();

    ttimer_init(); //��ʱ����ʼ��

    tnos_tim_ms_init(); //ϵͳms�ӳٳ�ʼ��

    tnos_board_other_init(); //����Ӳ����ʼ��

    tnos_app_init();

    tnos_run();
}


#if (TNOS_DBG_USE != 0)
/***********************************************************
 * ������������ӡ�����б�
 * ���������
 * ��������� ��
 * �� �� ֵ��  ��
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

#if 0
    for (u32 i = 0; i < TNOS_PRO_MAX; i++)
    {
        list_t *plist_head = gs_prdy[i];

        for (plist = plist_head->prev; plist != plist_head; plist = plist->prev)
        {
            ptcb = LIST_ENTRY(plist, tnos_tcb_t, list_run);
            TNOS_DBG(" name end[%u][%s]", i, ptcb->pname);
        }
    }
#endif
}
#endif



#if 0
/***********************************************************
 * �����������ҵ���С�ӳٳ�ʱʱ��  (�������������ȼ���,ʱ��С,�л������߳�,ʱ��Ƭ��������)
 * �����������
 * ��������� ��
 * �� �� ֵ��  �δ�ֵ
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
    else //��ʱΪ��
    {
        *pnext_time = mss;
    }
}
#endif

/***********************************************************
 * ������������������뵽��ʱ������
 * �����������
 * ��������� ��
 * �� �� ֵ��  �δ�ֵ
 ***********************************************************/
static void tnos_insert_delay(tnos_tcb_t *ptcb, u32 *pnext_time)
{
    list_t *plist;

    //��ʱ����С��������
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

    if (plist == gs_list_head_delay.next) //��ǰ����Ϊ�ӳ���̵�����
    {
        *pnext_time = ttimer_get_delay(&ptcb->tm);
    }

    list_insert_before(plist, &ptcb->list_delay);
}

/***********************************************************
 * �������� :ʱ��Ƭ�������ȴ��� (ֻ�е����������ʹ��, �����Ƴ� list_run)
 * ���������ptcb     �������
 *          pro       ���ȼ�
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_set_ready_wait(tnos_tcb_t *ptcb)
{
    TNOS_DBG("READ wait[%u] [%s]", ptcb->pro, ptcb->pname);

    gs_rdy_wait_grop |= BIT(ptcb->pro);
    ptcb->ms_less = ptcb->mss;
    list_insert_before(&gs_prdy_wait[ptcb->pro], &ptcb->list_run);
}


//���ڴ��� �ȴ�������,֮��������е������·���ʱ��Ƭ
/***********************************************************
 * ������������������׼������(�������ӳٶ�����)
 * ���������ptcb     �������
 *          pro       ���ȼ�
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_set_ready(tnos_tcb_t *ptcb, BOOL is_after)
{
    TNOS_DBG("READ[%u] [%s]", ptcb->pro, ptcb->pname);

    if (ptcb->list_run.next == NULL) //�ڶ����в���(��ֹgs_rdy_grop �����ȼ��жϲ���)
    {//��ֹ��׼�������б����·���ʱ��Ƭ
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
 * ��������������(�����ֹ�ж�) (gs_ptnos_tcb_cur->tick_slice = 0; ǿ�Ƶ���)
 * ���������tick_delay ��ǰ������Ҫ�ӳٵ�tick��
 * ��������� pnext_time ����з���һ����Ϊ0
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_do_sched(time_tick_t *ptick_now, u32 delay_ms, u32 *pnext_time)
{
    register tnos_tcb_t *pcur;
    register tnos_tcb_t *pnext; //��һ�����е�����

	if (gs_tnos_shed_lock_cnt > 0)
	{
		return;
	}

	pcur = gs_ptnos_tcb_cur;

	TNOS_DBG("");
    TNOS_DBG("cur name[%s],rb[%u]t[%u.%03u]", pcur->pname, gs_rdy_grop, ptick_now->s, ptick_now->ms);

	//��ǰ����Ϊ�ǿ�������űȽ�ʱ��
    //�����ȼ���ͬ���ȼ�
	//ʱ��Ƭ����: �����ж���Ϊ ��ʱ����ʱ��Ƭ
	//           ��������,�Ҳ������к����еȴ�������,����ʱ��Ƭ
	if (pcur != &gs_idle_tcb)
	{
	    u8 ms_less = ttimer_time_less2(&pcur->tm, ptick_now);

	    if (ms_less != 0)  //�����ȼ���ͬ���ȼ����Դ�ϵ����ȼ�
	    {
            if (pcur->pro < gs_higth_rdy_tab[gs_rdy_grop]) //���ܴ��ڿ��������
            {
                TNOS_DBG("low");
                return;
            }

            TNOS_DBG("higher");
	    }

	    list_remove(&pcur->list_delay); //��ֹ��ǰ�������ӳٶ�����

        if (delay_ms == 0)
        {
            if (ms_less != 0) //�������ȼ��ж�
            {
                TNOS_DBG("  cur ready");
                tnos_set_ready(pcur, TRUE);
                pcur->ms_less = ms_less;
            }
            else //ʱ��Ƭ����, ��ֹ�ź���,û����ʱ������ǰ�����ʱ��Ƭ
            {
                TNOS_DBG("  cur wait");
                if (pcur->list_run.next == NULL) //�������ж�����
                {
                    TNOS_DBG("  cur wait ok");
                    tnos_set_ready_wait(pcur);
                }
            }
        }
        else  //����Ҫ�ӳ�(�����),ֱ������Ϊ׼��
	    {//(��ʱ�ж�һ���������)
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

    		if (plist == plist_head) //�յ� (�������ᵽ����,��������BUG, gs_rdy_grop ����� list_run����ͬ��)
    		{
                gs_rdy_grop &= ~BIT(pos);
                TNOS_DBG("empty 1");
                continue;
    		}

#if (TNOS_STK_CHECK != 0)
            if (gs_idle_sec != g_tick.s) //ǿ�Ƹ���������
            {
                gs_idle_sec = g_tick.s;
                pnext = &gs_idle_tcb;  //û���κξ�������,��Ϊ��������
                *pnext_time = 2;
                break;
            }
#endif

    		if (plist->next == plist_head) //ֻ��һ��
    		{
                plist->next = NULL;     //�����Ƴ���ǰ����
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
            list_remove(&pnext->list_delay); //��ֹ��ǰ�������ӳٶ�����

            ms_less = pnext->ms_less;

            if (ms_less == 0) //��ʱ��Ƭ�ȴ����� (�����ܳ���,��������BUG)
            {
                TNOS_DBG("run_next = 0");
                tnos_set_ready_wait(pnext);
                continue;
            }

    		if (pcur != pnext) //��ͬ����ֱ�ӵ���
    		{
    			ttimer_set2(&pnext->tm, ms_less, ptick_now);
    			tnos_insert_delay(pnext, pnext_time);
    			break;
    		}

    		//ͬһ������,���� 1/2ʱ��Ƭ,�������1��ʱ��Ƭ
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
    		return;
        }

    	if (gs_rdy_wait_grop != 0)  //�ȴ����ж�����������,���·���ʱ��Ƭ
        {
    	    list_t *ptmp;

    	    gs_rdy_grop = gs_rdy_wait_grop;
    	    gs_rdy_wait_grop = 0;

    	    ptmp = gs_prdy; //�л�λ��
    	    gs_prdy = gs_prdy_wait;
    	    gs_prdy_wait = ptmp;
            TNOS_DBG("retime");
            continue;
        }

#if (TNOS_STK_CHECK != 0)
    	gs_idle_sec = g_tick.s;
#endif

        if (pcur == &gs_idle_tcb) //�����п���
        {
            return;
        }

        pnext = &gs_idle_tcb;  //û���κξ�������,��Ϊ��������
        break;
    }

    gs_ptnos_tcb_ready = pnext;

#if TNOS_STK_CHECK
    pcur->ms_cpu_once += pass_ticks(&pcur->tick_start, ptick_now);
    pnext->tick_start.s = ptick_now->s;
    pnext->tick_start.ms = ptick_now->ms;
#endif

    TNOS_DBG_LIST("shell out");

	TNOS_DBG("s %s to %s", pcur->pname, gs_ptnos_tcb_ready->pname);
	tnos_task_sw();
}


/***********************************************************
 * �����������δ�ʱ�����д���(�ж�����,�������ⲿ���ж�)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_tick_proess(void)
{
    u32 reg;
    list_t *plist;
    time_tick_t tick_now;
    u32 next_time = 1000;

    TNOS_DBG("t");

    reg = irq_disable();
    get_time_tick2(&tick_now);

    plist = gs_list_head_delay.next;

    while (plist != &gs_list_head_delay)    //��ʱ�����г�ʱ�ļ��뵽׼��������(��ʱ��˳���������)
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
        list_remove(plist);    //����׼�����е����

        plist = plist_next;

        if (!ptcb->is_timeout_del)
        {
            TNOS_DBG_LIST("time change in");

            //ttimer_set_timeout(&ptcb->tm);
            tnos_set_ready(ptcb, FALSE);

            TNOS_DBG("r[%s]", ptcb->pname);
            TNOS_DBG_LIST("time change out");
        }
        else
        {
            TNOS_DBG("r remove[%s]", ptcb->pname);
        }
    }

    tnos_do_sched(&tick_now, 0, &next_time);
    tnos_tim_ms_set(next_time);
    irq_enable(reg);


    TNOS_DBG("  n[%u]", next_time);
}


/***********************************************************
 * ����������������������
 * ���������ptcb     �������
 *           pname    ��������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_task_set_name(tnos_tcb_t *ptcb, const char *pname)
{
#if (TNOS_STK_NAME != 0)
    ptcb->pname = pname;
#endif
}

/***********************************************************
 * ������������������
 * ���������ptcb     �������
 *          pro  ��������ȼ�
 *          task     ��������
 *          parg     ���񴫵ݲ���
 *          stk_addr ��ջ��ʼ��ַ
 *          stk_size ��ջ��С
 * ��������� ��
 * �� �� ֵ��  0�ɹ�, ����ʧ��
 ***********************************************************/
s32 tnos_task_create(tnos_tcb_t *ptcb, const char *pname, tnos_pro_t pro, u8 mss,
                     ptnos_task task, void *parg, u32 *stk_addr, u16 stk_size)
{
    u32 reg;

    TNOS_ASSERT(((ptcb != NULL) && (pname != NULL) && ((u32)pro < TNOS_PRO_MAX) && (mss > 0)
                 && (task != NULL) && (stk_addr != NULL) && (stk_size > 32)));

    reg = irq_disable();
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

            if (ptcb_f == ptcb) //ͬһ������ջ��ֵΪ2������
            {
                TNOS_ASSERT(ptcb_f == ptcb);
                irq_enable(reg);

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

    irq_enable(reg);

    return TNOS_ERR_NONE;
}

/***********************************************************
 * ������������ȡ��ǰ��������
 * �����������
 * ��������� ��
 * �� �� ֵ��  ����
 ***********************************************************/
const char* tnos_name(void)
{
	return gs_ptnos_tcb_cur->name;
}


/***********************************************************
 * ���������������������ȼ�
 * ���������ptcb     �������
 *        pro      ���ȼ�
 * ��������� ��
 * �� �� ֵ��  0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_set_pro(tnos_tcb_t *ptcb, tnos_pro_t pro)
{
    u32 reg;

    TNOS_ASSERT((ptcb != NULL) && ((u32)pro < TNOS_PRO_MAX));

    reg = irq_disable();
	ptcb->pro = pro; //�����ֹ�ж�!!!
    irq_enable(reg);

	return TNOS_ERR_NONE;
}


/***********************************************************
 * ��������������ʱ��Ƭ
 * ���������ptcb     �������
 *        ms       ʱ��Ƭʱ��,��λms
 * ��������� ��
 * �� �� ֵ�� 0:�ɹ� ����:ʧ��
 ***********************************************************/
s32 tnos_set_mss(tnos_tcb_t *ptcb, u8 ms)
{
    u32 reg;

    TNOS_ASSERT(ptcb != NULL);

	if (ms == 0)
	{
		ms = 1;
	}

    reg = irq_disable();
	ptcb->mss = ms; //�����ֹ�ж�!!!
    irq_enable(reg);

	return TNOS_ERR_NONE;
}

/***********************************************************
 * �������������жϵ��õ���
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
static void tnos_sched_noral(u32 delay_ms, BOOL set_timeout)
{
    time_tick_t tick_now;
    u32 next_time = 0;

    if (set_timeout)
    {
        ttimer_set_timeout(&gs_ptnos_tcb_cur->tm);
    }

    get_time_tick2(&tick_now);
    tnos_do_sched(&tick_now, delay_ms, &next_time);

    if (next_time != 0)
    {
        tnos_tim_ms_set(next_time);
    }
}

/***********************************************************
 * ��������������
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_sched(void)
{
    u32 reg;

    reg = irq_disable();
    tnos_sched_noral(0, TRUE);
    irq_enable(reg);

    tnos_irq_delay();
}

/***********************************************************
 * ���������� �����жϺ��� (����tnos��غ������ж���Ҫд)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_interrupt_enter(void)
{
    tnos_sched_lock();
}

/***********************************************************
 * ���������� �����˳��жϺ��� (����tnos��غ������ж���Ҫд)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_interrupt_exit(void)
{
    u32 reg;

    reg = irq_disable();
    if (--gs_tnos_shed_lock_cnt <= 0)
    {
        time_tick_t tick_now;
        u32 next_time;

        gs_tnos_shed_lock_cnt = 0;
        next_time = 0;
        get_time_tick2(&tick_now);
        tnos_do_sched(&tick_now, 0, &next_time);

        if (next_time != 0)
        {
            tnos_tim_ms_set(next_time);
        }
    }
    irq_enable(reg);
}


/***********************************************************
 * �������������� ��ֹ
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_sched_lock(void)
{
    u32 reg;

    reg = irq_disable();
    gs_tnos_shed_lock_cnt++;
    irq_enable(reg);
}

/***********************************************************
 * �������������� ����
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_sched_unlock(void)
{
    u32 reg;

    reg = irq_disable();
    if (--gs_tnos_shed_lock_cnt <= 0)
    {
        gs_tnos_shed_lock_cnt = 0;
        tnos_sched_noral(0, FALSE);
    }
    irq_enable(reg);
}

/***********************************************************
 * �����������ȴ�msʱ��
 * ���������delay_ms  msʱ��
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_delay_ms(u32 delay_ms)
{
    if (delay_ms != 0)
    {
        u32 reg;

        reg = irq_disable();
        if (gs_ptnos_tcb_cur != &gs_idle_tcb)
        {
            tnos_sched_noral(delay_ms, TRUE);
            irq_enable(reg);
        }
        else
        {
            irq_enable(reg);

            ttimer_delay_ms(delay_ms);
        }
    }
}

/***********************************************************
 * �����������źų�ʼ��
 * ���������psingal �źŽṹ��
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_singal_init(tnos_singal_t *psingal, u32 cnt)
{
    list_init(&psingal->list_wait_head);
    psingal->send_num = cnt;
}

/***********************************************************
 * ���������������ź� (���ֹ�жϲ��ܽ���)
 * ���������psingal �źŽṹ��
 *          tick_delay ��ǰ������Ҫ�ӳٵ�tick��
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_singal_send(tnos_singal_t *psingal)
{
    list_t *plist;

    if ((++psingal->send_num)>>31) //��ֹ���
    {
        psingal->send_num = 0x80000000 - 1;
    }

    plist = psingal->list_wait_head.next;

    if (plist != &psingal->list_wait_head) //û������ȴ�
    {
        tnos_singal_wait_t *pwait;
        tnos_tcb_t *ptcb;

        list_remove(plist);

        pwait = LIST_ENTRY(plist, tnos_singal_wait_t, list);
        ptcb = pwait->ptab;

        //��ǰ����Ϳ���������������л�
        if ((ptcb != gs_ptnos_tcb_cur) && (ptcb != &gs_idle_tcb))
        {
            list_remove(&ptcb->list_delay); //��ֹ�����ڶ�ʱ������
            tnos_set_ready(ptcb, FALSE);
            tnos_sched_noral(0, FALSE);
        }
    }
}


/***********************************************************
 * �����������ȴ��ź� (���ֹ�жϲ��ܽ���)
 * ���������psingal �źŽṹ��
 *          timeout_tick ��ʱʱ��
 * ��������� ��
 * �� �� ֵ����ʱ, ���� ��ǰ�����źŵĸ���
 ***********************************************************/
int tnos_singal_wait_ms(tnos_singal_t *psingal_in, u32 delay_ms, u32 *preg)
{
    if ((delay_ms != 0) && (psingal_in->send_num == 0)) //û���ӳ�
    {
        if (gs_ptnos_tcb_cur != &gs_idle_tcb) //�ǿ�������,����
        {
            tnos_singal_wait_t singal_wait;

            singal_wait.ptab = gs_ptnos_tcb_cur;
            list_insert_before(&psingal_in->list_wait_head, &singal_wait.list);
            tnos_sched_noral(delay_ms, TRUE);
            irq_enable(*preg);

            tnos_irq_delay1();

            *preg = irq_disable();
            list_remove(&singal_wait.list);
        }
        else //��������ֻ����ѯ
        {
            ttimer_t tm;

            ttimer_set(&tm, delay_ms);
            irq_enable(*preg);

            while (1)
            {
                if (ttimer_is_timeout(&tm))
                {
                    *preg = irq_disable();
                    break;
                }

                if (psingal_in->send_num != 0) //�鿴�Ƿ����źŷ���
                {
                    *preg = irq_disable();
                    if (psingal_in->send_num != 0) //ռ�÷����ź�
                    {
                        break;
                    }
                    irq_enable(*preg);
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
 * �����������Ƿ����źŷ���
 * ���������ppsingal_in �ź�ָ������
 *          arary_num    �����ά��
 * ��������� ��
 * �� �� ֵ��  ���źŵ�λ�� >=0�ɹ�,   -1 ʧ��
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
 * ������������������ź�(ֻ����ź���û��,����ձ�־)
 * ���������ppsingal_in �ź�ָ������
 *          arary_num    �����ά��
 *          delay_ms     �ӳ�ʱ��
 * ��������� ��
 * �� �� ֵ��  ���źŵ�λ�� >=0�ɹ�,  ����ʧ��, -1 ��ʱ,  -2:��������
 ***********************************************************/
s32 tnos_select(tnos_select_t *psel, u32 arary_num, u32 delay_ms)
{
    register tnos_select_t *p;
    s32 send_no;
    u32 reg;

#if (TNOS_DBG_ENABLE != 0)
    TNOS_ASSERT((psel != NULL) && ((u32)arary_num != 0));

    for (p = psel; p < psel + arary_num; p++)
    {
        TNOS_ASSERT(p->psingal != NULL);
    }
#endif

    reg = irq_disable();
    send_no = tnos_is_send(psel, arary_num);

    if ((delay_ms != 0) && (send_no < 0)) //û���ӳ�
    {
        if (gs_ptnos_tcb_cur != &gs_idle_tcb) //�ǿ�������,����
        {
            u32 i;

            for (i = 0; i < arary_num; i++)
            {
                psel[i].singal_wait.ptab = gs_ptnos_tcb_cur;
                list_insert_before(&psel[i].psingal->list_wait_head, &psel[i].singal_wait.list);
            }

            tnos_sched_noral(delay_ms, TRUE);
            irq_enable(reg);

            tnos_irq_delay1();

            reg = irq_disable();
            for (i = 0; i < arary_num; i++)
            {
                list_remove(&psel[i].singal_wait.list);
            }

            send_no = tnos_is_send(psel, arary_num);
        }
        else //��������ֻ����ѯ
        {
            ttimer_t tm;

            ttimer_set(&tm, delay_ms);
            irq_enable(reg);

            while (1)
            {
                if (ttimer_is_timeout(&tm))
                {
                    reg = irq_disable();
                    break;
                }

                if (send_no >= 0) //�鿴�Ƿ����źŷ���
                {
                    reg = irq_disable();
                    send_no = tnos_is_send(psel, arary_num);

                    if (send_no >= 0) //ռ�÷����ź�
                    {
                        break;
                    }
                    irq_enable(reg);
                }
            }
        }
    }
    irq_enable(reg);

    return send_no;
}

/***********************************************************
 * �������������ж˺���жϸ�ϵͳ��Ӧʱ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_irq_delay1(void)
{
    u32 i;

    for (i = 0; i < 8; i++)
    {
    }
}


/***********************************************************
 * �������������񷵻�(��ֹ����Ϊ��ѭ���˳��������)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
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
 * ��������������
 * ���������pexct ����ַ���
 *          pfunc ��������
 *          line  �к�
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_assert(const char* pexct, const char* pfunc, s32 line)
{
    DBG("err:[%s] assertion failed, func[%s],line[%d]\n", pexct, pfunc, line);

    while (1)
    {
        ;
    }
}
