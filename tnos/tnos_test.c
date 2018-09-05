/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_test.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能:测试
 * 版本历史:
 ***********************************************************/

#include "xutils.h"
#include "xtimer.h"
#include "xprintf.h"
#include "system_init.h"
#include "tnos.h"



#define TNOS_SLICE_MS                 20  //每个任务最长运行的时间,(单位ms, 最大255)


#if 0 //测试任务切换是否正常,堆栈


static tnos_tcb_t tcb_main;
static u32 stk_main[128];

static tnos_tcb_t tcb_m;
static u32 stk_m[128];


static tnos_tcb_t tcb_m2;
static u32 stk_m2[128];

#include "tnos_cpu_c.h"

uint32_t set_r6(u32 val)
{
 register uint32_t __regAPSR          __ASM("r6");
 __regAPSR = val;
}

void te1(void)
{
    u32 i = 0;

    u32 reg;
    reg = irq_disable();
    for (i = 0; i < 100; i++); {};

    for (i = 0; i < 100; i++); {};
    irq_enable(reg) ;
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main(void *p_arg)
{

    {
        u32 i = 0;

        u32 reg;
        reg = irq_disable();

        OS_TASK_SW();
        for (i = 0; i < 100; i++); {};

        te1();

        for (i = 0; i < 100; i++); {};
        irq_enable(reg) ;

    }

    while (1)
    {
        u32 a0, a1, a2, a3, a4, a5, a6, a7, a8 ,b0;

        a7 = 0x10101010;
        a6 = 0x20202020;
        a5 = 0x30303030;
        a4 = 0x40404040;

        a3 = 0x50505050;
        a2 = 0x60606060;
        a1 = 0x70707070;
        a0 = 0x80808080;

        u32 msp;
        msp = __get_MSP();

        set_r6(0x12345678);

        DBG("A[0X%08X\r\n", msp);

//        tnos_delay_ms(100);
        tnos_sched();
        msp = __get_MSP();
        DBG("B[0X%08X]", msp);
        b0 = 1;
        ++a0;
        ++a1;
        ++a2;
        ++a3;
        ++a4;
        ++a5;
        ++a6;
        ++a7;
        ++a8;

        ttimer_delay_ms(1000);

    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m(void *p_arg)
{
    while (1)
    {
        u32 a0, a1, a2, a3, a4, a5, a6, a7, a8 ,b0;

        a0 = 0x10101010;
        a1 = 0x20202020;
        a2 = 0x30303030;
        a3 = 0x40404040;

        a4 = 0x50505050;
        a5 = 0x60606060;
        a6 = 0x70707070;
        a7 = 0x80808080;

        u32 msp;
        msp = __get_MSP();
        DBG("a[0X%08X\r\n", msp);
//        tnos_delay_ms(100);
                tnos_sched();
        msp = __get_MSP();
        DBG("b[0X%08X]", msp);
        b0 = 1;
        ++a0;
        ++a1;
        ++a2;
        ++a3;
        ++a4;
        ++a5;
        ++a6;
        ++a7;
        ++a8;
        ttimer_delay_ms(1000);
    }
}

void tnos_app_init(void)
{
    tnos_task_create(&tcb_main,  "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)0, stk_main, ARRAY_SIZE(stk_main));
    tnos_task_create(&tcb_m,  "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task_m, (void *)1, stk_m, ARRAY_SIZE(stk_m));
}
#endif

#if 0//测试定时运行是否正常
static tnos_tcb_t tcb_main;
static u32 stk_main[128];

static tnos_tcb_t tcb_m;
static u32 stk_m[128];


static tnos_tcb_t tcb_m2;
static u32 stk_m2[128];


/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main(void *p_arg)
{
    time_tick_t tick;
    u32 pass;


    get_time_tick(&tick);

    while (1)
    {
        u32 delay;

        delay = 150;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("A[%u]", pass);

        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 12;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("B[%u]", pass);

        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }
    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m(void *p_arg)
{
    time_tick_t tick;
    u32 pass;

    get_time_tick(&tick);

    while (1)
    {
        u32 delay;

        delay = 200;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("C[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 1;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("D[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 1;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("E[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 1;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("F[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }
    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m2(void *p_arg)
{
    time_tick_t tick;
    u32 pass;

    get_time_tick(&tick);

    while (1)
    {
        u32 delay;

        delay = 120;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("I[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 3;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("G[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 4;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("K[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }

        delay = 5;
        tnos_delay_ms(delay);
        pass = pass_ticks_now(&tick, &tick);
        DBG("M[%u]", pass);
        if (pass > delay + 2)
        {
            DBG("EERR");
            while (1)
            {
            }
        }
    }
}

/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    tnos_task_create(&tcb_main, "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)0, stk_main, ARRAY_SIZE(stk_main));
    tnos_task_create(&tcb_m,  "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task_m, (void *)1, stk_m, ARRAY_SIZE(stk_m));
    tnos_task_create(&tcb_m2, "TK3", TNOS_PRO_MID, TNOS_SLICE_MS, task_m2, (void *)1, stk_m2, ARRAY_SIZE(stk_m2));
}
#endif


#if 0 //测试统计任务是否正常


static tnos_tcb_t tcb_main;
static u32 stk_main[128];

static tnos_tcb_t tcb_m;
static u32 stk_m[128];


static tnos_tcb_t tcb_m2;
static u32 stk_m2[128];

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main(void *p_arg)
{
    while (1)
    {
//        ttimer_delay_ms(100);
        //DBG("1");
        ttimer_delay_ms(50);
        tnos_delay_ms(50);
    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m(void *p_arg)
{
    while (1)
    {
        //DBG("2");
        ttimer_delay_ms(100);
        tnos_delay_ms(50);
    }
}

/***********************************************************
 * 函数名称：task_m2
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m2(void *p_arg)
{
    while (1)
    {
        //DBG("3");
        ttimer_delay_ms(200);
        tnos_delay_ms(50);
    }
}

void tnos_app_init(void)
{
    tnos_task_create(&tcb_main, "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)0, stk_main, ARRAY_SIZE(stk_main));
    tnos_task_create(&tcb_m, "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task_m, (void *)1, stk_m, ARRAY_SIZE(stk_m));
    tnos_task_create(&tcb_m2, "TK3", TNOS_PRO_MID, TNOS_SLICE_MS, task_m2, (void *)2, stk_m2, ARRAY_SIZE(stk_m2));
}
#endif


#if 0 //测试时间片是否正常
static tnos_tcb_t tcb_main;
static u32 stk_main[128];

static tnos_tcb_t tcb_m;
static u32 stk_m[128];


static tnos_tcb_t tcb_m2;
static u32 stk_m2[128];


/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main(void *p_arg)
{
    time_tick_t tick;
    u32 pass;
    u32 num = (u32)p_arg;

    get_time_tick(&tick);

    while (1)
    {
        DBG("tsk[%u] M\r\n", num);
        ttimer_delay_ms(TNOS_SLICE_MS);
    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main1(void *p_arg)
{
    time_tick_t tick;
    u32 pass;
    u32 num = (u32)p_arg;

    get_time_tick(&tick);

    while (1)
    {
        DBG("tsk[%u] M\r\n", num);
        ttimer_delay_ms(TNOS_SLICE_MS/2);
        tnos_delay_ms(TNOS_SLICE_MS + TNOS_SLICE_MS/2);
    }
}


/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    tnos_task_create(&tcb_main, "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)0, stk_main, ARRAY_SIZE(stk_main));
    tnos_task_create(&tcb_m, "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)1, stk_m, ARRAY_SIZE(stk_m));
    tnos_task_create(&tcb_m2, "TK3", TNOS_PRO_HIGH, TNOS_SLICE_MS, task_main1, (void *)3, stk_m2, ARRAY_SIZE(stk_m2));
}
#endif

#if 0
static tnos_tcb_t tcb[4];
static u32 stk[ARRAY_SIZE(tcb)][256];
static tnos_mutex_t mutex;
static u32 sw = 0;
static u32 sw_cnt = 0;

#define TIME_DELAY   900

#include "xtimer.h"

/***********************************************************
 * 功能描述：任务1
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task1(void *p_arg)
{
    u32 num = (u32)p_arg;
    time_tick_t tick;

    num = num++;
    num = --num;

    while (1)
    {
        u32 pass;

        DBG("tsk[%u] lock\r\n", num);
        tnos_mutex_lock(&mutex);

        get_time_tick(&tick);
        DBG("tsk[%u] lock[%u.%03u]", num, tick.s, tick.ms);
        tnos_delay_ms(TIME_DELAY);
        pass = pass_ticks_now(&tick, NULL);

        if (pass > TIME_DELAY + 1)
        {
            DBG("err pass[%u]\r\n", pass);
        }

        ++sw_cnt;

//        if (sw_cnt %1000 == 0)
//        {
//            DBG("sw_cnt");
//        }

        DBG("tsk[%u] sw_cnt[%d]\r\n", num, sw_cnt);
        get_time_tick(&tick);
        DBG("tsk[%u] unlock[%u.%03u]", num, tick.s, tick.ms);
        tnos_mutex_unlock(&mutex);
        tnos_delay_ms(TIME_DELAY);
    }
}


/***********************************************************
 * 功能描述：任务2
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task2(void *p_arg)
{
    time_tick_t tick;

    while (1)
    {
        tnos_mutex_lock(&mutex);

        get_time_tick(&tick);
        DBG("tsk2 lock[%u.%03u]\r\n", tick.s, tick.ms);
        tnos_delay_ms(1000);
        get_time_tick(&tick);
        DBG("tsk2 unlock[%u.%03u]\r\n", tick.s, tick.ms);
        tnos_mutex_unlock(&mutex);
    }
}


/***********************************************************
 * 功能描述：系统开始运行
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    tnos_mutex_init(&mutex);
    tnos_task_create(&tcb[0], "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)0, stk[0], ARRAY_SIZE(stk[0]));
    tnos_task_create(&tcb[1], "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)1, stk[1], ARRAY_SIZE(stk[1]));
//    tnos_task_create(&tcb[2],  "TK3", TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)2, stk[2], ARRAY_SIZE(stk[2]));
//    tnos_task_create(&tcb[3], "TK4", TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)3, stk[3], ARRAY_SIZE(stk[3]));
}

#endif


//测试信号量 切换速度 2个任务
#if 0

static tnos_sem_t sem[2];
static tnos_tcb_t tcb[4];
static u32 stk[ARRAY_SIZE(tcb)][128];
static u32 sw = 0;


/***********************************************************
 * 功能描述：任务1
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task1(void *p_arg)
{
    ttimer_t tm;
    u32 swa;

    ttimer_set(&tm, TIMER_S(1));

    while (1)
    {
        tnos_sem_post(&sem[0]);
        if (tnos_sem_wait(&sem[1], 1000) <= 0)
        {
            DBG("$");
        }

        if (ttimer_is_timeout(&tm))
        {
            ttimer_repeat(&tm);
            swa = 0;
        }

//        tnos_delay_ms(100);
    }
}

/***********************************************************
 * 功能描述：任务1
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task2(void *p_arg)
{
    ttimer_t tm;

    ttimer_set(&tm, TIMER_S(1));

    while (1)
    {
//        tnos_sem_post(&sem[1]);
        if (tnos_sem_wait(&sem[0], 1000) <= 0)
        {
            DBG("ERR!");
            while(1)
            {
            }
        }

        ++sw;

        if (ttimer_is_timeout(&tm))
        {
            ttimer_repeat(&tm);
            DBG("sw[%u]\r\n", sw);
            sw = 0;
        }

        tnos_sem_post(&sem[1]);
    }
}


/***********************************************************
 * 功能描述：信号量测试
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    tnos_sem_init(&sem[0], 0);
    tnos_sem_init(&sem[1], 0);
    tnos_task_create(&tcb[0], "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)0, stk[0], ARRAY_SIZE(stk[0]));
    tnos_task_create(&tcb[1], "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task2, (void *)1, stk[1], ARRAY_SIZE(stk[1]));
}

#endif


//测试信号量 切换速度  多个
#if 1

static tnos_tcb_t tcb[31];
static tnos_sem_t sem[ARRAY_SIZE(tcb)];
static u32 stk[ARRAY_SIZE(tcb)][128];
static u32 sw = 0;
static u32 gs_delay_tab[ARRAY_SIZE(tcb) + 1];
static u32 gs_wait_tab[ARRAY_SIZE(tcb) + 1];

static u32 gs_cnt = 0;
static u32 gs_cnt_all = 0;

/***********************************************************
 * 功能描述：任务1
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task1(void *p_arg)
{
    ttimer_t tm_wait;
    ttimer_t tm;
    ttimer_set(&tm, TIMER_S(1));

    u8 pos = (u32)p_arg;
    u8 sem_pos = pos;

    if (++sem_pos >= ARRAY_SIZE(tcb))
    {
        sem_pos = 0;
    }

    while (1)
    {
         if (tnos_sem_wait(&sem[pos], gs_wait_tab[pos]) <= 0)
         {
             DBG("err: pos[%u],cnt[%u],all[%u]", pos, gs_cnt, gs_cnt_all);
             while (1)
             {
             }
         }

         ++gs_cnt;
         ++gs_cnt_all;

         if (pos == 0)
         {
             if (ttimer_is_timeout(&tm))
             {
                 u32 cnt = gs_cnt;
                 ttimer_repeat_abs(&tm);

                 gs_cnt = 0;
                 DBG("cnt: [%u],[%u]", cnt, gs_cnt_all);
             }
         }
         tnos_sem_post(&sem[sem_pos]);
    }

}

/***********************************************************
 * 功能描述：信号量测试
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    for (u32 i = 0; i < ARRAY_SIZE(tcb); i++)
    {
        char name[20];
        XSNPRINTF(name, "TK%u", i);
        tnos_task_create(&tcb[i], name, TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)i, stk[i], ARRAY_SIZE(stk[i]));

        if (i < ARRAY_SIZE(tcb)/2)
        {
            gs_delay_tab[i] = 3 + i;
            gs_wait_tab[i] = 500 + i;
        }
        else
        {
            gs_delay_tab[i] = 3  + ARRAY_SIZE(tcb) - i;
            gs_wait_tab[i] = 500 + ARRAY_SIZE(tcb) - i;
        }

        tnos_sem_init(&sem[i], 0);
    }

    tnos_sem_post(&sem[0]);
}

#endif


//测试信号量
#if 0

static tnos_tcb_t tcb[31];
static tnos_sem_t sem[ARRAY_SIZE(tcb)];
static u32 stk[ARRAY_SIZE(tcb)][128];
static u32 sw = 0;
static u32 gs_delay_tab[ARRAY_SIZE(tcb) + 1];
static u32 gs_wait_tab[ARRAY_SIZE(tcb) + 1];

static u32 gs_cnt = 0;
static u32 gs_cnt_all = 0;

/***********************************************************
 * 功能描述：任务1
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task1(void *p_arg)
{
    ttimer_t tm_wait;
    ttimer_t tm;
    ttimer_set(&tm, TIMER_S(5));

    u8 pos = (u32)p_arg;
    u8 sem_pos = pos;

    if (++sem_pos >= ARRAY_SIZE(tcb))
    {
        sem_pos = 0;
    }

    while (1)
    {
        if (pos == 0)
        {
            tnos_delay_ms(10);
        }

         if (tnos_sem_wait(&sem[pos], gs_wait_tab[pos]) <= 0)
         {
             DBG("err: pos[%u],cnt[%u],all[%u]", pos, gs_cnt, gs_cnt_all);
             while (1)
             {
             }
         }

         ++gs_cnt;
         ++gs_cnt_all;
         ttimer_set(&tm_wait, gs_delay_tab[pos]);

         if (pos == 0)
         {
             if (ttimer_is_timeout(&tm))
             {
                 u32 cnt = gs_cnt;
                 ttimer_repeat_abs(&tm);

                 gs_cnt = 0;
                 DBG("cnt: [%u],[%u]", cnt, gs_cnt_all);
             }
         }

         ttimer_wait_timeout(&tm_wait); //消耗时间片
         tnos_sem_post(&sem[sem_pos]);
    }

}

/***********************************************************
 * 功能描述：信号量测试
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_app_init(void)
{
    for (u32 i = 0; i < ARRAY_SIZE(tcb); i++)
    {
        char name[20];
        XSNPRINTF(name, "TK%u", i);
        tnos_task_create(&tcb[i], name, TNOS_PRO_MID, TNOS_SLICE_MS, task1, (void *)i, stk[i], ARRAY_SIZE(stk[i]));

        if (i < ARRAY_SIZE(tcb)/2)
        {
            gs_delay_tab[i] = 3 + i;
            gs_wait_tab[i] = 500 + i;
        }
        else
        {
            gs_delay_tab[i] = 3  + ARRAY_SIZE(tcb) - i;
            gs_wait_tab[i] = 500 + ARRAY_SIZE(tcb) - i;
        }

        tnos_sem_init(&sem[i], 0);
    }
}

#endif


//测试消息队列
#if 0

static tnos_tcb_t tcb_main;
static u32 stk_main[128];

static tnos_tcb_t tcb_m;
static u32 stk_m[128];

static tnos_msgq_t msg_q;
static u32 gs_buf[5*3];


/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_main(void *p_arg)
{
    typedef struct
    {
        u8 num;
        u32 val;
    }ts_t;

    static ts_t tw, tr, *pw, *pr;
    static char buf[100];
    u32 start_num;
    u32 i;

    start_num = tw.num = 0;
#if 1

    for (i = 0; i < 2; i++)
    {
        if (tnos_msgq_send(&msg_q, &tw, sizeof(tw)) < 0)
        {
            DBG("w err  num[%u]", tw.num);
        }

        DBG("w num[%u]", tw.num);
        ++tw.num;
    }

    for (i = 0; i < 2; i++)
    {
        if (tnos_msgq_rev(&msg_q, &tr, sizeof(tr), 0) <= 0)
        {
            DBG("r err num[%u]", i);
            while (1)
            {
            }
        }

        DBG("r num[%u]", tr.num);

        if (tr.num != start_num++)
        {
            DBG("r err val, num [%u] val[%u]", i, tr.num);
            while (1)
            {
            }
        }
    }

    if (tnos_msgq_rev(&msg_q, &tr, sizeof(tr), 0) > 0)
    {
        DBG("err end0 num[%u]", tr.num);
        while (1)
        {
        }
    }

#endif


#if 1
    start_num = tw.num;

    for (i = 0; i < 3; i++)
    {
        if (tnos_msgq_send(&msg_q, &tw, sizeof(tw)) < 0)
        {
            DBG("w err  num[%u]", tw.num);
        }

        DBG("w num[%u]", tw.num);
        ++tw.num;
    }

    for (i = 0; i < 3; i++)
    {
        if (tnos_msgq_rev(&msg_q, &tr, sizeof(tr), 0) <= 0)
        {
            DBG("r err num[%u]", i);
            while (1)
            {
            }
        }

        DBG("r num[%u]", tr.num);

        if (tr.num != start_num++)
        {
            DBG("r err val, num [%u] val[%u]", i, tr.num);
            while (1)
            {
            }
        }
    }

    if (tnos_msgq_rev(&msg_q, &tr, sizeof(tr), 0) > 0)
    {
        DBG("err end1 num[%u]", tr.num);
        while (1)
        {
        }
    }


#endif



#if 1
    start_num = tw.num;

    for (i = 0; i < 3; i++)
    {
        if (tnos_msgq_send(&msg_q, &tw, sizeof(tw)) < 0)
        {
            DBG("w err  num[%u]", tw.num);
        }

        DBG("w num[%u]", tw.num);
        ++tw.num;
    }

    for (i = 0; i < 3; i++)
    {
        tnos_msgq_data_t *pmsg_data;

        s32 len = tnos_msgq_rev_ptr(&msg_q, &pmsg_data, 0);

        if (len <= 0)
        {
            DBG("r err num[%u]", i);
            while (1)
            {
            }
        }

        if (len < sizeof(tw))
        {
            tnos_msgq_rev_ptr_next(&msg_q, pmsg_data);
            DBG("r err len num[%u]", i);
            while (1)
            {
            }
        }

        pr = TNOS_MSGQ_GET_BUF(ts_t *, pmsg_data);

        DBG("r num[%u]", pr->num);

        if (pr->num != start_num++)
        {
            tnos_msgq_rev_ptr_next(&msg_q, pmsg_data);
            DBG("r err val, num [%u] val[%u]", i, tr.num);
            while (1)
            {
            }
        }
        else
        {
            tnos_msgq_rev_ptr_next(&msg_q, pmsg_data);
        }
    }

    if (tnos_msgq_rev(&msg_q, &tr, sizeof(tr), 0) > 0)
    {
        DBG("err end1 num[%u]", tr.num);
        while (1)
        {
        }
    }


#endif



    while (1)
    {
        tnos_delay_ms(50);
    }
}

/***********************************************************
 * 函数名称：task_main
 * 功能描述：主任务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
static void task_m(void *p_arg)
{
    while (1)
    {
        //DBG("2");
//        ttimer_delay_ms(100);
        tnos_delay_ms(50);
    }
}


void tnos_app_init(void)
{
    tnos_msgq_init(&msg_q, 16, gs_buf, sizeof(gs_buf));
    tnos_task_create(&tcb_main, "TK1", TNOS_PRO_MID, TNOS_SLICE_MS, task_main, (void *)0, stk_main, ARRAY_SIZE(stk_main));
    tnos_task_create(&tcb_m, "TK2", TNOS_PRO_MID, TNOS_SLICE_MS, task_m, (void *)1, stk_m, ARRAY_SIZE(stk_m));
//    tnos_task_create(&tcb_m2, "TK3", TNOS_PRO_MID, TNOS_SLICE_MS, task_m2, (void *)2, stk_m2, ARRAY_SIZE(stk_m2));
}
#endif
