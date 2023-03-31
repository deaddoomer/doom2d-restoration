ideal
p386
model flat,cpp

extrn error:proc

macro delay
local a,b,c
jmp short a
a:
jmp short b
b:
jmp short c
c:
endm

macro CutLin
local a
call CutLine
or	bx,bx
jnz	a
ret
a:
endm

dataseg

vp_waitrr db 1
public vp_waitrr


udataseg

scra dd ?
scrx dw ?
scrw dw ?
scry dw ?
scrh dw ?

pal_tmp db 256*3 dup(?)

_xlen dw ?
_ylen dw ?
_stepy dd ?

vm_s dd ?
vm_l dw ?
vm_buf db 320 dup(?)

public scra,scrx,scrw,scry,scrh
public pal_tmp


codeseg

proc V_wait
public V_wait
	mov dx,3DAh
@@1:	in al,dx
	test al,8
	jnz @@1
@@2:	in al,dx
	test al,8
	jz @@2
	ret
endp

proc V_init uses ecx edi
public V_init
mov	ah,15
int	10h
push	ax
mov	ax,19
int	10h
mov	ah,15
int	10h
cmp	al,19
pop	ax
je @@1
mov	ah,0
int	10h
push 3		;HARDERR
push 1		;LIB
push 2		;VGALIB
call error
add sp,12
mov ax,1
ret
	@@1:
mov ax,ds
mov es,ax
mov edi,0A0000h
mov ecx,16384
xor eax,eax
cld
rep stosd
  mov [scra],0A0000h
  mov [scrx],0
  mov [scry],0
  mov [scrw],320
  mov [scrh],200
xor ax,ax
ret
endp

proc V_done
public V_done
	mov ah,15
	int 10h
	cmp al,3
	je @@x
	mov ax,3
	int 10h
@@x:	ret
endp

proc V_dot uses ebx, @@x:dword,@@y:dword,@@c:dword
public V_dot
	mov ax,[scrx]
	cmp [word @@x],ax
	jl @@ex
	add ax,[scrw]
	cmp [word @@x],ax
	jge @@ex
	mov ax,[scry]
	cmp [word @@y],ax
	jl @@ex
	add ax,[scrh]
	cmp [word @@y],ax
	jge @@ex
	mov ax,320
	mul [word @@y]
	add ax,[word @@x]
	movzx ebx,ax
	add ebx,[scra]
	mov al,[byte @@c]
	mov [ebx],al
@@ex:	ret
endp

proc CutLine near uses si di
;ax=pos,bx=len,cx=spos,dx=slen
mov	si,cx
add	si,dx
cmp	ax,si
jl	@@a
	mov	bx,0
	ret
@@a:
cmp	ax,cx
jge	@@1
add	ax,bx
cmp	ax,cx
jg	@@b
	mov	bx,0
	ret
@@b:
mov	bx,ax
sub	bx,cx
mov	ax,cx
@@1:
mov	di,ax
add	di,bx
cmp	di,si
jng	@@1a
	mov	bx,si
	sub	bx,ax
@@1a:
ret
endp

macro prep_img
mov	esi,[@@adr]
mov	ax,[word @@x]
sub ax,[esi+4]
mov [word @@x],ax
mov	bx,[esi]
mov	cx,[scrx]
mov	dx,[scrw]
CutLin
mov	[_xlen],bx
mov	dx,[esi]
sub	dx,bx
movzx edx,dx
mov	[_stepy],edx
mov	di,ax
sub	di,[word @@x]
mov	[word @@x],ax
mov	ax,[word @@y]
sub ax,[esi+6]
mov [word @@y],ax
mov	bx,[esi+2]
mov	cx,[scry]
mov	dx,[scrh]
CutLin
mov	[_ylen],bx
mov	bx,ax
sub	ax,[word @@y]
mul	[word esi]
add	di,ax
mov	[word @@y],bx
movzx edi,di
add	esi,edi
add	esi,8
mov ax,320
mul [word @@y]
add	ax,[word @@x]
movzx	edi,ax
add	edi,[scra]
mov ax,ds
mov es,ax
endm

