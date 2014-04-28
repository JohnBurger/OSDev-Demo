;
; Exec/Ints/Debug.inc
;

; This module installs a Debug handler
Exec.Ints.Debug:
                MOV             EAX,Ints.Debug
                MOV             BX,IDT.Debug
                MOV             CX,Selector(GDT.Ints, GDT, RPL0) ; Note needs to RPL0
                MOV             DL,Type.Sys(Trap, DPL3, 386) & ~System.Present ; Allow User-mode Programs
                CALL            Exec.Alloc.IDT
                RET
