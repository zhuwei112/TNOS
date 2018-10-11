/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_cfg.h
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 配置文件
 * 版本历史:
 ***********************************************************/
#ifndef TNOS_CFG_H__
#define TNOS_CFG_H__


#include "tnos_def.h"


//任务属性相关
#define TNOS_STK_NAME_SIZE         8 //任务名字占用空间大小

//信号量相关
#define TNOS_SEM_ENABLE            1  //启用信号量

//互斥锁相关
#define TNOS_MUTEX_ENABLE          1  //启用互斥锁

//消息队列相关
//固定最大长度,每次分配最大长度空间
#define TNOS_MSGQ_ENABLE           1  //启用消息队

//消息循环缓冲(消息长度为单位)
//可变长度,每次分配数据长度, 地址连续(如果尾部剩余空间不够,丢弃空间!!)
#define TNOS_MSGCYC_ENABLE         1  //启用消息循环缓冲

//循环缓冲区
//字节为单位的循环缓冲区
#define TNOS_CYC_ENABLE           1  //启用管道


//堆检和CPU使用率检查
#define TNOS_STK_CHECK             1   //堆检查
#define TNOS_PS_PERIOD             5   //统计一次周期,单位S

//空闲任务相关
#define TNOS_TASK_IDLE_NAME        "idle" //空闲任务的名字
#define TNOS_IDLE_STK_NUM          128    //空闲任务的堆大小(使用的是main函数堆栈!!)
#define TNOS_IDLE_HOOK             1      //空闲任务钩子

//是否启用调试
#define TNOS_DBG_ENABLE             1 //启用调试

#define TNOS_ARG_CHECK             0 //参数检查

//
//#if (TNOS_TICK_RATE_HZ > 1000)
//    #error "TNOS_TICK_RATE_HZ mast <= 1000"
//#endif
//
//#define TNOS_TICK_MS    ((1000/(TNOS_TICK_RATE_HZ)))

#if (TNOS_TASK_NUM > 31)
    #error "TNOS_TASK_NUM mast  <= 31"
#endif

#if (TNOS_SLICE_MS > 255)
    #error "TNOS_TASK_NUM mast  <= 255"
#endif

#if (TNOS_STK_NAME_SIZE <= 0)
    #error "TNOS_STK_NAME_SIZE mast  > 0"
#endif

#endif
