/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_dev.h
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 设备相关
 * 版本历史:
 ***********************************************************/
#ifndef TNOS_DEV_H__
#define TNOS_DEV_H__

#include "tnos_def.h"

/***********************************************************
 * 功能描述：板子硬件系统时钟和优先级初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:为了后面初始化打印和定时器做准备
 ***********************************************************/
void tnos_board_sys_pro_init(void);

/***********************************************************
 * 功能描述：板子硬件其它初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_board_other_init(void);

/***********************************************************
 * 功能描述：打印初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************/
void xprintf_init(void);

/***********************************************************
 * 功能描述：毫秒定时器初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void tnos_tim_ms_init(void);

/***********************************************************
 * 功能描述：系统设置定时器溢出时间 (外部调用需要实现)
 * 输入参数：ms 定时时间,如果传入的ms超过定时器最大值 设置未定时器最大值
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:限制最大ms为1000(保证定时器1s能调用一次)
 ***********************************************************/
void tnos_tim_ms_set(u32 ms);

/***********************************************************
 * 功能描述：系统时间初始化
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void ttimer_init(void);

/***********************************************************
 * 功能描述：普通方式打印输出字符(需要自己实现)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void xprintf_put_char(char val);


#endif
