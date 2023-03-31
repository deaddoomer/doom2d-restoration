ideal
p386
jumps
model flat,cpp

BM_WALL=1

FLDW=100
FLDH=100
MAXTXW=16
MAXTXH=8
WD=200
HT=98

extrn srand:proc
extrn V_spr:proc,V_pic:proc,V_clr:proc
extrn V_remap_rect:proc

udataseg

extrn clrmap:byte,scra:dword
extrn walp:dword,walf:dword
extrn w_o:dword,w_x:dword,w_y:dword

dpmi_mem_info dd ?
	db 44 dup(?)

const

tctab db 0,54h,0A2h,72h,60h,0C8h,0DCh,0D6h,0FBh,48h,98h,255-10 dup(0)
public tctab

udataseg

__x dd ?
__y dd ?
__sx dd ?
__sy dd ?
__ex dd ?
__ey dd ?

codeseg

proc Z_drawfldt uses ebx ecx esi edi, @@f:dword
public Z_drawfldt
	mov eax,[w_x]
	sub eax,WD/2
	mov edx,eax
	and edx,7
	neg edx
	add edx,(1-MAXTXW)*8
	mov [__x],edx
	shr eax,3
	add eax,1-MAXTXW
	mov [__sx],eax
	add eax,MAXTXW+WD/8+1
	mov [__ex],eax
	mov eax,[w_y]
	sub eax,HT/2
	mov edx,eax
	and edx,7
	neg edx
	inc edx
	add edx,[w_o]
	add edx,(1-MAXTXH)*8
	mov [__y],edx
	shr eax,3
	add eax,1-MAXTXH
	mov [__sy],eax
	add eax,MAXTXH+HT/8+1
	mov [__ey],eax
	mov eax,FLDW
	mul [__sy]
	add eax,[__sx]
	add eax,[@@f]
	mov esi,eax
	cld
@@1:	mov eax,[__sy]
	or eax,eax
	js @@1z
	cmp eax,[__ey]
	jge @@1x
	cmp eax,FLDH
	jge @@1x
	push esi
	push [__x]
	mov edi,[__sx]
@@2:	lodsb
	or edi,edi
	js @@2z
	cmp edi,[__ex]
	jge @@2x
	cmp edi,FLDW
	jge @@2x
	or al,al
	jz @@2z
	movzx ebx,al
	push [word tctab+ebx]
	push 4
	mov eax,[__y]
	add eax,2
	push eax
	push 4
	mov eax,[__x]
	add eax,2
	push eax
	call V_clr
	add sp,18
@@2z:	add [__x],8
	inc edi
	jmp @@2
@@2x:	pop [__x]
	pop esi
@@1z:	add [__y],8
	inc [__sy]
	add esi,FLDW
	jmp @@1
@@1x:	ret
endp

proc Z_drawfld uses ebx ecx esi edi, @@f:dword
public Z_drawfld
	mov eax,[w_x]
	sub eax,WD/2
	mov edx,eax
	and edx,7
	neg edx
	add edx,(1-MAXTXW)*8
	mov [__x],edx
	shr eax,3
	add eax,1-MAXTXW
	mov [__sx],eax
	add eax,MAXTXW+WD/8+1
	mov [__ex],eax
	mov eax,[w_y]
	sub eax,HT/2
	mov edx,eax
	and edx,7
	neg edx
	inc edx
	add edx,[w_o]
	add edx,(1-MAXTXH)*8
	mov [__y],edx
	shr eax,3
	add eax,1-MAXTXH
	mov [__sy],eax
	add eax,MAXTXH+HT/8+1
	mov [__ey],eax
	mov eax,FLDW
	mul [__sy]
	add eax,[__sx]
	add eax,[@@f]
	mov esi,eax
	cld
@@1:	mov eax,[__sy]
	or eax,eax
	js @@1z
	cmp eax,[__ey]
	jge @@1x
	cmp eax,FLDH
	jge @@1x
	push esi
	push [__x]
	mov edi,[__sx]
@@2:	lodsb
	or edi,edi
	js @@2z
	cmp edi,[__ex]
	jge @@2x
	cmp edi,FLDW
	jge @@2x
	or al,al
	jz @@2z
	xor ah,ah
	movzx ebx,ax
	shl ebx,2
	mov eax,[walp+ebx]
;	or eax,eax
;	jz @@2z
	cmp eax,3
	jbe @@water
@@wall:
	push eax
	push [__y]
	push [__x]
	test [walf+ebx],1
	jnz short @@2b
	call V_pic
	jmp short @@2c
@@2b:	call V_spr
@@2c:	add sp,12
@@2z:	add [__x],8
	inc edi
	jmp @@2
@@2x:	pop [__x]
	pop esi
@@1z:	add [__y],8
	inc [__sy]
	add esi,FLDW
	jmp @@1
@@1x:	ret

;water effects

@@water:
	add al,7
	mov ah,al
	xor al,al
	movzx eax,ax
	add eax,offset clrmap
	push eax 8 [__y] 8 [__x]
	call V_remap_rect
	add sp,20
	jmp @@2z
endp

proc gotoxy uses bx, @@x:dword,@@y:dword
public gotoxy
	mov ah,2
	mov bh,0
	mov dl,[byte @@x]
	mov dh,[byte @@y]
	dec dl
	dec dh
	int 10h
	ret
endp

proc wherex uses bx cx
public wherex
	mov ah,3
	mov bh,0
	int 10h
	inc dl
	movzx eax,dl
	ret
endp

proc wherey uses bx cx
public wherey
	mov ah,3
	mov bh,0
	int 10h
	inc dh
	movzx eax,dh
	ret
endp

proc textmode
public textmode
	mov ax,3
	int 10h
	ret
endp

proc putcn uses bx cx, @@c:dword,@@a:dword,@@n:dword
public putcn
	mov ah,9
	mov bh,0
	mov bl,[byte @@a]
	mov al,[byte @@c]
	mov cx,[word @@n]
	int 10h
	ret
endp

proc cputstr uses bx esi, @@p:dword
public cputstr
	mov esi,[@@p]
@@1:	cld
	lodsb
	or al,al
	jz @@x
	mov ah,14
	int 10h
	jmp @@1
@@x:	ret
endp

proc cputch uses bx, @@c:byte
public cputch
	mov ah,14
	mov al,[@@c]
	int 10h
	ret
endp

proc randomize uses cx
public randomize
	mov ah,0
	int 1Ah
	push cx dx
	call srand
	pop eax
	mov ax,ds
	mov es,ax
	ret
endp

proc dpmi_memavl
public dpmi_memavl
	mov ax,ds
	mov es,ax
	lea edi,[dpmi_mem_info]
	mov ax,500h
	int 31h
	mov eax,[dpmi_mem_info]
	ret
endp

end
