;
; x86/CR3.inc
;
; CR3 (Control Register 3) is also known as the Page Directory Base Register
; (PDBR), because it's a Register that holds the Base of the Page Directory...
;

; The following are the bitfields in the CR3 register

; Page Directory Base: Physical address of Page Directory (Page aligned)
x86.CR3.PDB     EQU             1111_1111_1111_1111_1111_0000_0000_0000b

%if __CPU__>=486

; Page-level Cache Disable: Whether the PD is cached
x86.CR3.PCD     EQU             0100_0000_0000_0000_0000_0000_0001_0000b

; Page-level Write Through: Whether writes to the PD are written to memory
x86.CR3.PWT     EQU             0010_0000_0000_0000_0000_0000_0000_1000b

%endif ; >=486
