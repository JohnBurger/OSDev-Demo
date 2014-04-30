;
; Exec/Ints/BadStack.inc
;

; Ths module installs a Stack Fault handler. It's mostly a stub, leaving the
; hard work to the generic .Fault handler.
Exec.Ints.BadStack:
                MOV             AX,IDT.BadStack  ; This is the IDT Entry
                MOV             BX,Exec.LDT.BadStack ; This is the LDT triple
                MOV             EDX,.Handler     ; This is the handler
                CALL            Exec.Ints.Fault  ; Set it up
                RET

.Handler:
;               PUSH   DWORD    0               ; Pseudo-fault code (not required)
                PUSH            12              ; Interrupt number

                MOV             AX,Selector(GDT.VGA, GDT, RPL0)
                MOV             DS,AX
                INC     BYTE    [WORD 000Ch*2]  ; Dingle(tm) to say we've done it!

                MOV             BX,Exec.LDT.BadStack.TSS ; For TSS Backlink
                MOV             EDX,[ESP+4]     ; Get Error code
                CALL            Exec.Ints.NoSeg.Present ; Test if Not Present
                JNZ             .Finish         ; Now it's Present
.Error:
                MOV             BX,Exec.LDT.BadStack.TSS ; For TSS Backlink
                CALL            Exec.Ints.Fault.Handler ; Something else is wrong
.Finish:
                ADD             ESP,8           ; Get rid of interrupt number and Error code
                IRETD                           ; Return, but resumes here, so...
                JMP             .Handler        ; ...go back for more
