/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_dbg.h
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ϵͳ ͷ�ļ�
 * �汾��ʷ:
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
 * ��������������
 * ���������pexct ����ַ���
 *          pfunc ��������
 *          line  �к�
 * �����������
 * �� �� ֵ��  ��
 ***********************************************************/
void tnos_assert(const char* pexct, const char* pfunc, s32 line);

#else
#define TNOS_ASSERT(exct)
#endif

#endif
