/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: xutils.h
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 定义
 * 版本历史:
 ***********************************************************/
#ifndef XUTILS_H__
#define XUTILS_H__

#include "tnos_def.h"
#include "xprintf.h"

/***********************************************************
 * 函数名称： xstrlen
 * 功能描述：安全 strlen
 * 输入参数： s 输入字符串
 * 输出参数：无
 * 返 回 值：字符串长度, NULL 为0
 ***********************************************************/
u16 xstrlen(const char *s);

/*---------------------------------------------------------
 * xstrncpy 安全 xstrncpy 保证最后有结束符 (在n=0时,没有)
 * dest out  输出的位置
 * src   in  输入字符串
 * size  in  缓冲区的大小(包括结束符长度)
 * 返回 0 , 字符串长度, -1失败(n为0)
 *---------------------------------------------------------*/
s16 xstrncpy(char * dest, const char * src, u16 size);

//xstrncpy 保证dest 不溢出 (dest必须为数组)
#undef XSTRNCPY
#define XSTRNCPY(dest, src)  xstrncpy((dest), (src), sizeof((dest)))

//xstrncpy 保证dest, 和src 都不溢出 (dest和src必须为数组)
#undef XSTRNCPY2
#define XSTRNCPY2(dest, src) xstrncpy((dest), (src), MIN(sizeof((dest)), sizeof((src))))




/***********************************************************
 * 函数名称：xvsnprintf
 * 功能描述：安全 vsnprintf 保证安全不溢出
 * 输入参数：maxlen 最大大小,包括结束符
			pfmt   格式化字符串
 *          arg    可变参数
 * 输出参数：pbuf  输出缓冲区(只要有maxlen, 结尾一定是'\0')
 * 返 回 值： 实际格式的长度(len < maxlen)
 ***********************************************************/
//int xvsnprintf(char *pbuf, int maxlen, const char *pfmt, __va_list arg);

/***********************************************************
 * 函数名称：xsnprintf
 * 功能描述：安全 snprintf 保证安全不溢出
 * 输入参数：maxlen 最大大小,包括结束符
			pfmt   格式化字符串
 *          arg    可变参数
 * 输出参数：pbuf  输出缓冲区(只要有maxlen, 结尾一定是'\0')
 * 返 回 值： 实际格式的长度(len < maxlen)
 ***********************************************************/
int xsnprintf(char *pbuf, int maxlen, const char *pfmt, ...) AT_FORMAT_PRINTF(3, 4);

/*安全格式化到数组中,最大大小为数组大小*/
#undef XSNPRINTF
#define XSNPRINTF(pbuf, pfmt, ...)  xsnprintf(pbuf, sizeof(pbuf), pfmt, ##__VA_ARGS__)

/***********************************************************
 * 函数名称：xvsnprintf
 * 功能描述：安全 vsnprintf 保证安全不溢出
 * 输入参数：maxlen 最大大小,包括结束符
pfmt   格式化字符串
 *          arg    可变参数
 * 输出参数：pbuf  输出缓冲区(只要有maxlen, 结尾一定是'\0')
 * 返 回 值： 实际格式的长度(len < maxlen)
 ***********************************************************/
int xvsnprintf(char *pbuf, int maxlen, const char *pfmt, va_list arg);

/***********************************************************
 * 函数名称：xstrstr
 * 功能描述： 匹配字符串,匹配后跳到字符串中匹配needle[0]位置
 * 输入参数：pbuf 输入缓冲区
 *          pneedle 匹配字符串
 * 输出参数：无
 * 返 回 值： 找到的匹配位置, NULL 失败
 ***********************************************************/
char *xstrstr(const char *pbuf, const char *pneedle);

/***********************************************************
 * 函数名称：xstrstre
 * 功能描述： 匹配字符串,匹配后跳到字符串中匹配needle[strlen(pneedle)]位置
 * 输入参数：pbuf 输入缓冲区
 *          pneedle 匹配字符串
 * 输出参数：无
 * 返 回 值： 找到的匹配位置, NULL 失败
 ***********************************************************/
char *xstrstre(const char *pbuf, const char *pneedle);

/***********************************************************
 * 函数名称：xstristre
 * 功能描述： 不区分大小匹配字符串,匹配后跳到字符串中匹配needle[strlen(pneedle)]位置
 * 输入参数：pbuf 输入缓冲区
 *          pneedle 匹配字符串
 * 输出参数：无
 * 返 回 值： 找到的匹配位置, NULL 失败
 ***********************************************************/
char *xstristre(const char *pbuf, const char *pneedle);


/***********************************************************
 * 函数名称：rm_trim
 * 功能描述：删除左右边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无空格位置
 ***********************************************************/
char * rm_trim(char *s);

/***********************************************************
 * 函数名称：rm_left_trim
 * 功能描述：删除左边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无空格位置
 ***********************************************************/
char * rm_left_trim(const char *s);

/***********************************************************
 * 函数名称：rm_right_trim
 * 功能描述：删除右边的空格
 * 输入参数：s 输入字符串
 * 输出参数：无
 * 返 回 值： 无
 ***********************************************************/
void rm_right_trim(char *s);

/***********************************************************
 * 功能描述： 将数字转为字符串
 * 输入参数： value 值
 *        size  数组大小
 * 输出参数：pstring 输出数组
 * 返 回 值： 数组的第一个位置
 ***********************************************************/
char*itoa(int value, char *pstring, int size);

/***********************************************************
 * 功能描述：32位大小端转换
 * 输入参数：x
 * 输出参数：无
 * 返 回 值： 转换后的值
 ***********************************************************/
u32 swab32(u32 x);

/***********************************************************
 * 功能描述：16位大小端转换
 * 输入参数：x
 * 输出参数：无
 * 返 回 值： 转换后的值
 ***********************************************************/
u16 swab16(u16 x);

/*************************************************
  函数名称:    seconds
  函数描述:    将rtc时间转换为自1970.1.1开始的秒数
  作者：
  输入参数:    rtc: 转换后的rtc时间
  输出参数:
  返回说明:    转换后的秒数
  其它说明:
*************************************************/
u32 seconds(u8 *time);

u32  xdate_seconds(u8 *time);


//"0123456789ABCDEF"
extern const char gc_itos[];

//程序版本号
extern const char gc_version[];

#endif

