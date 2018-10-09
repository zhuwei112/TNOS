;
;********************************************************************************************************
; File      : 开源 GPLV2协议
; Version   : V1.0.1
; By        : wlison zhu
; For       : ARMv7M Cortex-M0
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


;********************************************************************************************************
;                                   void PendSV_Handler(void)
;********************************************************************************************************
PendSV_Handler PROC
    CPSID   I
    LDR     R0, =gs_ptnos_tcb_ready
    LDR     R3, [R0]
    LDR     R2, =gs_ptnos_tcb_cur
    LDR     R1, [R2]

    CMP     R1, R3                     ;gs_ptnos_tcb_cur == gs_ptnos_tcb_ready ? exit
    BEQ     svc_exit
    CMP     R1, #0x00
    BEQ     svc_nosave                 ;Skip register save the first time

    MRS     R0, PSP
    SUBS    R0, R0, #0x20
    STR     R0, [R1]                   ;gs_ptnos_tcb_cur->pstk = SP;

    STMIA   R0!, {R4 - R7}             ;将R4~R7入栈
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4 - R7}             ;将R8~R11入栈

svc_nosave
    STR     R3, [R2]
    LDR     R0, [R3]                  ;gs_ptnos_tcb_cur = gs_ptnos_tcb_ready;


    MOV     R1, R0
    ADDS    R0, R0, #16               ;恢复 (R8-R11)
    LDMIA   R0!, {R4-R7}
    MOV     R8, R4
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    LDMIA   R1!, {R4 - R7}            ;恢复 (R4-R7)
    MSR     PSP, R0                   ;将新的SP更新到PSP中

svc_exit
    MOV      R0, LR
    MOVS     R1, #0x04
    ORRS     R0, R0, R1

    CPSIE   I
    BX      R0                    ;Exception return will restore remaining context
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