proc V_pic uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_pic
	prep_img
	mov edx,edi
	movzx ebx,[_xlen]
	mov eax,[_stepy]
	cld
@@1:
	mov edi,edx
	mov ecx,ebx
	rep movsb
	add esi,eax
	add edx,320
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr1color uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@c:dword
public V_spr1color
prep_img
mov ah,[byte @@c]
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		mov al,ah
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,320
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_spr
prep_img
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,320
	dec [_ylen]
	jnz @@1
ret
endp

proc V_sprf uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@f:dword
public V_sprf
prep_img
mov edx,edi
mov ebx,[_stepy]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		or al,al
		jnz @@2a
		inc edi
		jmp short @@2b
		@@2a:
		call [@@f]
		@@2b:
		loop @@2
	add esi,ebx
	add edx,320
	dec [_ylen]
	jnz @@1
ret
endp

proc V_spr2 uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword
public V_spr2
mov	esi,[@@adr]
mov	ax,[word @@x]
sub ax,[esi]
add ax,[esi+4]
mov [word @@x],ax
mov	bx,[esi]
mov	cx,[scrx]
mov	dx,[scrw]
CutLin
mov	[_xlen],bx
mov	dx,[esi]
sub	dx,bx
movzx edx,dx
mov	[_stepy],edx
mov di,[word @@x]
add di,[esi]
sub di,ax
sub di,bx
add ax,bx
dec ax
mov	[word @@x],ax
mov	ax,[word @@y]
sub ax,[esi+6]
mov [word @@y],ax
mov	bx,[esi+2]
mov	cx,[scry]
mov	dx,[scrh]
CutLin
mov	[_ylen],bx
mov	bx,ax
sub	ax,[word @@y]
mul	[word ptr esi]
add	di,ax
mov	[word @@y],bx
movzx	edi,di
add	esi,edi
add	esi,8
mov ax,320
mul [word @@y]
add	ax,[word @@x]
movzx	edi,ax
add	edi,[scra]
mov ax,ds
mov es,ax
mov edx,edi
mov ebx,[_stepy]
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		cld
		lodsb
		or al,al
		jnz @@2a
		dec edi
		jmp short @@2b
		@@2a:
		std
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,320
	dec [_ylen]
	jnz @@1
cld
ret
endp

proc V_clr uses ebx ecx si edi, @@x:dword,@@w:dword,@@y:dword,@@h:dword,@@color:dword
public V_clr
mov ax,[word @@x]
mov bx,[word @@w]
mov cx,[scrx]
mov dx,[scrw]
CutLin
mov [word @@x],ax
mov [word @@w],bx
mov ax,[word @@y]
mov bx,[word @@h]
mov cx,[scry]
mov dx,[scrh]
CutLin
mov [word @@y],ax
mov [word @@h],bx
mov ax,ds
mov es,ax
mov ax,320
mul [word @@y]
add ax,[word @@x]
movzx ebx,ax
add ebx,[scra]
cld
mov al,[byte @@color]
mov si,[word @@h]
mov dx,[word @@w]
	@@1:
	mov edi,ebx
	movzx ecx,dx
	rep stosb
	add ebx,320
	dec si
	jnz @@1
ret
endp

proc VP_wait
public VP_wait
	cmp [vp_waitrr],0
	je @@x
	mov dx,3DAh
@@1:	in al,dx
	test al,8
	jnz @@1
@@2:	in al,dx
	test al,8
	jz @@2
@@x:	ret
endp

proc VP_getall uses ecx edi, @@p:dword
public VP_getall
	call VP_wait
	mov edi,[@@p]
	cld
	mov dx,3C7h
	xor al,al
	out dx,al
	delay
	mov dl,0C9h
	mov ecx,256*3
@@1:	in al,dx
	delay
	stosb
	loop @@1
	ret
endp

