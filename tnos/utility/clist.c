/***********************************************************
 * 版权信息:开源 GPLV2协议
 * 文件名称: clist.c
 * 文件作者: 朱巍
 * 完成日期:
 * 当前版本: 1.0.0
 * 主要功能: 定义
 * 版本历史:
 ***********************************************************/
#include "clist.h"

/*---------------------------------------------------------
 *list_insert_after 将链表n,查到l的后面
 *---------------------------------------------------------*/
void list_insert_after(list_t *l, list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/*---------------------------------------------------------
 *list_insert_before 将链表n,查到l的前面
 *---------------------------------------------------------*/
void list_insert_before(list_t *l, list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/*---------------------------------------------------------
 *list_remove 移除链表
 *---------------------------------------------------------*/
void list_remove(list_t *n)
{
    if (n->next != NULL)
    {
        n->next->prev = n->prev;
        n->prev->next = n->next;

        n->next = NULL;
    }
}

/*---------------------------------------------------------
 *pnew 代替 pold 位置(移除pold)
 *---------------------------------------------------------*/
void list_replace(list_t *pold, list_t *pnew)
{
	 pnew->next = pold->next;
	 pold->next->prev = pnew;

	 pnew->prev = pold->prev;
	 pold->prev->next = pnew;
}
