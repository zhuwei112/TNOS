/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: tnos_def.h
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 定义
 * 版本历史:
 ***********************************************************/
#ifndef _TNOS_DEF_H
#define _TNOS_DEF_H


#define TNOS_VER_MAJOR                  1UL //主版本号
#define TNOS_VER_SUB                    0UL //分版本号
#define TNOS_VER_REVISE                 3UL //低版本号

//版本的值
#define TNOS_VER    ((TNOS_VER_MAJOR * 10000) + (TNOS_VER_SUB * 100) + TNOS_VER_REVISE)

#ifdef EXLIPCE
typedef unsigned char            uint8_t;
typedef unsigned short           uint16_t;
typedef unsigned int             uint32_t;
typedef unsigned long long       uint64_t;
typedef char                     int8_t;
typedef short                    int16_t;
typedef int                      int32_t;
typedef long long                int64_t;
#endif

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  <string.h>



typedef unsigned char            u8;
typedef unsigned short           u16;
typedef unsigned int             u32;
typedef unsigned long long    u64;
typedef char                     s8;
typedef short                    s16;
typedef int                      s32;
typedef long long             s64;
typedef unsigned char            BOOL8;
typedef unsigned int             BOOL;




/***C++test说明***********************
 * FALSE, TRUE 定义为无符号,防止后面告警
 *       否则后面都会有上百的警告
 * **********************************/
#define  FALSE   ((u32)0)
#define  TRUE    ((u32)1)

#undef NULL
#define NULL 0

#define  LIKELY(x)        __builtin_expect(!!(x), 1) //预测发生概率大的事情
#define  UNLIKELY(x)      __builtin_expect(!!(x), 0) //预测小概率事情

#if defined ( __CC_ARM )          /* ARM Compiler */
    #define AT_ALIGN2               __attribute__((aligned(2)))              //2字节对齐
    #define AT_ALIGN4               __attribute__((aligned(4)))              //4字节对齐
    #define AT_UNUSED               __attribute__ ((unused))                  //可以不用到
    #define AT_FORMAT_PRINTF(c, p)                                 //按照printf 风格检查参数
    #define AT_SECTION(x)           __attribute__((section(x)))
    #define WEAK                    __weak
    #define INLINE__                static __inline
    #define ASM__                   __asm

#elif defined ( __ICCARM__ )      //IAR

    #define AT_ALIGN2               PRAGMA(data_alignment=2)             //2字节对齐
    #define AT_ALIGN4               PRAGMA(data_alignment=4)              //4字节对齐
    #define AT_PACKED               __attribute__((packed))                  //最小对齐
    #define AT_UNUSED
    #define AT_FORMAT_PRINTF(c, p)                                    //按照printf 风格检查参数
    #define AT_SECTION(x)           @ x
    #define WEAK                    __weak
    #define INLINE__                static inline
    #define ASM__                   __asm

#elif defined ( __GNUC__ )  //GNU

    #define AT_ALIGN2               __attribute__((aligned(2)))              //2字节对齐
    #define AT_ALIGN4               __attribute__((aligned(4)))              //4字节对齐
    #define AT_PACKED               __attribute__((packed))                  //最小对齐
    #define AT_UNUSED               __attribute__ ((unused))                  //可以不用到
    #define AT_FORMAT_PRINTF(c, p)  __attribute__((format(printf, (c), (p)))) //按照printf 风格检查参数
    #define AT_SECTION(x)           __attribute__((section(x)))
    #define WEAK                    __attribute__((weak))
    #define INLINE__                static __inline
    #define ASM__                   __asm
#endif

/*数组大小*/
#undef  ARRAY_SIZE
#define ARRAY_SIZE(a)       ((sizeof(a))/(sizeof((a)[0])))

/*结构体成员在结构体的偏移值*/
#undef  OFFET_OF
#define OFFET_OF(s,m)       ((u32)&(((s *)0)->m))

/*结构体成员在结构体的大小*/
#undef MEMBER_SIZE
#define MEMBER_SIZE(s, m) (sizeof((((s *)0)->m)))

/*lignByte 字节 向上对齐, 保证 包含data字节 */
#define ALIGN_UP(data, lignByte)   (((data) + (lignByte) - 1) & ~((u32)(lignByte) - 1)) //以最大 lignByte 方式对齐, 次数必须为 2的n次幂

/*lignByte 字节 向下对齐, data 多余的去掉, 必须为2的n次幂 */
#define ALIGN_DOWN(data, lignByte) ((data) & ~((u32)(lignByte) - 1))

#undef MIN
#define MIN(a,b) (((a)<=(b)) ? (a):(b))


#undef MAX
#define MAX(a,b) (((a)>=(b)) ? (a):(b))


#undef  BIT
#define BIT(bit)   ((u32)(1U << (bit)))

#undef  BIT_MSK
#define BIT_MSK(bit) (BIT(bit) - 1U)  //最大到31位, 32位会错误


#define TO_S(a)    #a         //数字转为字符串
#define DTO_S(d)  TO_S(d)     //宏定义内容转为字符串

//字符串有数据
#undef STRING_HAVE
#define STRING_HAVE(s) (((s) != NULL) && ((s)[0] != 0))

//字符串无数据
#undef STRING_EMPTY
#define STRING_EMPTY(s) (((s) == NULL) || ((s)[0] == 0))

/*解决new失败处理*/
#define NEW(p, type) \
try\
{\
    p = new type;\
}\
catch(std::bad_alloc& e)\
{\
    p = NULL;\
}



/*拷贝 sizeof(dest) 长度内容 到 dest*/
#undef MEMCPY
#define MEMCPY(dest, src) memcpy((dest), (src), sizeof((dest)))

#undef MEMCPY2
#define MEMCPY2(dest, src) memcpy((&dest), (src), sizeof((dest)))

/*清空数据  长度sizeof(dest)*/
#undef CLEAN
#define CLEAN(dest) memset((dest), 0, sizeof((dest)))

/*清空数据  长度sizeof(dest)*/
#undef CLEAN2
#define CLEAN2(dest) memset((&dest), 0, sizeof((dest)))

#undef CLEAN_SIZE
#define CLEAN_SIZE(dest, size) memset((dest), 0, (size))

#undef CLEAN_SIZE2
#define CLEAN_SIZE2(dest, size) memset((&dest), 0, (size))

//字符串有数据
#undef STRING_HAVE
#define STRING_HAVE(s) (((s) != NULL) && ((s)[0] != 0))

//字符串无数据
#undef STRING_EMPTY
#define STRING_EMPTY(s) (((s) == NULL) || ((s)[0] == 0))



/***********************************************************
 * 功能描述：禁止中断(支持中断嵌套)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 ***********************************************************/
void irq_disable(void);

/***********************************************************
 * 功能描述：允许中断(支持中断嵌套)
 * 输入参数：无
 * 输出参数： 无
 * 返 回 值：  无
 * 注意:中断嵌套不匹配导致中断永久性关闭!!!
 ***********************************************************/
void irq_enable(void);


#endif

