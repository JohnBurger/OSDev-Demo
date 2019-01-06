;
; x86/EFlags.inc
;

; The following are the bitfields in the EFlags register

; ID flag: If this bit can be set/cleared, then CPUID is supported
x86.EFlags.ID   EQU             0000_0000_0010_0000_0000_0000_0000_0000b

; Virtual Interrupt Pending
x86.EFlags.VIP  EQU             0000_0000_0001_0000_0000_0000_0000_0000b

; Virtual Interrupt Flag
x86.EFlags.VIF  EQU             0000_0000_0000_1000_0000_0000_0000_0000b

; Alignment Check/Access Control
x86.EFlags.AC   EQU             0000_0000_0000_0100_0000_0000_0000_0000b

; Virtual-8086 Mode
x86.EFlags.VM   EQU             0000_0000_0000_0010_0000_0000_0000_0000b

; Resume Flag: Used during Debug exception IRET
x86.EFlags.RF   EQU             0000_0000_0000_0001_0000_0000_0000_0000b

; Nested Task: Whether to check the TSS.Link field on IRET
x86.EFlags.NT   EQU             0000_0000_0000_0000_0100_0000_0000_0000b

; I/O Privilege Level
x86.EFlags.IOPL EQU             0000_0000_0000_0000_0011_0000_0000_0000b

; Overflow Flag
x86.EFlags.OF   EQU             0000_0000_0000_0000_0000_1000_0000_0000b

; Direction Flag: Changed with CLD/STD
x86.EFlags.DF   EQU             0000_0000_0000_0000_0000_0100_0000_0000b

; Interrupt Flag: Changed with CLI/STI
x86.EFlags.IF   EQU             0000_0000_0000_0000_0000_0010_0000_0000b

; Trace Flag
x86.EFlags.TF   EQU             0000_0000_0000_0000_0000_0001_0000_0000b

; Sign Flag
x86.EFlags.SF   EQU             0000_0000_0000_0000_0000_0000_1000_0000b

; Zero Flag
x86.EFlags.ZF   EQU             0000_0000_0000_0000_0000_0000_0100_0000b

; Auxiliary carry Flag: Used as a half-carry for BCD calculations
x86.EFlags.AF   EQU             0000_0000_0000_0000_0000_0000_0001_0000b

; Parity Flag
x86.EFlags.PF   EQU             0000_0000_0000_0000_0000_0000_0000_0100b

; Always set
x86.EFlags.1    EQU             0000_0000_0000_0000_0000_0000_0000_0010b

; Carry Flag
x86.EFlags.CF   EQU             0000_0000_0000_0000_0000_0000_0000_0001b
