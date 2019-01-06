;
; Ints/User.inc
;

; This module demonstrates providing System Services to a User-mode program.
; The interrupts have been given a DPL of 3, to allow the User program access.

;...............................................................................
; HLT is a privileged instruction, since it can lock up the PC. But for demo
; purposes, an interrupt has been set up for the User program to call.
; Note that it gives the opportunity for the OS to ensure interrupts aren't
; disabled during the HLT.

Ints.Halt:
;               STI                     ; Ensure Interrupts enabled! Or else...
                HLT
                IRETD                   ; That was easy! (Note EFlags popped...)
;...............................................................................
; This interrupt does the same Task Switch that happens on the Timer call.
Ints.Yield:     ; Note interrupts are disabled!
                PUSH            EAX
                CALL            Ints.Switch
                POP             EAX
                IRETD
