;
; Exec/Ints/Fault.inc
;

; This module is the generic Fault handler, for Faults that need their own
; runtime environment: Stack, TSS, etc. It presumes the following:
; 1) The LDT has three consecutive entries for the Fault Handler:
;    a) Stack - This will be created
;    b) Alias - This will be created and set up.
;    c) Fault - This will be used by the Fault handler to alias the Faulting TSS
; 2) The .Fault function installs the Handler using the provided values.
; 3) The .Handler function will be called by the specific Fault handler.

; This function should be called by the specific Fault handlers' initialisation,
; after they set up some registers:
; Input:  EAX = Offset of Descriptor in the IDT to modify
;         EBX = Pointer to the Stack/Alias/Fault Triple in the LDT
;         EDX = Specific Fault handler entry point.
; Output:
;         A new 4 kiB Stack for the first Descriptor in the Triple
;         A fully set up new TSS, with Stack0, ready to execute at EDX
;         A Task Gate entry in the IDT at [EAX] pointing to the new TSS
;
Exec.Ints.Fault:

Exec.Ints.Fault.Stack.Size   EQU   1000h        ; Give ourselves PLENTY of room!

%push Ints.Fault ; Let's not leave these %defines just lying around...

%define         %$IDT.Entry     EBP - 4
%define         %$LDT.Triple    EBP - 8
%define         %$Handler       EBP - 12
%define         %$Base          EBP - 16
%define         %$TSS           EBP - 20

                ENTER           20, 0

                MOV             [%$IDT.Entry],EAX
                MOV             [%$LDT.Triple],EBX
                MOV             [%$Handler],EDX

                MOV             AX,Selector(Exec.LDT.Alias, LDT, RPL0)
                MOV             ES,AX         ; Point to LDT alias

                ; Allocate Stack for Fault handler
                MOV             ECX,Exec.Ints.Fault.Stack.Size
                CALL            Exec.Alloc.RAM
                TEST            EAX,EAX
                JZ              .End

                ; Store in LDT
                MOV             DL,Type.Mem(Stack, DPL0, RW)
                MOV             DH,Gran.Mem(Byte,  Big)
                CALL            Exec.Alloc.DT.Mem

                ; Allocate TSS
                SLDT            AX            ; My LDT
                CALL            Exec.Alloc.TSS
                TEST            EAX,EAX
                JZ              .End

                ; Remember its Base
                MOV             [%$Base],EAX

                MOV             EBX,[%$LDT.Triple]
                OR              BX,WORD x86.Seg.TI      ; Index inside LDT
                MOV             EDX,[%$Handler]

                ; Initialise TSS values
                MOV             [ES:x86.TSS.SS],BX
                MOV             [ES:x86.TSS.CS],CS
                MOV             [ES:x86.TSS.EIP],EDX

                ; Enable it as a TSS
                MOV             AH,System.TSS   ; Oh BOY is it a System TSS!
                CALL            Exec.Alloc.TSS.Enable

                MOV             [%$TSS],EBX

                MOV             AX,Selector(Exec.LDT.Alias, LDT, RPL0)
                MOV             ES,AX             ; Point to LDT alias again

                ; Set up Alias entry in LDT
                MOV             EAX,[%$Base]
                MOV             EBX,[%$LDT.Triple]
                ADD             EBX,x86.Desc_size ; Point to .TSS Alias in LDT
                MOV             DL,Type.Mem(Data, DPL0, NoRW) ; TSS Alias
                MOV             DH,Gran.Mem(Byte, Small)
                CALL            Exec.Alloc.DT.Mem

                ; Finally, update IDT Entry to tell it it's now handled by a TSS
                XOR             EAX,EAX
                MOV             EBX,[%$IDT.Entry]
                MOV             ECX,[%$TSS]
                MOV             DL,Type.Sys(Task, DPL0, 286) ; No '386 variant
                CALL            Exec.Alloc.IDT
.End:
                LEAVE
                RET
%pop
;...............................................................................
; This function should be called by the specific Fault handlers' entry points,
; after they prepare the environment:
; Input:  [ESP+8] = Error code
;         [ESP+4] = Interrupt number
;         EBX     = Pointer to 'my' TSS Alias in the Stack/Alias/Fault triple
;
.Handler:
                STI                           ; Interrupts may have been off

.TestIRQs:
                ; Worse, Fault may have happened during an IRQ handler!
                ; Isolate IRQ handlers that have started but aren't acknowledged
                MOV             AL,Dev.PIC.Cmd.ISR
                OUT             Dev.PIC.A.Cmd,AL
                JMP             $+2             ; Wait for I/O bus
                IN              AL,Dev.PIC.A.Cmd

                AND             AL,Dev.PIC.A.Key|Dev.PIC.A.Timer; Only worried about these
                JZ              .NoInterrupts    ; Nothing to worry about!
                CMP             AL,Dev.PIC.A.Key ; If keypress outstanding...
                JE              .KeyInterrupt    ; Handle it
                MOV             AL,Dev.PIC.Cmd.EOI ; Acknowledge
                OUT             Dev.PIC.A.Cmd,AL ; current IRQ (should be Timer)
                JMP             .TestIRQs        ; And keep looking

.KeyInterrupt:
                INT             (IDT.Key-IDT)/x86.Desc_size ; Re-invoke the Key interrupt
; Continue
.NoInterrupts:
                CALL            Exec.Ints.Fault.TSS

                MOV             EDI,[ESP+4]       ; Load Interrupt number
                PUSH            EDX               ; Save faulting TR for display
                CALL            GDT.Ints : Ints.Fault.External

                ADD             ESP,4             ; Ignore faulting TR
                RET
;...............................................................................
; This function gets the Back link field from 'my' TSS Alias and brute-force
; copies that TSS' Descriptor into the LDT for access as a Data Segment.
; Input:  EBX = LDT pointer to 'my' TSS Alias
; Output: EDX = My TSS' Back link value
;          DS = GDT Alias
;          ES = LDT Alias
;          FS = Faulting TSS Alias
;          EBX modified
Exec.Ints.Fault.TSS:
                MOV             AX,Selector(GDT.Alias, GDT, RPL0)
                MOV             DS,AX         ; Need information from GDT
                MOV             AX,Selector(Exec.LDT.Alias, LDT, RPL0)
                MOV             ES,AX         ; Need to update alias in LDT
                LEA             AX,[BX + x86.Seg.TI] ; Need my TSS alias in LDT
                MOV             FS,AX

                ADD             EBX,x86.Desc_size ; Point to TSS alias in LDT

                MOVZX           ESI,WORD [FS:x86.TSS.Back] ; Who called me?
                MOV             EDX,ESI           ; Remember for later

                MOV             AL,Type.Mem(Data, DPL0, NoRW)
                CALL            Exec.Ints.Fault.Copy

                LEA             AX,[BX + x86.Seg.TI] ; Selector for alias in LDT
                MOV             FS,AX
                RET

; This function copies a Descriptor to the LDT and turns it into an alias.
; Input:   AL = Desired final .Type
;         EBX = Pointer to LDT Descriptor to modify
;         ESI = Pointer to Descriptor that's the source to copy
;          DS = Source DT (GDT?)
;          ES = LDT
; Output: ECX, ESI, EDI modified
Exec.Ints.Fault.Copy:
                ; Copy faulting Descriptor, then alter .Type to make it an alias
                CLD                               ; Work forwards
                MOV             ECX,x86.Desc_size / 4      ; Number of DWords
                MOV             EDI,EBX
                REP             MOVSD             ; Brute force details across!
                MOV             [ES:EBX+x86.Desc.Type],AL
                RET
