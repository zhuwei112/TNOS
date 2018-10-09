/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: xutils.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ���ù��ߺ���
 * �汾��ʷ:
 ***********************************************************/
#include "xutils.h"

const char gc_itos[]  = "0123456789ABCDEF";

/***********************************************************
 * �������ƣ� xstrlen
 * ������������ȫ strlen
 * ��������� s �����ַ���
 * �����������
 * �� �� ֵ���ַ�������, NULL Ϊ0
 ***********************************************************/
u16 xstrlen(const char *s)
{
    return (s != NULL) ? strlen(s) : (u16)0;
}


/***********************************************************
 * �������ƣ� xstrncpy
 * ������������ȫ strncpy ��֤����н�����
 * ��������� src �����ַ���
 *           size  �������Ĵ�С(��������������)
 * ���������dest �����λ��
 * �� �� ֵ���ַ�������, -1ʧ��(nΪ0)
 ***********************************************************/
s16 xstrncpy(char *dest, const char *src, u16 size)
{
	u16 len;
	
    /***C++test˵��***********************
     * src ������һ���ַ���
     * ��֤ ��src �Ŀ���,������ dest �Ĵ�Сsize,
     * ��dest��NULLʱ,һ���Ǹ��ַ���
     * **********************************/

    if ((dest == NULL) || (size == 0)) //dest ���Ϸ�
    {
        return -1;
    }

    len = xstrlen(src);

    if (len != 0)
    {
        if (len > size - 1)
        {
            len = size - 1;
        }

        memcpy(dest, src, len);
    }

    dest[len] = 0;
    return (s16)len;
}



/***********************************************************
 * �������ƣ�xvsnprintf
 * ������������ȫ vsnprintf ��֤��ȫ�����
 * ���������maxlen ����С,����������
pfmt   ��ʽ���ַ���
 *          arg    �ɱ����
 * ���������pbuf  ���������(ֻҪ��maxlen, ��βһ����'\0')
 * �� �� ֵ�� ʵ�ʸ�ʽ�ĳ���(len < maxlen)
 ***********************************************************/
int xvsnprintf(char *pbuf, int maxlen, const char *pfmt, va_list arg)
{
    if ((pbuf != NULL) && (maxlen > 0))
    {
        int len = vsnprintf(pbuf, maxlen, pfmt, arg); //

        if (len > 0)      //
        {
            if (len > maxlen - 1)  //��ֹ������
            {
                len = maxlen - 1;
            }

            pbuf[len] = 0;
            return (int)len;
        }

        pbuf[0] = 0;
    }

    return 0U;
}

/***********************************************************
 * �������ƣ�xsnprintf
 * ������������ȫ snprintf ��֤��ȫ�����
 * ���������maxlen ����С,����������
pfmt   ��ʽ���ַ���
 *          arg    �ɱ����
 * ���������pbuf  ���������(ֻҪ��maxlen, ��βһ����'\0')
 * �� �� ֵ�� ʵ�ʸ�ʽ�ĳ���(len < maxlen)
 ***********************************************************/
int xsnprintf(char *pbuf, int maxlen, const char *pfmt, ...)
{
    int ret;
    va_list va;

    va_start(va, pfmt);
    ret = xvsnprintf(pbuf, maxlen, pfmt, va);
    va_end(va);

    return ret;
}


/***********************************************************
 * �������ƣ�xstrstr
 * ���������� ƥ������,ƥ�������ƥ��λ��
 * ���������pbuf ���뻺����
 *          pneedle ƥ���ַ���
 * �����������
 * �� �� ֵ�� �ҵ���ƥ��λ��, NULL ʧ��
 ***********************************************************/
char *xstrstr(const char *pbuf, const char *pneedle)
{
    if (pbuf != NULL)
    {
        return (char *)strstr(pbuf, pneedle);
    }

    return NULL;
}

/***********************************************************
 * �������ƣ�xstrstre
 * ���������� ƥ���ַ���,ƥ��������ַ�����ƥ��needle[strlen(pneedle)]λ��
 * ���������pbuf ���뻺����
 *          pneedle ƥ���ַ���
 * �����������
 * �� �� ֵ�� �ҵ���ƥ��λ��, NULL ʧ��
 ***********************************************************/
char *xstrstre(const char *pbuf, const char *pneedle)
{
    char *ret = xstrstr(pbuf, pneedle);

    if (ret != NULL)
    {
        ret += strlen(pneedle);
    }

    return ret;
}

///***********************************************************
// * �������ƣ�xstristre
// * ���������� �����ִ�Сƥ���ַ���,ƥ��������ַ�����ƥ��needle[strlen(pneedle)]λ��
// * ���������pbuf ���뻺����
// *          pneedle ƥ���ַ���
// * �����������
// * �� �� ֵ�� �ҵ���ƥ��λ��, NULL ʧ��
// ***********************************************************/
//char *xstristre(const char *pbuf, const char *pneedle)
//{
//    char *ret = xstristr(pbuf, pneedle);
//
//    if (ret != NULL)
//    {
//        ret += strlen(pneedle);
//    }
//
//    return ret;
//}

