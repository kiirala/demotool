BITS 64
%include "amd64_call.asm"

%include "sdlinc.asm"
%include "sizes.asm"
%include "config.asm"
	
extern SDL_Quit, SDL_Init, SDL_SetVideoMode, SDL_WM_SetCaption
extern SDL_PollEvent
	
	;; Omat funktiot, m‰‰ritelty muissa tiedostoissa
extern load, unload, simpleRender, resize

global _start:function

section .text

virhe:	
	xor eax,eax
	inc eax			; syscall write
	mov edi, eax		; stdout
	mov rsi, virhe_msg
	mov edx, virhe_msg_end - virhe_msg
	syscall

	call SDL_Quit		; Tapetaan SDL, n‰yttˆ j‰rkev‰‰n tilaan
	mov eax, 60		; syscall exit
	xor edi,edi
	inc edi
	syscall
	
_start:
	;call dynlink		

	;; Alustetaan SDL
	call SDL_Init, dword SDL_INIT_EVERYTHING
	test eax, eax
	jnz virhe

	;; Asetetaan n‰yttˆtila
%ifdef FULLSCREEN
	call SDL_SetVideoMode, scrWidth, scrHeight, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE | SDL_HWSURFACE | SDL_HWACCEL | SDL_FULLSCREEN
%else
	call SDL_SetVideoMode, scrWidth, scrHeight, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE | SDL_HWSURFACE | SDL_HWACCEL
%endif	
	test eax,eax
	jz virhe
	mov [screen], rax	; Talletetaan pointteri -> screen

	;; Asetetaan ohjelmaikkunalle nimi
	mov rdi, nimi
	mov rsi, rdi
	call SDL_WM_SetCaption

	;; -----------------------
	;; --- Muuta alustusta ---
	;; -----------------------
	call load
	call resize, dword scrWidth, dword scrHeight
	
	;; -------------------
	;; --- P‰‰silmukka ---
	;; -------------------
.loop:	mov rdi, event ; Tutkitaan saapuneet eventit
	call SDL_PollEvent
	test eax, eax
	jz .eventDone
	cmp [event], byte SDL_KEYDOWN
	je quit
	cmp [event], byte SDL_QUIT
	je quit
	jmp .loop

.eventDone:
	;; Kutsutaan piirtoa
	call simpleRender

	;; Palataan takaisin p‰‰silmukan alkuun
	;; mov eax, [do_continue]
	;; test eax,eax
	;; jz .loop
	jmp .loop

quit:
	;; Sammutetaan ohjelma
	call unload
	call SDL_Quit
	mov eax, 60		; syscall exit
	xor edi,edi
	syscall

section .rodata
virhe_msg:	db "Error.", 10
virhe_msg_end:
nimi:		db "Toon tree", 0

section .bss align=8
screen:	resq 1
kohtax:	resd 1
kohtay:	resd 1
event:	resb sizeof_SDL_Event