proc VP_setall uses ecx esi, @@p:dword
public VP_setall
	call VP_wait
	mov esi,[@@p]
	mov dx,3C8h
	xor al,al
	out dx,al
	delay
	mov dx,3C9h
	mov ecx,256*3
	cld
@@1:	lodsb
	out dx,al
	delay
	loop @@1
	ret
endp

proc VP_set uses ecx esi, @@p:dword,@@r:dword,@@n:dword
public VP_set
	call VP_wait
	mov dx,3C8h
	mov al,[byte @@r]
	out dx,al
	delay
	inc dx
	movzx ecx,[word @@n]
	mov eax,ecx
	add ecx,ecx
	add ecx,eax
	mov esi,[@@p]
@@1:	lodsb
	out dx,al
	delay
	loop @@1
	ret
endp

proc VP_fill uses ecx, @@r:dword,@@g:dword,@@b:dword
public VP_fill
	call VP_wait
	mov dx,3C8h
	xor al,al
	out dx,al
	delay
	inc dx
	mov ecx,256
@@1:	mov al,[byte @@r]
	out dx,al
	delay
	mov al,[byte @@g]
	out dx,al
	delay
	mov al,[byte @@b]
	out dx,al
	delay
	loop @@1
	ret
endp

proc VP_tocolor uses ecx esi edi, @@pal:dword,@@R:dword,@@G:dword,@@B:dword,@@N:dword,@@S:dword
public VP_tocolor
	mov edi,[@@pal]
	mov esi,offset pal_tmp
	mov ecx,256
@@1:	mov al,[byte @@R]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	mov al,[byte @@G]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	mov al,[byte @@B]
	sub al,[edi]
	imul [byte @@S]
	idiv [byte @@N]
	add al,[edi]
	mov [esi],al
	inc esi
	inc edi
	loop @@1
	ret
endp

proc VP_topal uses ecx esi edi, @@p1:dword,@@p2:dword,@@N:dword,@@S:dword
public VP_topal
	push ds
	pop es
	mov edi,offset pal_tmp
	mov esi,[@@p1]
	mov ecx,256*3/4
	cld
	rep movsd
	mov esi,offset pal_tmp
	mov edi,[@@p2]
	mov ecx,256*3
@@2:	mov al,[edi]
	sub al,[esi]
	imul [byte @@S]
	idiv [byte @@N]
	add [esi],al
	inc esi
	inc edi
	loop @@2
	ret
endp

proc VM_line0 uses esi edi, @@x:word,@@w:word,@@len:word;esi=>data
	mov ax,[@@x]
	mov bx,[@@w]
	mov cx,[scrx]
	mov dx,[scrw]
	CutLin
	push bx
	mov cx,ax
	sub ax,[@@x]
	mov [@@x],cx
	mul [@@len]
	div [@@w]
	movzx eax,ax
	add esi,eax
	mov bx,dx
	neg bx
	mov ax,[@@x]
	movsx eax,ax
	mov [vm_s],eax
	pop ax
	mov [@@x],ax
	mov [vm_l],ax
	lea edi,[vm_buf]
@@1:	inc esi
	mov ax,[@@w]
	add ax,bx
	xor dx,dx
	div [@@len]
	mov bx,dx
	movzx ecx,ax
	jcxz @@1
	mov al,[esi-1]
@@2:	mov [edi],al
	inc edi
	dec [@@x]
	loopnz @@2
	jnz @@1
	ret
endp

proc VM_line1 uses esi edi, @@x:word,@@w:word,@@len:word;esi=>data,edi=>scr
	mov ax,[@@x]
	mov bx,[@@w]
	mov cx,[scrx]
	mov dx,[scrw]
	CutLin
	push bx
	mov cx,ax
	sub ax,[@@x]
	mov [@@x],cx
	mul [@@len]
	div [@@w]
	movzx eax,ax
	add esi,eax
	mov bx,dx
	neg bx
	movsx eax,[@@x]
	add edi,eax
	pop [@@x]
@@1:	inc esi
	mov ax,[@@w]
	add ax,bx
	xor dx,dx
	div [@@len]
	mov bx,dx
	movzx ecx,ax
	jcxz @@1
	mov al,[esi-1]
