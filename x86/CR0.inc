;
; x86/CR0.inc
;

; The following are the bitfields in the CR0 register

; PaGing: Enable paging (when in Protected Mode)
x86.CR0.PG      EQU             1000_0000_0000_0000_0000_0000_0000_0000b

; Cache Disable: Disable memory cache
x86.CR0.CD      EQU             0100_0000_0000_0000_0000_0000_0000_0000b

; Not Write-through:  Disable writing cache contents back to memory
x86.CR0.NW      EQU             0010_0000_0000_0000_0000_0000_0000_0000b

; Alignment Mask: Enable memory alignment checking (see x86.EFlags.AC)
x86.CR0.AM      EQU             0000_0000_0000_0100_0000_0000_0000_0000b

; Write Protect: When set, prevents the Kernel from writing to Read-Only pages
x86.CR0.WP      EQU             0000_0000_0000_0001_0000_0000_0000_0000b

; Numeric Error: Enables native handling of FPU exceptions, rather than legacy
x86.CR0.NE      EQU             0000_0000_0000_0000_0000_0000_0010_0000b

; Extension Type: Indicates (legacy) support for FPU instructions
x86.CR0.ET      EQU             0000_0000_0000_0000_0000_0000_0001_0000b

; Task Switched: Set on every hardware task switch, so FPU instructions fault
x86.CR0.TS      EQU             0000_0000_0000_0000_0000_0000_0000_1000b

; EMulate: Set when no FPU is available, forcing software emulation
x86.CR0.EM      EQU             0000_0000_0000_0000_0000_0000_0000_0100b

; Monitor Coprocessor: Whether WAIT/FWAIT raise an FPU exception
x86.CR0.MP      EQU             0000_0000_0000_0000_0000_0000_0000_0010b

; Protection Enable: Enable Protected Mode segmentation
x86.CR0.PE      EQU             0000_0000_0000_0000_0000_0000_0000_0001b
