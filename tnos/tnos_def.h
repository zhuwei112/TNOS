/***********************************************************
 * ��Ȩ��Ϣ:��Դ GPLV2Э��
 * �ļ�����: tnos_def.h
 * �ļ�����: ��Ρ
 * �������:
 * ��ǰ�汾: 1.0.0
 * ��Ҫ����: ����
 * �汾��ʷ:
 ***********************************************************/
#ifndef _TNOS_DEF_H
#define _TNOS_DEF_H


#define TNOS_VER_MAJOR                  1UL //���汾��
#define TNOS_VER_SUB                    0UL //�ְ汾��
#define TNOS_VER_REVISE                 2UL //�Ͱ汾��

//�汾��ֵ
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




/***C++test˵��***********************
 * FALSE, TRUE ����Ϊ�޷���,��ֹ����澯
 *       ������涼�����ϰٵľ���
 * **********************************/
#define  FALSE   ((u32)0)
#define  TRUE    ((u32)1)

#undef NULL
#define NULL 0

#define  LIKELY(x)        __builtin_expect(!!(x), 1) //Ԥ�ⷢ�����ʴ������
#define  UNLIKELY(x)      __builtin_expect(!!(x), 0) //Ԥ��С��������

#if defined ( __CC_ARM )          /* ARM Compiler */
    #define AT_ALIGN2               __attribute__((aligned(2)))              //2�ֽڶ���
    #define AT_ALIGN4               __attribute__((aligned(4)))              //4�ֽڶ���
    #define AT_UNUSED               __attribute__ ((unused))                  //���Բ��õ�
    #define AT_FORMAT_PRINTF(c, p)                                 //����printf ��������
    #define AT_SECTION(x)           __attribute__((section(x)))
    #define WEAK                    __weak
    #define INLINE__                static __inline
    #define ASM__                   __asm

#elif defined ( __ICCARM__ )      //IAR

    #define AT_ALIGN2               PRAGMA(data_alignment=2)             //2�ֽڶ���
    #define AT_ALIGN4               PRAGMA(data_alignment=4)              //4�ֽڶ���
    #define AT_PACKED               __attribute__((packed))                  //��С����
    #define AT_UNUSED
    #define AT_FORMAT_PRINTF(c, p)                                    //����printf ��������
    #define AT_SECTION(x)           @ x
    #define WEAK                    __weak
    #define INLINE__                static inline
    #define ASM__                   __asm

#elif defined ( __GNUC__ )  //GNU

    #define AT_ALIGN2               __attribute__((aligned(2)))              //2�ֽڶ���
    #define AT_ALIGN4               __attribute__((aligned(4)))              //4�ֽڶ���
    #define AT_PACKED               __attribute__((packed))                  //��С����
    #define AT_UNUSED               __attribute__ ((unused))                  //���Բ��õ�
    #define AT_FORMAT_PRINTF(c, p)  __attribute__((format(printf, (c), (p)))) //����printf ��������
    #define AT_SECTION(x)           __attribute__((section(x)))
    #define WEAK                    __attribute__((weak))
    #define INLINE__                static __inline
    #define ASM__                   __asm
#endif

/*�����С*/
#undef  ARRAY_SIZE
#define ARRAY_SIZE(a)       ((sizeof(a))/(sizeof((a)[0])))

/*�ṹ���Ա�ڽṹ���ƫ��ֵ*/
#undef  OFFET_OF
#define OFFET_OF(s,m)       ((u32)&(((s *)0)->m))

/*�ṹ���Ա�ڽṹ��Ĵ�С*/
#undef MEMBER_SIZE
#define MEMBER_SIZE(s, m) (sizeof((((s *)0)->m)))

/*lignByte �ֽ� ���϶���, ��֤ ����data�ֽ� */
#define ALIGN_UP(data, lignByte)   (((data) + (lignByte) - 1) & ~((u32)(lignByte) - 1)) //����� lignByte ��ʽ����, ��������Ϊ 2��n����

/*lignByte �ֽ� ���¶���, data �����ȥ��, ����Ϊ2��n���� */
#define ALIGN_DOWN(data, lignByte) ((data) & ~((u32)(lignByte) - 1))

#undef MIN
#define MIN(a,b) (((a)<=(b)) ? (a):(b))


#undef MAX
#define MAX(a,b) (((a)>=(b)) ? (a):(b))


#undef  BIT
#define BIT(bit)   ((u32)(1U << (bit)))

#undef  BIT_MSK
#define BIT_MSK(bit) (BIT(bit) - 1U)  //���31λ, 32λ�����


#define TO_S(a)    #a         //����תΪ�ַ���
#define DTO_S(d)  TO_S(d)     //�궨������תΪ�ַ���

//�ַ���������
#undef STRING_HAVE
#define STRING_HAVE(s) (((s) != NULL) && ((s)[0] != 0))

//�ַ���������
#undef STRING_EMPTY
#define STRING_EMPTY(s) (((s) == NULL) || ((s)[0] == 0))

/*���newʧ�ܴ���*/
#define NEW(p, type) \
try\
{\
    p = new type;\
}\
catch(std::bad_alloc& e)\
{\
    p = NULL;\
}



/*���� sizeof(dest) �������� �� dest*/
#undef MEMCPY
#define MEMCPY(dest, src) memcpy((dest), (src), sizeof((dest)))

#undef MEMCPY2
#define MEMCPY2(dest, src) memcpy((&dest), (src), sizeof((dest)))

/*�������  ����sizeof(dest)*/
#undef CLEAN
#define CLEAN(dest) memset((dest), 0, sizeof((dest)))

/*�������  ����sizeof(dest)*/
#undef CLEAN2
#define CLEAN2(dest) memset((&dest), 0, sizeof((dest)))

#undef CLEAN_SIZE
#define CLEAN_SIZE(dest, size) memset((dest), 0, (size))

#undef CLEAN_SIZE2
#define CLEAN_SIZE2(dest, size) memset((&dest), 0, (size))

//�ַ���������
#undef STRING_HAVE
#define STRING_HAVE(s) (((s) != NULL) && ((s)[0] != 0))

//�ַ���������
#undef STRING_EMPTY
#define STRING_EMPTY(s) (((s) == NULL) || ((s)[0] == 0))



/***********************************************************
 * ������������ֹ�ж�(֧���ж�Ƕ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 ***********************************************************/
void irq_disable(void);

/***********************************************************
 * ���������������ж�(֧���ж�Ƕ��)
 * �����������
 * ��������� ��
 * �� �� ֵ��  ��
 * ע��:�ж�Ƕ�ײ�ƥ�䵼���ж������Թر�!!!
 ***********************************************************/
void irq_enable(void);


#endif