@@2:	or al,al
	jnz @@2a
	inc edi
	jmp short @@2z
@@2a:	stosb
@@2z:	dec [@@x]
	loopnz @@2
	jnz @@1
	ret
endp

proc VM_spr uses ebx ecx esi edi, @@x:dword,@@y:dword,@@sx:dword,@@sy:dword,@@p:dword
public VM_spr
local @@h:word,@@w:word,@@len:word,@@err:word
	mov esi,[@@p]
	mov ax,[esi]
	mov [word @@w],ax
	mov ax,[esi+2]
	mov [@@len],ax
	mov ax,[esi+4]
	imul [word @@sx]
	idiv [@@w]
	sub [word @@x],ax
	mov ax,[esi+6]
	imul [word @@sy]
	idiv [@@len]
	sub [word @@y],ax
	mov ax,[word @@y]
	mov bx,[word @@sy]
	mov cx,[scry]
	mov dx,[scrh]
	CutLin
	mov [@@h],bx
	mov cx,ax
	sub ax,[word @@y]
	mov [word @@y],cx
	mul [@@len]
	div [word @@sy]
	neg dx
	mov [@@err],dx
	mul [@@w]
	movzx eax,ax
	add esi,eax
	add esi,8
	mov ax,320
	mul [word @@y]
	movzx edi,ax
	add edi,[scra]
	mov ax,ds
	mov es,ax
	cld
@@1:	mov ax,[word @@sy]
	add ax,[@@err]
	xor dx,dx
	div [@@len]
	mov [@@err],dx
	movzx ecx,ax
	jcxz @@1z
	dec cx
	jnz @@3
	push [@@w]
	push [word @@sx]
	push [word @@x]
	call VM_line1
	add sp,6
	add edi,320
	dec [@@h]
	jnz @@1z
	ret
@@3:	inc cx
	push ecx
	push [@@w]
	push [word @@sx]
	push [word @@x]
	call VM_line0
	add sp,6
	pop ecx
@@2:	push esi
	push edi
	push cx
	lea esi,[vm_buf]
	add edi,[vm_s]
	movzx ecx,[vm_l]
@@2a:	lodsb
	or al,al
	jnz @@2b
	inc edi
	jmp short @@2z
@@2b:	stosb
@@2z:	loop @@2a
	pop cx
	pop edi
	pop esi
	add edi,320
	dec [@@h]
	loopnz @@2
	jnz @@1z
	ret
@@1z:	movzx eax,[@@w]
	add esi,eax
	jmp @@1
endp

proc V_setrect near @@x:dword,@@w:dword,@@y:dword,@@h:dword
public V_setrect
	mov ax,[word @@x]
	mov [scrx],ax
	mov ax,[word @@y]
	mov [scry],ax
	mov ax,[word @@w]
	mov [scrw],ax
	mov ax,[word @@h]
	mov [scrh],ax
	ret
endp

proc V_setscr near @@p:dword
public V_setscr
	mov eax,[@@p]
	or eax,eax
	jnz @@1
	mov eax,0A0000h
@@1:	mov [scra],eax
	ret
endp

proc V_copytoscr uses ebx ecx esi edi, @@x:dword,@@w:dword,@@y:dword,@@h:dword
public V_copytoscr
	mov ax,[word @@w]
	add ax,[word @@x]
	add ax,3
	and ax,0FFFCh
	and [word @@x],0FFFCh
	sub ax,[word @@x]
	mov [word @@w],ax
	mov ax,320
	mul [word @@y]
	add ax,[word @@x]
	movzx esi,ax
	mov edi,esi
	add esi,[scra]
	add edi,0A0000h
	movzx edx,[word @@w]
	mov ebx,320
	sub ebx,edx
	shr dx,2
	mov ax,ds
	mov es,ax
	cld
@@1:	movzx ecx,dx
	rep movsd
	add esi,ebx
	add edi,ebx
	dec [word @@h]
	jnz @@1
	ret
endp

end
