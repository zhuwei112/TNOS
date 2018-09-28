/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_dbg.h
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 系统 头文件
 * 版本历史:
 ***********************************************************/
#ifndef TNOS_DBG_H__
#define TNOS_DBG_H__

#include "tnos_cfg.h"

#if (TNOS_DBG_ENABLE != 0)

#define TNOS_ASSERT(exct)                                          \
    if (!(exct))                                                   \
    {                                                              \
        tnos_assert(#exct, __FUNCTION__, __LINE__);  \
    }


/***********************************************************
 * 功能描述：断言
 * 输入参数：pexct 输出字符串
 *          pfunc 函数名称
 *          line  行号
 * 输出参数：无
 * 返 回 值：  无
 ***********************************************************/
void tnos_assert(const char* pexct, const char* pfunc, s32 line);

#else
#define TNOS_ASSERT(exct)
#endif

#endif
