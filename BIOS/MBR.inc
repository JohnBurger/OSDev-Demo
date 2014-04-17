;
; BIOS/MBR.inc
;

; These are the definitions for the standard BIOS Master Boot Record

; Some MBRs modify the freshly-loaded VBR (Volume Boot Record)!
; We need to reserve room (one byte) for this at this address
BIOS.MBR.BPB.Write EQU          BIOS.BPB.Extended.Base

; If we booted from a hard disk, ES:SI will be pointing to the relevant Partition
                STRUC           BIOS.MBR.Entry
.Active         RESB            1
.StartHead      RESB            1
.StartCylSect   RESW            1
.Type           RESB            1
.EndHead        RESB            1
.EndCylSect     RESW            1
.Start          RESD            1
.Size           RESD            1
                ENDSTRUC

; The two CylSect entries above have the following format. But don't be fooled!
; The HIGH bits of the Cylinder are in the LOW byte  - it's weirdly reversed!
;                   EQU         7654_3210_9800_0000
BIOS.MBR.Entry.Cyl  EQU         1111_1111_1100_0000b
BIOS.MBR.Entry.Sect EQU         0000_0000_0011_1111b
