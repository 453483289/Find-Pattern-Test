; ++
;
; Copyright (c) dude719, Quarkslab.  All rights reserved.
;
; Module:
;
;	findpattern.asm
;
; Summary:
;
;    This module implements a findpattern routine
;
; Environment:
;
;    UEFI firmware
;
; --

;*********************************************************************
; .data section
.DATA

ALIGN 16

;*********************************************************************
; .text section
.CODE

;*********************************************************************
; Find a pattern (Wildcard is 0xCC)
;
; VOID* FindPattern(VOID* ImageBase, UINT32 ImageSize, UINT8* Pattern, UINT32 PatternSize);
;
; RCX = ImageBase
; RDX = ImageSize
; R8  = Pattern
; R9  = PatternSize
;
; Returns address that pattern is found or NULL if not found
; 
;*********************************************************************
FindPattern PROC

pattern_search_begin:
	push rcx
	push rdi ; backup som regs
	push rsi
	push r10
	xor rdi, rdi ; zero out some regs
	xor rsi, rsi
	xor r10, r10
	sub edx, r9d ; sub pattern size from image size
pattern_search_loop: ; main loop
	cmp esi, edx ; check if at end of the image
	jae pattern_search_not_found
	mov r10b, byte ptr [r8 + rdi]
	cmp r10b, byte ptr [rcx + rdi]
	je pattern_search_matched
	cmp r10b, 0CCh ; check wildcard
	jne pattern_search_continue
pattern_search_matched:
	inc edi
	cmp edi, r9d
	jae pattern_search_exit
	jmp pattern_search_loop
pattern_search_continue:
	xor rdi, rdi
	inc rcx
	inc esi
	jmp pattern_search_loop
pattern_search_not_found:
	xor rcx, rcx ; return NULL
pattern_search_exit:
	mov rax, rcx ; return value that pattern was found at
	pop r10
	pop rsi
	pop rdi
	pop rcx
	ret

FindPattern ENDP

END
