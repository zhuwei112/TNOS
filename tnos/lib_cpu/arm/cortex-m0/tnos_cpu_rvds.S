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

    ;EXPORT  tnos_start_rdy               ;strart run
    EXPORT  PendSV_Handler               ;pend svc


;********************************************************************************************************
;                                               EQUATES
;********************************************************************************************************

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


  ;IMPORT  xprintf_put_char

;********************************************************************************************************
;                                   void PendSV_Handler(void)
;********************************************************************************************************
PendSV_Handler PROC
    MRS     R3, PRIMASK
    CPSID   I
    MOVS    r0, #64
    ;PUSH    {LR}
    ;BL.W     xprintf_put_char
    ;POP     {LR}
    LDR     R2, =gs_ptnos_tcb_cur
    MRS     R0, PSP
    CMP     R0, #0x00
    BEQ     OS_CPU_PendSVHandler_nosave                     ;Skip register save the first time

    SUBS    R0, R0, #0x20                                       ;Save remaining regs r4-11 on process stack
    LDR     R1, [R2]
    STR     R0, [R1]                                            ;gs_ptnos_tcb_cur->pstk = SP;

    STMIA   R0!, {R4 - R7}             ;将R4~R7入栈
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4 - R7}             ;将R8~R11入栈


OS_CPU_PendSVHandler_nosave
    LDR     R1, =gs_ptnos_tcb_ready                             ;gs_ptnos_tcb_cur = gs_ptnos_tcb_ready;
    LDR     R0, [R1]
    STR     R0, [R2]
    LDR     R0, [R0]

    MOV     R1, R0
    ADDS    R0, R0, #16                ;恢复 (R8-R11)
    LDMIA   R0!, {R4-R7}
    MOV     R8, R4
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    LDMIA   R1!, {R4 - R7}            ;恢复 (R4-R7)
    MSR     PSP, R0                   ;将新的SP更新到PSP中

    MOVS    R0, #0x04
    RSBS    R0, R0, #0x00

    ;LDM     R0, {R4-R11}                                        ;Restore r4-11 from new process stack
    ;ADDS    R0, R0, #0x20
    ;MSR     PSP, R0                                             ;Load PSP with new process SP
    ;ORR     LR, LR, #0x04                                       ;Ensure exception return uses process stack

    MSR     PRIMASK, R3
    BX      R0
    ;BX      LR                                                  ;Exception return will restore remaining context
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
    MRS    R0, MSP
    MOV    R1, LR
    MOVS   R2, #04
    ANDS   R1, R1, R2
    CMP    R1, #0
    BEQ    hf0
    MRS    R0, PSP
hf0
    SUBS   R0, R0, #0x20
    STMIA  R0!, {R4 - R7}             ;将R4~R7入栈
    MOV    R4, R8
    MOV    R5, R9
    MOV    R6, R10
    MOV    R7, R11
    STMIA  R0!, {R4 - R7}             ;将R8~R11入栈

    BL      hardfault_handler_proc
    B       .

    ENDP

    ALIGN   4

    END

