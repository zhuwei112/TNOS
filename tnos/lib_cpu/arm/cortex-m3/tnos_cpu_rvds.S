;
;********************************************************************************************************
; File      : 开源 GPLV2协议
; Version   : V1.0.1
; By        : wlison zhu
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : Keil uVision
;********************************************************************************************************


    IMPORT  gs_ptnos_tcb_cur             ;cur run tcb
    IMPORT  gs_ptnos_tcb_ready           ;ready run tcb

    EXPORT  PendSV_Handler               ;pend svc



;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************
    PRESERVE8
    THUMB

    AREA CODE, CODE, READONLY


    ;IMPORT  xprintf_put_char
;********************************************************************************************************
;                                   void PendSV_Handler(void)
;********************************************************************************************************
PendSV_Handler PROC
    ;MOVS    r0, #64
    ;PUSH    {LR}
    ;BL.W     xprintf_put_char
    ;POP     {LR}

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

    CPSIE   I
    BX      LR                                                  ;Exception return will restore remaining context
    ENDP



hardfault_handler_proc     PROC
            EXPORT  hardfault_handler_proc                [WEAK]
            B       .
            ENDP

;********************************************************************************************************
;              HardFault_Handler
;********************************************************************************************************
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