#define ISSPACE(val) ((val) == ' ')


/***********************************************************
 * �������ƣ�rm_left_trim
 * ����������ɾ����ߵĿո�
 * ���������s �����ַ���
 * �����������
 * �� �� ֵ�� �޿ո�λ��
 ***********************************************************/
char * rm_left_trim(const char *s)
{
    if (s != NULL)
    {
        while (ISSPACE(*s))
        {
            ++s;
        }
    }

    return (char *)s;
}

/***********************************************************
 * �������ƣ�rm_right_trim
 * ����������ɾ���ұߵĿո�
 * ���������s �����ַ���
 * �����������
 * �� �� ֵ�� ��
 ***********************************************************/
void rm_right_trim(char *s)
{
    u32 len = xstrlen(s);

    if (len != 0)
    {
        /***polyspace ����˵��***********************
        * s ΪNULLʱ,len=0,
        * ���� p������ΪNULL
        * **********************************/
        char *p = s + len - 1;

        while((p >= s) && (ISSPACE(*p)))
        {
            --p;
        }

        *(++p) = 0;
    }
}

/***********************************************************
 * �������ƣ�rm_trim
 * ����������ɾ�����ұߵĿո�
 * ���������s �����ַ���
 * �����������
 * �� �� ֵ�� �޿ո�λ��
 ***********************************************************/
char *rm_trim(char *s)
{
    s = rm_left_trim(s);
    rm_right_trim(s);

    return s;
}


/***********************************************************
 * ���������� ������תΪ�ַ���
 * ��������� value ֵ
 *        size  �����С
 * ���������pstring �������
 * �� �� ֵ�� ����ĵ�һ��λ��
 ***********************************************************/
char*itoa(int value, char *pstring, int size)
{

	if (size >= 2)
	{
		char *pend = &pstring[size];
		char *p = pend;

		*--p = 0;

		do
		{
			*--p = gc_itos[value % 10];
		}while (((value /= 10) != 0) && (p > pstring));

		if (p > pstring)
		{
			memmove(pstring, p, pend - p);
		}

		return pstring;
	}
	
	return NULL;
}


/***********************************************************
 * ����������32λ��С��ת��
 * ���������x
 * �����������
 * �� �� ֵ�� ת�����ֵ
 ***********************************************************/
u32 swab32(u32 x)
{
    return ((x & 0XFF) << 24) |
      ((x & 0XFF00) << 8) |
      ((x & 0XFF0000UL) >> 8) |
      ((x & 0XFF000000UL) >> 24);
}

/***********************************************************
 * ����������16λ��С��ת��
 * ���������x
 * �����������
 * �� �� ֵ�� ת�����ֵ
 ***********************************************************/
u16 swab16(u16 x)
{
    return ((x & 0XFF) << 8) | ((x & 0XFF00) >> 8);
}


#define SECONDS_PER_DAY         (24 * 60 * 60)
#define DAYS_PER_400YEARS       (365 * 400 + 97)
#define DAYS_PER_100YEARS       (365 * 100 + 24)
#define DAYS_PER_4YEARS         (365 * 4 + 1)
#define DAYS_1970_TO_2001       (1 * 365 + 1) //(31 * 365 + 8)
#define SECONDS_OF_TIME_ZONE    (0) //(8 * 3600)



/*************************************************
  ��������:    months
  ��������:    �ж�ָ���������ƽ�������,��������Ӧ���·�����
  ���ߣ�
  �������:    year: ���
  �������:    ��
  ����˵��:    ��Ӧ��ݵ��·�����
  ����˵��:
*************************************************/
//static u32 *months(u32 year)
//{
//    //ƽ��ÿ���µ�����
//    static u32 s_nonleapyear[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//    //����ÿ���µ�����
//    static u32 s_leapyear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//    if((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
//    {
//        return s_leapyear;
//    }

