; Tsunami - a 4k intro released at Assembly 2007
; Copyright (C) 2007  Niko Kiirala <niko@kiirala.com>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

%define location(a) a wrt ..sym
	
%macro call 2
	xor rax,rax
	mov rdi, %2
	call %1
%endmacro

%macro call 3
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	call %1
%endmacro

%macro call 4
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	mov rdx, %4
	call %1
%endmacro

%macro call 5
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	mov rdx, %4
	mov rcx, %5
	call %1
%endmacro

%macro call 6
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	mov rdx, %4
	mov rcx, %5
	mov r8, %6
	call %1
%endmacro

%macro call 7
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	mov rdx, %4
	mov rcx, %5
	mov r8, %6
	mov r9, %7
	call %1
%endmacro

%macro call 8+
	xor rax,rax
	mov rdi, %2
	mov rsi, %3
	mov rdx, %4
	mov rcx, %5
	mov r8, %6
	mov r9, %7
	%rep %0 - 7
	push dword %0
	%rotate -1
	%endrep
	call %1
	add rsp, (%0 - 7) * 8
%endmacro
