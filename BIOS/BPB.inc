;
; BIOS/BPB.inc
;

; These are the definitions for the standard BIOS Parameter Block (BPB) in the
; middle of a Volume Boot Record (VBR - the first sector of a Volume).
; Note that the VBR is different from the Master Boot Record (which holds
; the Partition Table) - floppies only have a VBR, while hard drives and USB
; memory sticks have both: an MBR at the first sector, then (probaby) VBRs at
; each of the defined partitions (also known as Volumes, hence VBR).

BIOS.BPB.Base     EQU           000Bh

                  STRUC         BIOS.BPB
.Sector.Bytes     RESW          1               ; Usually 512
.Cluster.Sector   RESB          1               ; Floppy? 1. Hard disk? 8 or more
.Sector.Reserved  RESW          1               ; 1 (this one!) but can be more
.FAT.Copies       RESB          1               ; Usually 2
.RootDir.Entries  RESW          1               ; Floppy? 224
.Sector.Num.Small RESW          1               ; For large disks: 0
.Media.Type       RESB          1               ; See BIOS.BPB.Media.* below
.FAT.Sectors      RESW          1               ; Dependent on size of disk
.Track.Sectors    RESW          1               ; Floppy? 18. Hard disk? 63.
.Head.Number      RESW          1               ; Floppy? 1 or 2. Hard disk? 255
.Sector.Hidden    RESD          1               ; Distance from beginning of disk
.Sector.Num.Big   RESD          1               ; Size of Volume
                  ENDSTRUC

BIOS.BPB.Media.1         EQU    1111_0000b      ; Always 1
BIOS.BPB.Media.Fixed     EQU    0000_0000b
BIOS.BPB.Media.Removable EQU    0000_1000b
BIOS.BPB.Media.Tracks.80 EQU    0000_0000b      ; Tracks per head
BIOS.BPB.Media.Tracks.40 EQU    0000_0100b
BIOS.BPB.Media.Sectors.9 EQU    0000_0000b      ; Sectors per track
BIOS.BPB.Media.Sectors.8 EQU    0000_0010b
BIOS.BPB.Media.Sides.1   EQU    0000_0000b      ; Sides on disk
BIOS.BPB.Media.Sides.2   EQU    0000_0001b

BIOS.BPB.Extended.Base EQU      0024h

                  STRUC         BIOS.BPB.Extended ; At 0024h
.Drive            RESB          1                ; Drive number (stored by MBR?)
.ChkDsk           RESB          1                ; Reserved
.Signature        RESB          1                ; 40h used by DOS
.Volume.Serial    RESD          1
.Volume.Label     RESB          11
.FileSystem.Type  RESB          8
                  RESB          8
                  ENDSTRUC

BIOS.BPB.Extended.ChkDsk.Request EQU 0000_0010b
BIOS.BPB.Extended.ChkDsk.Dirty   EQU 0000_0001b
