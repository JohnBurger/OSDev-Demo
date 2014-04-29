;
; Exec/Ints/NoSeg.inc
;

; This module installs an example No Segment handler.
; TAKE TWO: This module installs a whole Fault environment to handle the
; No Segment Fault. It leverages the generic Task Fault handler, but doesn't
; call the Fatal Register displayer: instead it re-implements what the old
; handler did, only from the nested Task context.
Exec.Ints.NoSeg:
                MOV             AX,IDT.NoSeg     ; This is the IDT Entry
                MOV             BX,Exec.LDT.NoSeg ; This is the LDT Triple
                MOV             EDX,.Handler     ; This is the handler
                CALL            Exec.Ints.Fault  ; Set it up
                RET

.Handler:
;               PUSH   DWORD    0               ; Pseudo-fault code (not required)
                PUSH            11              ; Interrupt number
                MOV             BX,Exec.LDT.NoSeg.TSS   ; For TSS Backlink
                MOV             EDX,[ESP+4]     ; Get Error code
                CALL            Exec.Ints.NoSeg.Present
                JNZ             .Dingle         ; Now present?

                ; VirtualBox PUSHes a zero error code. NOT very helpful!
                MOV             BX,Exec.LDT.NoSeg.TSS   ; For TSS Backlink
                CALL            Exec.Ints.Fault.Handler ; Can't handle this!
                JMP             .Finish
.Dingle:
                MOV             AX,Selector(GDT.VGA, GDT, RPL0)
                MOV             DS,AX
                INC     BYTE    [WORD 000Bh*2]  ; Dingle(tm) to say we've done it!
.Finish:
                ADD             ESP,8           ; Get rid of interrupt number and Error code
                IRETD                           ; Return, but resumes here, so...
                JMP             .Handler        ; ...go back for more

; Mark a Segment as present.
; Input:   BX = Your TSS alias (for Backlink)
;         EDX = Error Code
; Output:   Z flag on error (e.g. already present)
;         BX, EDX modified
Exec.Ints.NoSeg.Present:
                TEST            DL,x86.Seg.IDT  ; Not Present in IDT?
                JZ              .NormalDT       ; Nope. Check for GDT/LDT

.InIDT:
                MOV             AX,Selector(Exec.LDT.IDT, LDT, RPL0) ; Yes
                MOV             DS,AX           ; Point to IDT
                JMP             .Present        ; Now make Present in IDT (even if zero!)

.NormalDT:
                MOV             AX,Selector(GDT.Alias, GDT, RPL0)
                MOV             DS,AX

                TEST            DL,x86.Seg.LDT   ; Does Error code show LDT Descriptor?
                JNZ             .InLDT           ; Yes. Lots of work ahead!

.InGDT:                                          ; No. In GDT
                TEST            DX,x86.Seg.Index ; Ignoring extra bits in Error code...
                JZ              .Error           ; Zero is NOT helpful in GDT!

                JMP             .Present         ; DS already set up!

.InLDT:
; Now I've got a problem. I need to mark the Present bit in a Descriptor, but
; that Descriptor is in the Faulting Task's LDT - which isn't mine, so I don't
; have an alias to it! I need to create one...
                PUSH            EDX
                CALL            Exec.Ints.Fault.TSS    ; Find TSS Backlink

                MOV             AL,Type.Mem(Data, DPL0, RW) ; Alias' .Type (writable)
                MOVZX           ESI,WORD [FS:x86.TSS.LDT] ; Point to faulting LDT
                MOV             BX,Exec.LDT.NoSeg.LDT     ; Point to my Alias for it

                CALL            Exec.Ints.Fault.Copy      ; Copy across
                POP             EDX

                OR              EBX,x86.Seg.LDT  ; Can now load DS with faulting LDT
                MOV             DS,BX
.Present:
                MOV             AX,7              ; Bit number of x86.Desc.Type.Present
                AND             EDX,x86.Seg.Index ; Ignore extra bits in Error code
                BTS             [EDX + x86.Desc.Type],AX
                JNC             .Finish           ; Wasn't already set
.Error:
                XOR             EAX,EAX           ; Z flag on Error!
.Finish:
                RET
