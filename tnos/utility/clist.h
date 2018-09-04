/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: clist.h
 * 文件作者: 朱巍
 * 完成日期:2016年2月17日
 * 当前版本: 1.0.0
 * 主要功能: c封装列表
 * 版本历史:
 ***********************************************************/
#ifndef CLIST_H_
#define CLIST_H_

#include "tnos_def.h"

/*双向链表相关*/
typedef struct LIST_TARG
{
    struct LIST_TARG *next;
    struct LIST_TARG *prev;
}list_t;

/*---------------------------------------------------------
 *list_init 链表初始化,链表头必须调用!!
 *---------------------------------------------------------*/
INLINE__ void list_init(list_t *l)
{
    l->next = l->prev = l;
}


/*---------------------------------------------------------
 *list_isempty 链表是否为空(一般检查链表头!!)
 *---------------------------------------------------------*/
INLINE__ BOOL list_isempty(const list_t *l)
{
    return l->next == l;
}

/*---------------------------------------------------------
 *list_ishave 链表有数据(一般检查链表头!!)
 *---------------------------------------------------------*/
INLINE__ BOOL list_ishave(const list_t *l)
{
    return l->next != l;
}

/*---------------------------------------------------------
 *list_insert_after 将链表n,查到l的后面
 *---------------------------------------------------------*/
 void list_insert_after(list_t *l, list_t *n);

 /*---------------------------------------------------------
  *list_insert_before 将链表n,查到l的前面
  *---------------------------------------------------------*/
void list_insert_before(list_t *l, list_t *n);

/*---------------------------------------------------------
 *pnew 代替 pold 位置(移除pold)
 *---------------------------------------------------------*/
void list_replace(list_t *pold, list_t *pnew);


/*---------------------------------------------------------
 *list_remove 移除链表
 *---------------------------------------------------------*/
void list_remove(list_t *n);

/*---------------------------------------------------------
 *LIST_OBJECT_INIT 链表头初始化,用于变量初始化里
 *---------------------------------------------------------*/
#define LIST_OBJECT_INIT(object) { &(object), &(object) }
/*---------------------------------------------------------
 *LIST_ENTRY 将 链表转为 包含该链表结构体的起始位置
 * node 现在的位置,  type 包含链表结构体的类型   member该结构体中链表的成员名称
 *---------------------------------------------------------*/
#define LIST_ENTRY(node, type, member) \
        ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

/*---------------------------------------------------------
 *LIST_FPREACH_ENTRY_LIST cong某个list_start开始 循环每一个成员 ,并返回起始位置
 * pos 结构体起始位置      head 头部位置    member该结构体中链表的成员名称
 *---------------------------------------------------------*/
/***C++test说明***********************
 * LIST_FPREACH_ENTRY_LIST 增加括号会导致编译不过
 * **********************************/
#define LIST_FPREACH_ENTRY_LIST(pos, list_start, head, member)  \
    for (pos = LIST_ENTRY(list_start, typeof(*pos), member); \
        &pos->member != (head); \
        pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))

/*---------------------------------------------------------
 *list_forEachEntry 循环每一个成员 ,并返回起始位置
 * pos 结构体起始位置      head 头部位置    member该结构体中链表的成员名称
 *---------------------------------------------------------*/
/***C++test说明***********************
 * LIST_FPREACH_ENTRY_LIST 增加括号会导致编译不过
 * **********************************/
#define LIST_FPREACH_ENTRY(pos, head, member) \
    LIST_FPREACH_ENTRY_LIST(pos, (head)->next, head, member)


#endif /* UTILITY_INC_CLIST_H_ */
