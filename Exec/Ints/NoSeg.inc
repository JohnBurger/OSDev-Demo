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
                TEST    BYTE    [ESP+4],x86.Seg.IDT ; Missing in IDT?
                JZ              .NormalDT       ; Nope. Phew!
.BadDT:
                MOV             BX,Exec.LDT.NoSeg.TSS  ; For TSS Backlink
                CALL            Exec.Ints.Fault.Handler ; Can't handle this!
                JMP             .Finish

.NormalDT:
                MOV             EBX,Exec.LDT.NoSeg.TSS ; For TSS Backlink
                CALL            Exec.Ints.Fault.TSS    ; Find TSS Backlink

; Now I've got a problem. I need to mark the Present bit in a Descriptor, but
; that Descriptor may be in the Faulting Task's LDT - which isn't mine, so I
; don't have an alias to it! I may need to create one...

; For debugging, I'll always create the LDT alias.
                MOV             AL,Type.Mem(Data, DPL0, RW) ; Alias' .Type (writable)
                MOVZX           ESI,WORD [FS:x86.TSS.LDT] ; Point to faulting LDT
                MOV             BX,Exec.LDT.NoSeg.LDT     ; Point to my Alias for it

                CALL            Exec.Ints.Fault.Copy      ; Copy across

; Move these three lines of code back above the "For debugging" comment above,
; to avoid doing an unnecessary LDT copy for GDT Descriptors.
                MOV             EDX,[ESP+4]      ; Get Error code
                TEST            EDX,x86.Seg.TI   ; Does code show LDT Descriptor?
                JZ              .Present         ; No! And GDT is already in DS!

                OR              EBX,x86.Seg.TI   ; Can now load DS with faulting LDT
                MOV             DS,BX
.Present:
                AND             EDX,x86.Seg.Index ; Ignore extra bits in Error code
                JZ              .BadDT            ; Zero? ZERO? (Must be VirtualBox!)
                OR      BYTE    [EDX+x86.Desc.Type],x86.Desc.Type.Present

                MOV             AX,Selector(GDT.VGA, GDT, RPL0)
                MOV             DS,AX
                INC     BYTE    [0016h]         ; Dingle(tm) to say we've done it!
.Finish:
                ADD             ESP,8           ; Get rid of interrupt number and Error code
                IRETD                           ; Return, but resumes here, so...
                JMP             .Handler        ; ...go back for more