//    return s_nonleapyear;
//}
//#define xMINUTE   (60             ) //1�ֵ�����
//#define xHOUR      (60*xMINUTE) //1Сʱ������
//#define xDAY        (24*xHOUR   ) //1�������
//#define xYEAR       (365*xDAY   ) //1�������
//u32  xdate_seconds(u8 *time)
//{
//  static unsigned int  month[12]={
//    /*01��*/xDAY*(0),
//    /*02��*/xDAY*(31),
//    /*03��*/xDAY*(31+28),
//    /*04��*/xDAY*(31+28+31),
//    /*05��*/xDAY*(31+28+31+30),
//    /*06��*/xDAY*(31+28+31+30+31),
//    /*07��*/xDAY*(31+28+31+30+31+30),
//    /*08��*/xDAY*(31+28+31+30+31+30+31),
//    /*09��*/xDAY*(31+28+31+30+31+30+31+31),
//    /*10��*/xDAY*(31+28+31+30+31+30+31+31+30),
//    /*11��*/xDAY*(31+28+31+30+31+30+31+31+30+31),
//    /*12��*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)
//  };
//  u8 month_now = time[2];
//    u8 day_now = time[3];
//    u8 hour = time[4];
//    u8 minute = time[5];
//    u8 second = time[6];

//    char test[20] = {0,};
//    sprintf(test, "%d", time[0]);
//    sprintf(test + 2, "%d", time[1]);
//    test[4] = '\0';
//    u32 year_now = atoi(test);

//  unsigned int  seconds = 0;
//  unsigned int  year = 0;
//  year = year_now-1970;       //������2100��ǧ�������
//  seconds = xYEAR*year + xDAY*((year+1)/4);  //ǰ�����ȥ������
//  seconds += month[month_now-1];      //���Ͻ��걾�¹�ȥ������
//  if( (month_now > 2) && (((year+2)%4)==0) )//2008��Ϊ����
//    seconds += xDAY;            //�����1������
//  seconds += xDAY*(day_now-1);         //���ϱ����ȥ������
//  seconds += xHOUR*hour;           //���ϱ�Сʱ��ȥ������
//  seconds += xMINUTE*minute;       //���ϱ����ӹ�ȥ������
//  seconds += second;               //���ϵ�ǰ����<br>��seconds -= 8 * xHOUR;
//  seconds -= xHOUR*8;//yuant�ӵ�
//  return seconds;
//}
///*************************************************
//  ��������:    seconds
//  ��������:    ��rtcʱ��ת��Ϊ��1970.1.1��ʼ������
//  ���ߣ�
//  �������:    rtc: ת�����rtcʱ��
//  �������:
//  ����˵��:    ת���������
//  ����˵��:
//*************************************************/
//u32 seconds(u8 *time)
//{
//    // First, accumulate days since January 1, 2001.
//    // Using 2001 instead of 1970 makes the leap-year
//    // handling easier (see SecondsToUTC), because
//    // it is at the beginning of the 4-, 100-, and 400-year cycles.
//    u32 day = 0UL;
//    u8 month_now = time[2];
//    u8 day_now = time[3];
//    u8 hour_now = time[4];
//    u8 minute_now = time[5];
//    u8 second_now = time[6];

//    char test[20] = {0,};
//    sprintf(test, "%d", time[0]);
//    sprintf(test + 2, "%d", time[1]);
//    test[4] = '\0';
//    u32 year = atoi(test);
//    u32 year_now = year;

//    u8 buf2[6]={0,};
//    buf2[0]= month_now;//year >>24;
//    buf2[1]= day_now;//year >>16;
//    buf2[2]= hour_now;//year >>8;
//    buf2[3]= minute_now;//year >>0;
//    buf2[4]= second_now;//year >>0;
//    udp_send_data(0x02, buf2, 6);

//    // Rewrite year to be >= 2001.
//    if(year < 2001)
//    {
//        u32 n = (2001 - year) / 400 + 1;
//        year += 400 * n;
//        if(day >= DAYS_PER_400YEARS * n)//yuant2017210
//        {
//            day -= DAYS_PER_400YEARS * n;
//        }
//    }

//    // Add in days from 400-year cycles.
//    u32 n = (year - 2001) / 400;
//    year -= 400 * n;
//    day += DAYS_PER_400YEARS * n;

//    // Add in 100-year cycles.
//    n = (year - 2001) / 100;
//    year -= 100 * n;
//    day += DAYS_PER_100YEARS * n;

//    // Add in 4-year cycles.
//    n = (year - 2001) / 4;
//    year -= 4 * n;
//    day += DAYS_PER_4YEARS * n;

//    // Add in non-leap years.
//    n = year - 2001;
//    day += 365 * n;

//    // Add in days this year.
//    u32 *month = months(year_now);
//    int m = 0;
//    for(m = 0; m < month_now - 1; m++)
//    {
//        day += month[m];
//    }
//    day += day_now - 1;

//    // Convert days to seconds since January 1, 2001.
//    u32 sec = day * SECONDS_PER_DAY;

//    // Add in time elapsed today.
//    sec += hour_now * 3600;
//    sec += minute_now * 60;
//    sec += second_now;

//    // Convert from seconds since 2001 to seconds since 1970.
//    sec += DAYS_1970_TO_2001 * SECONDS_PER_DAY;
//    sec -= SECONDS_OF_TIME_ZONE;

//    return sec;
//}


