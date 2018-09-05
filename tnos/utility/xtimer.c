/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: xtimer.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ��ʱ����װ
 * �汾��ʷ:
 ***********************************************************/


#include "xtimer.h"

time_tick_t g_tick = {0};



/***********************************************************
 * �����������Ƚ�ʱ���С
 * ���������tnew �µ�ʱ��
 *          told  �ɵ�ʱ��
 * ��������� ��
 * �� �� ֵ��  1: tnew > told     0 : tnew = told    -1: tnew < told
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
 * �����������ӳ�msʱ��
 * ��������� �ȴ�msʱ��
 * ���������  ��
 * �� �� ֵ��  ��
 ***********************************************************/
void ttimer_delay_ms(u32 ms)
{
    ttimer_t tm;

    ttimer_set(&tm, ms);
    ttimer_wait_timeout(&tm);
}



/***********************************************************
 * ������������ѯ�Ƿ��Ѿ���ʱ
 * ��������� ptick_now ʱ��(�Ƚϵ�ʱ��)(����ΪNULL)
 * ��������� ��
 * �� �� ֵ��  FALSE ��, ������ʱ (ֹͣ�󷵻�FALSE)
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
		else if (sec < 0) //ʱ�䲻��
		{
			return FALSE;
		}

		pthis->ctrl |= BIT(TTIMER_BIT_TIMEOUT);
	}

	return TRUE;
}

/***********************************************************
 * ������������ѯ�Ƿ��Ѿ���ʱ(����ȴ�Ϊusʱ����< 500,������(����ȷ))
 * ��������� ��
 * ��������� ��
 * �� �� ֵ��  FALSE ��, ������ʱ (ֹͣ�󷵻�FALSE)
 ***********************************************************/
BOOL ttimer_is_timeout(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    return ttimer_is_timeout2(pthis, &tick_now);
}

/***********************************************************
 * ������������ѯ�Ƿ��Ѿ���ʱ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ��  FALSE ��, ������ʱ (ֹͣ�󷵻�FALSE)
 ***********************************************************/
void ttimer_wait_timeout(ttimer_t *pthis)
{
	while (!ttimer_is_timeout(pthis))
	{
	}
}

/***********************************************************
 * ����������ʣ����¼�
 * ��������� ptick_now �Ƚ�ʱ�� (����ΪNULL)
 * ��������� ��
 * �� �� ֵ��  0 �ѳ�ʱ���Ѿ�ֹͣ,  ���� ʣ��ʱ��
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
 * ����������ʣ����¼�
 * ��������� ��
 * ��������� ��
 * �� �� ֵ��  0 �ѳ�ʱ���Ѿ�ֹͣ,  ���� ʣ��ʱ��
 ***********************************************************/
u32 ttimer_time_less(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    return ttimer_time_less2(pthis, &tick_now);
}

/***********************************************************
 * ������������ȥ��ʱ��
 * ��������� ptick_old �ȴ���ʼʱ��
 *           ptick_new ����ʱ��
 * ��������� ��
 * �� �� ֵ��  ��ȥ��ms��
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
 * ���������������ڹ�ȥ��ʱ��ms
 * ��������� ptick �ȴ���ʼʱ��
 * ��������� ptick_now ���ڵ�ʱ�� (NULL ������)
 * �� �� ֵ��  ��ȥ��ms��
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
  * �������������� us ʱ��
  * ���������
  * ��������� ��
  * �� �� ֵ��  ��
  ***********************************************************/
INLINE__ void time_add(time_tick_t *ptick, u32 ms)
{
	u32 ms_in = ptick->ms + ms;

	ptick->ms = ms_in%1000;
	ptick->s += ms_in/1000;
}

/***********************************************************
 * ���������������ӳ�ʱ��
 * ��������� ms �ӳ�ʱ��(���6��)
 *           ptick_now ʱ�� (����ΪNULL)
 * ��������� ��
 * �� �� ֵ��  ��
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
 * ���������������ӳ�ʱ��
 * ��������� ms �ӳ�ʱ��
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void ttimer_set(ttimer_t *pthis, u32 ms)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);

    ttimer_set2(pthis, ms, &tick_now);
}

/***********************************************************
 * �������������ó�ʱʱ��Ϊ��һ�η�ֹͣ��setʱ��(��ǰʱ�����¼���ʱ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��   ��
 ***********************************************************/
void ttimer_repeat(ttimer_t *pthis)
{
    time_tick_t tick_now;

    get_time_tick(&tick_now);
    ttimer_set2(pthis, pthis->ctrl & TTIMER_MASK_DELAY, &tick_now);
}

/***********************************************************
 * �������������ó�ʱʱ��Ϊ��һ�η�ֹͣ��setʱ��(�ϴγ�ʱʱ������ʱ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��   ��
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





