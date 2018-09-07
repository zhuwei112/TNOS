/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_cfg.h
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: �����ļ�
 * �汾��ʷ:
 ***********************************************************/
#ifndef TNOS_CFG_H__
#define TNOS_CFG_H__


#include "tnos_def.h"


//�����������
#define TNOS_STK_NAME_SIZE         8 //��������ռ�ÿռ��С

//�ź������
#define TNOS_SEM_ENABLE            1  //�����ź���

//���������
#define TNOS_MUTEX_ENABLE          1  //���û�����

//��Ϣ�������
//�̶���󳤶�,ÿ�η�����󳤶ȿռ�
#define TNOS_MSGQ_ENABLE           1  //������Ϣ��

//��Ϣѭ������
//�ɱ䳤��,ÿ�η������ݳ���, ��ַ����(���β��ʣ��ռ䲻��,�����ռ�!!)
#define TNOS_MSGCYC_ENABLE         1  //������Ϣѭ������


//�Ѽ��CPUʹ���ʼ��
#define TNOS_STK_CHECK             1   //�Ѽ��
#define TNOS_PS_PERIOD             1   //ͳ��һ������,��λS

//�����������
#define TNOS_TASK_IDLE_NAME        "idle" //�������������
#define TNOS_IDLE_STK_NUM          128    //��������ĶѴ�С(ʹ�õ���main������ջ!!)
#define TNOS_IDLE_HOOK             1      //����������

//�Ƿ����õ���
#define TNOS_DBG_ENABLE             1 //���õ���

#define TNOS_ARG_CHECK             0 //�������

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
