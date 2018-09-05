/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: clist.c
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ����
 * �汾��ʷ:
 ***********************************************************/
#include "clist.h"

/*---------------------------------------------------------
 *list_insert_after ������n,�鵽l�ĺ���
 *---------------------------------------------------------*/
void list_insert_after(list_t *l, list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/*---------------------------------------------------------
 *list_insert_before ������n,�鵽l��ǰ��
 *---------------------------------------------------------*/
void list_insert_before(list_t *l, list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/*---------------------------------------------------------
 *list_remove �Ƴ�����
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
 *pnew ���� pold λ��(�Ƴ�pold)
 *---------------------------------------------------------*/
void list_replace(list_t *pold, list_t *pnew)
{
	 pnew->next = pold->next;
	 pold->next->prev = pnew;

	 pnew->prev = pold->prev;
	 pold->prev->next = pnew;
}
