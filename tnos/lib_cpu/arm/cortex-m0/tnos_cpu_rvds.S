;
;********************************************************************************************************
; File      : tnos_cpu_a.S
; Version   : V1.0.1
; By        : wlison zhu
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : Keil uVision
;********************************************************************************************************


    IMPORT  gs_ptnos_tcb_cur             ;cur run tcb
    IMPORT  gs_ptnos_tcb_ready           ;ready run tcb

    EXPORT  tnos_start_rdy               ;strart run
    EXPORT  PendSV_Handler               ;pend svc

    EXPORT  irq_disable
    EXPORT  irq_enable

;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************
SCB_VTOR          EQU     0xE000ED08               ; Vector Table Offset Register
NVIC_INT_CTRL     EQU     0xE000ED04               ; interrupt control state register
NVIC_SYSPRI2      EQU     0xE000ED20               ; PendSV priority register
NVIC_PENDSV_PRI   EQU     0x00FF0000               ; PendSV priority value (lowest)
NVIC_PENDSVSET    EQU     0x10000000               ; value to trigger PendSV exception


    PRESERVE8
    THUMB

    AREA CODE, CODE, READONLY


;********************************************************************************************************
;                                      void tnos_start_rdy(void)
; Note(s) : first task to start
;           a. set priority for PendSV (lowest)
;           b. PSP=0 (for frist PendSV)
;           c. restore MSP
;           d. triggers PendSV
;           e. enable irq
;********************************************************************************************************
tnos_start_rdy    PROC
    LDR     r0, =NVIC_SYSPRI2
    LDR     r1, =NVIC_PENDSV_PRI  ;NVIC_SYSPRI2 |= NVIC_SYSPRI2
    LDR.W   r2, [r0,#0x00]
    ORR     r1,r1,r2
    STR     r1, [r0]

    MOV     R0, #0               ;PSP=0, for frist PendSV
    MSR     PSP, R0

    LDR     r0, =SCB_VTOR        ;restore MSP
    LDR     r0, [r0]
    LDR     r0, [r0]
    MSR     MSP, r0

    LDR     r0, =NVIC_INT_CTRL   ;triggers PendSV
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
    CPSIE   I

    B       .
    ENDP


;********************************************************************************************************
;                                   void PendSV_Handler(void)
;********************************************************************************************************
PendSV_Handler PROC
    MRS     R3, PRIMASK
    CPSID   I
    LDR     R2, =gs_ptnos_tcb_cur
    MRS     R0, PSP
    CBZ     R0, OS_CPU_PendSVHandler_nosave                     ;Skip register save the first time

    SUBS    R0, R0, #0x20                                       ;Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, [R2]
    STR     R0, [R1]                                            ;gs_ptnos_tcb_cur->pstk = SP;

OS_CPU_PendSVHandler_nosave
    LDR     R1, =gs_ptnos_tcb_ready                             ;gs_ptnos_tcb_cur = gs_ptnos_tcb_ready;
    LDR     R0, [R1]
    STR     R0, [R2]
    LDR     R0, [R0]

    LDM     R0, {R4-R11}                                        ;Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ;Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ;Ensure exception return uses process stack

    MSR     PRIMASK, R3
    BX      LR                                                  ;Exception return will restore remaining context
    ENDP


;********************************************************************************************************
;              u32 irq_disable(void)
;              disable irq  and allow to break nesting
;********************************************************************************************************
irq_disable   PROC
        MRS     R0, PRIMASK
        CPSID   I
        BX      LR
        ENDP

;********************************************************************************************************
;              void irq_enable(u32 reg)
;              enable irq  and allow to break nesting
;********************************************************************************************************
irq_enable   PROC
    MSR     PRIMASK, r0
    BX      LR
    ENDP


hardfault_handler_proc     PROC
            EXPORT  hardfault_handler_proc                [WEAK]
            B       .
            ENDP

;********************************************************************************************************
;              HardFault_Handler
;********************************************************************************************************
;    IMPORT hardfault_handler_proc
    EXPORT HardFault_Handler
HardFault_Handler    PROC
    TST     lr, #0x04               ; if(!EXC_RETURN[2])
    MRSNE   r0, msp                 ; get fault context from handler.
    MRSEQ   r0, psp                 ; get fault context from thread.

    STMFD   r0!, {r4 - r11}         ; push r4 - r11 register
    STMFD   r0!, {lr}               ; push exec_return register

    MSRNE   msp, r0                 ; update stack pointer to MSP.
    MSREQ   psp, r0                 ; update stack pointer to PSP.

    BL      hardfault_handler_proc
    B       .
    ENDP

    ALIGN   4

    END

