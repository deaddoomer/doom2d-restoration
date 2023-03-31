ideal
p386
model flat,cpp

extrn srand:proc

dataseg

timer dd 0
btm dw 0,0
int8sav dd 0
int8sel dw 0
;rmsav dw 0,0
;rmptr dd 0
;rmsel dw 0
cnt dw 0
frq dw 0FFFFh
drv_on db 0

time_proc dd offset def_timeproc

const

;__timer_rm_proc db 50h,0B0h,20h,0E6h,20h,58h,0CFh

codeseg

proc def_timeproc
public def_timeproc
	ret
endp

proc T_set_timeproc uses bx, @@f:dword,@@p:dword
public T_set_timeproc
	mov dx,12h
	mov ax,34DCh
	div [word @@f]
	cmp ax,20
	jae @@1
	mov ax,20
@@1:	cli
	mov [frq],ax
	mov bx,ax
	mov al,36h
	out 43h,al
	mov al,bl
	out 40h,al
	mov al,bh
	out 40h,al
	mov eax,[@@p]
	mov [time_proc],eax
	sti
	ret
endp

proc T_init uses ebx ecx esi edi es
public T_init
	cmp [drv_on],0
	jne @@x
;	mov ax,100h
;	mov bx,1
;	int 31h
;	jc @@x
;	mov [rmsel],dx
;	movzx eax,ax
;	shl eax,4
;	mov [rmptr],eax
;	mov edi,eax
;	mov esi,offset __timer_rm_proc
;	push ds
;	pop es
;	cld
;	mov ecx,7
;	rep movsb
	mov ah,0
	int 1Ah
	mov [btm],dx
	mov [btm+2],cx
	push cx dx
	call srand
	pop eax
	cli
	mov ax,3508h
	int 21h
	mov [int8sav],ebx
	mov [int8sel],es
;	mov ax,200h
;	mov bl,8
;	int 31h
;	mov [rmsav],dx
;	mov [rmsav+2],cx
	push ds
	mov ax,2508h
	push cs
	pop ds
	mov edx,offset TimeDriver
	int 21h
	pop ds
;	mov ecx,[rmptr]
;	shr ecx,4
;	xor edx,edx
;	mov ax,201h
;	mov bl,8
;	int 31h
	mov bx,0FFFFh
	mov [frq],bx
	mov al,36h
	out 43h,al
	mov al,bl
	out 40h,al
	mov al,bh
	out 40h,al
	mov [drv_on],1
	sti
@@x:	ret
endp

proc T_done uses ebx ecx
public T_done
	cmp [drv_on],0
	je @@x
	cli
	mov al,36h
	out 43h,al
	xor al,al
	out 40h,al
	xor al,al
	out 40h,al
;	mov ax,201h
;	mov bl,8
;	mov dx,[rmsav]
;	mov cx,[rmsav+2]
;	int 31h
	push ds
	mov ax,2508h
	lds edx,[fword int8sav]
	int 21h
	pop ds
	mov [drv_on],0
	mov ah,1
	mov dx,[btm]
	mov cx,[btm+2]
	int 1Ah
;	mov ax,101h
;	mov dx,[rmsel]
;	int 31h
	sti
@@x:	ret
endp

proc TimeDriver
	push eax ds
	cld
	mov ax,DGROUP
	mov ds,ax
	mov ax,[frq]
	add [word timer],ax
	adc [word timer+2],0
	add [cnt],ax
	jnc @@1
	inc [dword btm]
	cmp ax,0FFFFh
	je @@2
@@1:	call [time_proc]
	mov al,20h
	out 20h,al
	pop ds eax
	iretd
@@2:	pushfd
	call [fword int8sav]
	call [time_proc]
	pop ds eax
	iretd
endp

public timer

end
