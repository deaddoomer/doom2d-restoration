ideal
p386
model flat,cpp

FLDW=100
FLDH=100

macro CutLin
local a
call CutLine
or	bx,bx
jnz	a
ret
a:
endm

udataseg

extrn scra:dword,scrx:word,scrw:word,scry:word,scrh:word
extrn fld:byte,tctab:byte

__c db ?

_xlen dw ?
_ylen dw ?
_stepy dd ?

codeseg

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

proc Z_drawmap uses ebx ecx esi edi, @@x:dword,@@y:dword
public Z_drawmap
mov	ax,[word @@x]
mov	bx,FLDW
mov	cx,[scrx]
mov	dx,[scrw]
CutLin
mov	[_xlen],bx
mov	dx,FLDW
sub	dx,bx
movzx edx,dx
mov	[_stepy],edx
mov	di,ax
sub	di,[word @@x]
mov	[word @@x],ax
mov	ax,[word @@y]
mov	bx,FLDH
mov	cx,[scry]
mov	dx,[scrh]
CutLin
mov	[_ylen],bx
mov	bx,ax
sub	ax,[word @@y]
imul ax,ax,FLDW
add	di,ax
mov	[word @@y],bx
movzx edi,di
lea esi,[fld]
add	esi,edi
mov ax,320
mul [word @@y]
add	ax,[word @@x]
movzx	edi,ax
add	edi,[scra]
mov ax,ds
mov es,ax
mov edx,edi
lea ebx,[tctab]
cld
	@@1:
	mov edi,edx
		movzx ecx,[_xlen]
		@@2:
		lodsb
		xlat
		stosb
		loop @@2
	add esi,[_stepy]
	add edx,320
	dec [_ylen]
	jnz @@1
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

proc V_manspr uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@c:dword
public V_manspr
prep_img
mov al,[byte @@c]
mov [__c],al
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
		cmp al,70h
		jb @@2c
		cmp al,7Fh
		ja @@2c
		sub al,70h
		add al,[__c]
		@@2c:
		stosb
		@@2b:
		loop @@2
	add esi,ebx
	add edx,320
	dec [_ylen]
	jnz @@1
ret
endp

proc V_manspr2 uses ebx ecx esi edi, @@x:dword,@@y:dword,@@adr:dword,@@c:dword
public V_manspr2
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
mov al,[byte @@c]
mov [__c],al
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
		cmp al,70h
		jb @@2c
		cmp al,7Fh
		ja @@2c
		sub al,70h
		add al,[__c]
		@@2c:
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

proc V_remap_rect uses ebx ecx esi edi, @@x:dword,@@w:dword,@@y:dword,@@h:dword,@@m:dword
public V_remap_rect
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
movzx esi,ax
add esi,[scra]
cld
mov ebx,[@@m]
mov dx,[word @@w]
	@@1:
	mov edi,esi
	movzx ecx,dx
	@@2:
		mov al,[edi]
		xlat
		stosb
		loop @@2
	add esi,320
	dec [word @@h]
	jnz @@1
ret
endp

proc V_maptoscr uses ebx ecx esi edi, @@x:dword,@@w:dword,@@y:dword,@@h:dword,@@m:dword
public V_maptoscr
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
	movzx ecx,[word @@w]
	mov edx,320
	sub edx,ecx
	shr cx,1
	mov ax,ds
	mov es,ax
	mov ebx,[@@m]
	cld
@@1:	push ecx
	@@2:
		lodsw
		xlat
		xchg al,ah
		xlat
		xchg al,ah
		stosw
		loop @@2
	pop ecx
	add esi,edx
	add edi,edx
	dec [word @@h]
	jnz @@1
	ret
endp

end
