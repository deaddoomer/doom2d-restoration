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

extrn bright:byte,mixmap:byte,flametab:byte

extrn fld:byte,bmap:byte,fld_need_remap:byte,clrmap:byte
extrn walp:dword,walf:dword
extrn w_o:dword,w_x:dword,w_y:dword
extrn w_ht:dword;

dpmi_mem_info dd ?
	db 44 dup(?)

dataseg

rndseed dd 1

codeseg

proc Z_calc_time uses ebx, @@t:dword,@@h:dword,@@m:dword,@@s:dword
public Z_calc_time
	mov ax,[word @@t]
	mov dx,[word @@t+2]
	mov bx,65455
	div bx
	mov ebx,[@@h]
	mov [ebx],ax
	mov ax,dx
	xor dx,dx
	mov bx,1091
	div bx
	mov ebx,[@@m]
	mov [ebx],ax
	mov ax,dx
	xor dx,dx
	mov bx,18
	div bx
	mov ebx,[@@s]
	mov [ebx],ax
	ret
endp

proc BM_remapfld uses bx ecx esi edi
public BM_remapfld
  lea esi,[fld]
  lea edi,[bmap]
  mov dx,FLDH/4
  mov bl,BM_WALL
  mov bh,255-BM_WALL
  cld
@@l1:
  mov ecx,FLDW/4
@@l2:
  push esi
  mov ax,[esi+FLDW]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  mov ax,[esi+FLDW*2]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  mov ax,[esi+FLDW*3]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  lodsw
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  mov ax,[esi+FLDW]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  mov ax,[esi+FLDW*2]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  mov ax,[esi+FLDW*3]
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  je @@a1
  lodsw
  cmp al,1
  je @@a1
  cmp al,2
  je @@a1
  cmp ah,1
  je @@a1
  cmp ah,2
  jne @@a2
@@a1:
  or [edi],bl
  jmp @@nxt
@@a2:
  and [edi],bh
@@nxt:
  pop esi
  add esi,4
  inc edi
  loop @@l2
  add esi,FLDW*3
  dec dx
  jnz @@l1
  mov [fld_need_remap],0
  ret
endp

proc BM_clear uses ebx ecx, @@f:byte
public BM_clear
  lea ebx,[bmap]
  mov ecx,FLDW/4*FLDH/4/4
  mov al,[@@f]
  not al
  mov ah,al
  push ax ax
  pop eax
@@1:
  and [ebx],eax
  add ebx,4
  loop @@1
  and [ebx],al
  ret
endp

udataseg

__x dd ?
__y dd ?
__sx dd ?
__sy dd ?
__ex dd ?
__ey dd ?

codeseg

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
	mov eax,[w_ht]
	shr eax,1
	neg eax
	add eax,[w_y]
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
	mov eax,[w_ht]
	shr eax,3
	add eax,9
	add eax,[__sy]
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

proc K_slow uses bx
public K_slow
	mov ax,305h
	mov bx,31Fh
	int 16h
	ret
endp

proc K_fast uses bx
public K_fast
	mov ax,305h
	mov bx,0
	int 16h
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

proc smoke_sprf uses ebx
public smoke_sprf
	movzx ebx,[byte edi]
	add al,[bright+ebx]
	add al,060h
	xor al,0Fh
	mov bh,al
	mov al,[mixmap+ebx]
	stosb
	ret
endp

proc flame_sprf uses ebx
public flame_sprf
	movzx ebx,[byte edi]
	add al,[bright+ebx]
	mov bl,al
	mov al,[flametab+ebx]
	stosb
	ret
endp

end
