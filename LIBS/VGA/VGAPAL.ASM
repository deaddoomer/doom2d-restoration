ideal
p386
model flat,cpp

RMUL=77
GMUL=151
BMUL=28

udataseg

pal db 256*4 dup(?)
palf dw ?

dataseg

paln dw 0

codeseg

proc V_remap uses ebx ecx edi, @@p:dword,@@m:dword
public V_remap
	mov edi,[@@p]
	mov ebx,[@@m]
	mov ax,[edi]
	mul [word edi+2]
	movzx ecx,ax
	add edi,8
	cld
@@1:	mov al,[edi]
	segds xlat
	stosb
	loop @@1
	ret
endp

proc VP_brightness uses bx, @@r:dword,@@g:dword,@@b:dword
public VP_brightness
	mov al,RMUL
	mul [byte @@r]
	mov bx,ax
	mov al,GMUL
	mul [byte @@g]
	add bx,ax
	mov al,BMUL
	mul [byte @@b]
	add ax,bx
	mov al,ah
	xor ah,ah
	ret
endp

proc VP_start_search uses bx ecx esi edi, @@p:dword,@@n:dword,@@f:dword
public VP_start_search
	mov esi,[@@p]
	lea edi,[pal]
	movzx ecx,[word @@n]
	mov [paln],cx
	mov ax,[word @@f]
	mov [palf],ax
	cld
	mov ax,ds
	mov es,ax
@@1:	lodsb
	stosb
	mov ah,RMUL
	mul ah
	mov bx,ax
	lodsb
	stosb
	mov ah,GMUL
	mul ah
	add bx,ax
	lodsb
	stosb
	mov ah,BMUL
	mul ah
	add ax,bx
	mov al,ah
	stosb
	loop @@1
	ret
endp

macro normcolor c
local a1,a2
	cmp c,0
	jnl a1
	mov c,0
	jmp short a2
a1:	cmp c,63
	jng a2
	mov c,63
a2:
endm

macro abs
local a1
	jns a1
	neg al
a1:
endm

proc VP_findcolor uses bx ecx esi di, @@r:dword,@@g:dword,@@b:dword
public VP_findcolor
	normcolor < [word @@r] >
	normcolor < [word @@g] >
	normcolor < [word @@b] >
	mov al,RMUL
	mul [byte @@r]
	mov bx,ax
	mov al,GMUL
	mul [byte @@g]
	add bx,ax
	mov al,BMUL
	mul [byte @@b]
	add bx,ax
;bh=brightness  bl=best  di=best_diff
	xor bl,bl
	mov di,0FFFFh
	lea esi,[pal]
	xor cx,cx
	cld
@@1:	lodsb
	sub al,[byte @@r]
	abs
	xor ah,ah
	mov dx,ax
	lodsb
	sub al,[byte @@g]
	abs
	xor ah,ah
	add dx,ax
	lodsb
	sub al,[byte @@b]
	abs
	xor ah,ah
	add dx,ax
	lodsb
	cmp dx,di
	jnb @@1a
	mov di,dx
	mov bl,cl
@@1a:	inc cx
	cmp cx,[paln]
	jb @@1
	mov al,bl
	xor ah,ah
	add ax,[palf]
	ret
endp

end
