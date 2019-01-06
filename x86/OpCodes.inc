;
; x86/OpCodes.inc
;

%define         NOOP            090h    ; The op-code for NOP, for code Padding

; A useful pseudo-instruction to invoke the built-in Debugger - after it's set up!
%define         DEBUG           DB 0CCh ; INT 3 - NASM assembles this as 0CD 03h
