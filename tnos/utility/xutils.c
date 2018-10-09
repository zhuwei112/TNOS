/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: xutils.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 常用工具函数
 * 版本历史:
 ***********************************************************/
#include "xutils.h"

const char gc_itos[]  = "0123456789ABCDEF";

/***********************************************************
 * 函数名称： xstrlen
 * 功能描述：安全 strlen
 * 输入参数： s 输入字符串
 * 输出参数：无
 * 返 回 值：字符串长度, NULL 为0
 ***********************************************************/
u16 xstrlen(const char *s)
{
    return (s != NULL) ? strlen(s) : (u16)0;
}


/***********************************************************
 * 函数名称： xstrncpy
 * 功能描述：安全 strncpy 保证最后有结束符
 * 输入参数： src 输入字符串
 *           size  缓冲区的大小(包括结束符长度)
 * 输出参数：dest 输出的位置
 * 返 回 值：字符串长度, -1失败(n为0)
 ***********************************************************/
s16 xstrncpy(char *dest, const char *src, u16 size)
{
	u16 len;
	
    /***C++test说明***********************
     * src 必须是一个字符串
     * 保证 从src 的拷贝,不超过 dest 的大小size,
     * 且dest非NULL时,一定是个字符串
     * **********************************/

    if ((dest == NULL) || (size == 0)) //dest 不合法
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
 * 函数名称：xvsnprintf
 * 功能描述：安全 vsnprintf 保证安全不溢出
 * 输入参数：maxlen 最大大小,包括结束符
pfmt   格式化字符串
 *          arg    可变参数
 * 输出参数：pbuf  输出缓冲区(只要有maxlen, 结尾一定是'\0')
 * 返 回 值： 实际格式的长度(len < maxlen)
 ***********************************************************/
int xvsnprintf(char *pbuf, int maxlen, const char *pfmt, va_list arg)
{
    if ((pbuf != NULL) && (maxlen > 0))
    {
        int len = vsnprintf(pbuf, maxlen, pfmt, arg); //

        if (len > 0)      //
        {
            if (len > maxlen - 1)  //防止超长度
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
 * 函数名称：xsnprintf
 * 功能描述：安全 snprintf 保证安全不溢出
 * 输入参数：maxlen 最大大小,包括结束符
pfmt   格式化字符串
 *          arg    可变参数
 * 输出参数：pbuf  输出缓冲区(只要有maxlen, 结尾一定是'\0')
 * 返 回 值： 实际格式的长度(len < maxlen)
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
 * 函数名称：xstrstr
 * 功能描述： 匹配数据,匹配后跳到匹配位置
 * 输入参数：pbuf 输入缓冲区
 *          pneedle 匹配字符串
 * 输出参数：无
 * 返 回 值： 找到的匹配位置, NULL 失败
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
 * 函数名称：xstrstre
 * 功能描述： 匹配字符串,匹配后跳到字符串中匹配needle[strlen(pneedle)]位置
 * 输入参数：pbuf 输入缓冲区
 *          pneedle 匹配字符串
 * 输出参数：无
 * 返 回 值： 找到的匹配位置, NULL 失败
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
// * 函数名称：xstristre
// * 功能描述： 不区分大小匹配字符串,匹配后跳到字符串中匹配needle[strlen(pneedle)]位置
// * 输入参数：pbuf 输入缓冲区
// *          pneedle 匹配字符串
// * 输出参数：无
// * 返 回 值： 找到的匹配位置, NULL 失败
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
 * 函数名称：rm_left_trim
 * 功能描述：删除左边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无空格位置
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
 * 函数名称：rm_right_trim
 * 功能描述：删除右边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无
 ***********************************************************/
void rm_right_trim(char *s)
{
    u32 len = xstrlen(s);

    if (len != 0)
    {
        /***polyspace 不过说明***********************
        * s 为NULL时,len=0,
        * 所以 p不可能为NULL
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
 * 函数名称：rm_trim
 * 功能描述：删除左右边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无空格位置
 ***********************************************************/
char *rm_trim(char *s)
{
    s = rm_left_trim(s);
    rm_right_trim(s);

    return s;
}


/***********************************************************
 * 功能描述： 将数字转为字符串
 * 输入参数： value 值
 *        size  数组大小
 * 输出参数：pstring 输出数组
 * 返 回 值： 数组的第一个位置
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
 * 功能描述：32位大小端转换
 * 输入参数：x
 * 输出参数：无
 * 返 回 值： 转换后的值
 ***********************************************************/
u32 swab32(u32 x)
{
    return ((x & 0XFF) << 24) |
      ((x & 0XFF00) << 8) |
      ((x & 0XFF0000UL) >> 8) |
      ((x & 0XFF000000UL) >> 24);
}

/***********************************************************
 * 功能描述：16位大小端转换
 * 输入参数：x
 * 输出参数：无
 * 返 回 值： 转换后的值
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
  函数名称:    months
  函数描述:    判断指定的年份是平年或闰年,并返回相应的月份日数
  作者：
  输入参数:    year: 年号
  输出参数:    无
  返回说明:    相应年份的月份日数
  其它说明:
*************************************************/
//static u32 *months(u32 year)
//{
//    //平年每个月的日数
//    static u32 s_nonleapyear[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//    //闰年每个月的日数
//    static u32 s_leapyear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//    if((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
//    {
//        return s_leapyear;
//    }

//    return s_nonleapyear;
//}
//#define xMINUTE   (60             ) //1分的秒数
//#define xHOUR      (60*xMINUTE) //1小时的秒数
//#define xDAY        (24*xHOUR   ) //1天的秒数
//#define xYEAR       (365*xDAY   ) //1年的秒数
//u32  xdate_seconds(u8 *time)
//{
//  static unsigned int  month[12]={
//    /*01月*/xDAY*(0),
//    /*02月*/xDAY*(31),
//    /*03月*/xDAY*(31+28),
//    /*04月*/xDAY*(31+28+31),
//    /*05月*/xDAY*(31+28+31+30),
//    /*06月*/xDAY*(31+28+31+30+31),
//    /*07月*/xDAY*(31+28+31+30+31+30),
//    /*08月*/xDAY*(31+28+31+30+31+30+31),
//    /*09月*/xDAY*(31+28+31+30+31+30+31+31),
//    /*10月*/xDAY*(31+28+31+30+31+30+31+31+30),
//    /*11月*/xDAY*(31+28+31+30+31+30+31+31+30+31),
//    /*12月*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)
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
//  year = year_now-1970;       //不考虑2100年千年虫问题
//  seconds = xYEAR*year + xDAY*((year+1)/4);  //前几年过去的秒数
//  seconds += month[month_now-1];      //加上今年本月过去的秒数
//  if( (month_now > 2) && (((year+2)%4)==0) )//2008年为闰年
//    seconds += xDAY;            //闰年加1天秒数
//  seconds += xDAY*(day_now-1);         //加上本天过去的秒数
//  seconds += xHOUR*hour;           //加上本小时过去的秒数
//  seconds += xMINUTE*minute;       //加上本分钟过去的秒数
//  seconds += second;               //加上当前秒数<br>　seconds -= 8 * xHOUR;
//  seconds -= xHOUR*8;//yuant加的
//  return seconds;
//}
///*************************************************
//  函数名称:    seconds
//  函数描述:    将rtc时间转换为自1970.1.1开始的秒数
//  作者：
//  输入参数:    rtc: 转换后的rtc时间
//  输出参数:
//  返回说明:    转换后的秒数
//  其它说明:
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


