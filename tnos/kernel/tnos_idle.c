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



/***********************************************************
 * �������������������ʼ��
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void idle_task(void *parg)
{
#if (TNOS_STK_CHECK != 0)
    time_tick_t tick_ps, tick_ps_new;

    get_time_tick(&tick_ps);
#endif

    tnos_sched();

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

            irq_disable();
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
            irq_enable();

            if (all != 0)
            {
                u32 use_less;
                tnos_tcb_t *ptcb;

                tnos_sched_lock();

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
                tnos_sched_unlock();
            }
        }
#endif
    }
}
