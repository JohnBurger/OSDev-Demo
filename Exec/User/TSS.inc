;
; Exec/User/TSS.inc
;

; This module creates a User-mode Program's TSS and populates it.

Exec.User.TSS:
; This function creates a TSS for the LDT, and populates it ready for a User Task
; Input:  AX  = LDT
; Output: EAX = Non-zero on success

                CALL            Exec.Alloc.TSS
                TEST            EAX,EAX
                JZ              .End

                PUSH            DS

                PUSH            ES
                POP             DS      ; Use DS - more efficient

; Initialise fields in new TSS for User Task
                MOV             CL,Window.Width
                MOV             CH,Window.Height

                MOV             EDX,[%$Pos]

                XOR             EBX,EBX ; Use relative addressing: more code efficient
; Note that TSS is already zeroed - no need to store again
                MOV     WORD    [EBX + x86.TSS.CS],  Selector(User.LDT.Code,   LDT, RPL3)
                MOV     WORD    [EBX + x86.TSS.EIP], User.Entry
                MOV     WORD    [EBX + x86.TSS.SS0], Selector(User.LDT.Stack0, LDT, RPL0)
;               MOV    DWORD    [EBX + x86.TSS.ESP0],0
                MOV     WORD    [EBX + x86.TSS.SS],  Selector(User.LDT.Stack3, LDT, RPL3)
;               MOV    DWORD    [EBX + x86.TSS.ESP], 0
                MOV     WORD    [EBX + x86.TSS.ES],  Selector(GDT.VGA,         GDT, RPL3)
                MOV     WORD    [EBX + x86.TSS.DS],  Selector(User.LDT.Data,   LDT, RPL3)
                MOV     WORD    [EBX + x86.TSS.EBX], VGA.Cols * 2
                MOV             [EBX + x86.TSS.ECX], ECX
                MOV             [EBX + x86.TSS.EDX], EDX

                POP             DS

                MOV             AH,0            ; Not a System TSS
                CALL            Exec.Alloc.TSS.Enable
.End:
                RET
