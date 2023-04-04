ideal
p386
model flat,cpp

BUFSZ=1024

const

drv_ftab dd get_info,detect,init,shutdown,set_dma
drv_name db 'Sound Blaster',0
drv_par dw 7, 220h,210h,230h,240h,250h,260h,280h
	dw 1, 1
	dw 9, 7,5,3,4,9,10,11,12,15

dataseg

port dw 220h
irq db 7
label irqsav pword
  dd 0980BC00h
  dw ?
mproc dd ?
dmabuf dd ?
bufsel dw 0
dmaf db 0
sbfreq db ?

proc __sb_drv
public __sb_drv
	cmp ebx,4
	ja @@err
	shl ebx,2
	jmp [drv_ftab+ebx]
@@err:	xor ax,ax
	ret
endp

proc sbout near
	mov dx,[port]
	add dx,12
@@1:	in al,dx
	or al,al
	js @@1
	mov al,ah
	out dx,al
	ret
endp

proc get_info
	mov ax,1
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	ret
endp

proc detect
	mov dx,ax
	call near reset
	cmp al,0AAh
	je @@ok
	xor ax,ax
	ret
@@ok:	mov ax,1
	ret
endp

proc init
	cli
	mov [port],ax
	mov [irq],dl
	mov dx,ax
	call near reset
	mov ah,35h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:	int 21h
	mov [dword irqsav],ebx
	mov [word irqsav+4],es
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@b
	add al,60h
@@b:	push cs
	pop ds
	mov edx,offset irqproc
	int 21h
	pop ds
	mov cl,[irq]
	mov ah,0FEh
	rol ah,cl
	mov dx,21h
	cmp [irq],8
	jb @@c
	mov dl,0A1h
@@c:	in al,dx
	and al,ah
	out dx,al
	mov ah,0D1h
	call near sbout
	sti
	ret
endp

proc shutdown
	cli
	mov dx,[port]
	add dx,14
	in al,dx
	mov dx,[port]
	call near reset
	mov ah,0D3h
	call near sbout
	cmp [bufsel],0
	je @@nobuf
	mov ax,101h
	mov dx,[bufsel]
	int 31h
@@nobuf:
	push ds
	mov ah,25h
	mov al,[irq]
	add al,8
	cmp al,16
	jb @@a
	add al,60h
@@a:	lds edx,[irqsav]
	int 21h
	pop ds
	mov cl,[irq]
	mov ah,1
	rol ah,cl
	mov dx,21h
	cmp [irq],8
	jb @@c
	mov dl,0A1h
@@c:	in al,dx
	or al,ah
	out dx,al
	sti
	ret
endp

proc reset
	add dx,6
	mov al,1
	out dx,al
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	in al,dx
	xor al,al
	out dx,al
	add dx,8
	xor cx,cx
@@1:	dec cx
	jz @@1x
	in al,dx
	or al,al
	jns @@1
@@1x:	sub dx,4
	in al,dx
	ret
endp

proc irqproc
	push eax ebx ecx edx esi edi ebp ds es
	mov ax,DGROUP
	mov ds,ax
	mov edi,[dmabuf]
	mov ecx,BUFSZ/2
	cmp [dmaf],0
	je @@1
	add edi,ecx
@@1:	push ds
	pop es
	call [mproc]
	xor [dmaf],1
	mov dx,[port]
	add dx,14
	in al,dx
	mov al,20h
	cmp [irq],8
	jb @@2
	out 0A0h,al
@@2:	out 20h,al
	pop es ds ebp edi esi edx ecx ebx eax
	iretd
endp

proc set_dma
	cli
	mov [dmaf],0
	mov [mproc],eax
	mov dx,0Fh
	mov ax,4240h
	div cx
	neg al
	mov [sbfreq],al
	mov ax,100h
	mov bx,(BUFSZ*2+15)/16
	int 31h
	jc @@x
	movzx eax,ax
	shl eax,4
	mov [bufsel],dx
	mov [dmabuf],eax
	movzx ecx,ax
	neg cx
	cmp cx,BUFSZ
	ja @@ok
	add [dmabuf],ecx
	add eax,ecx
@@ok:	push eax
	pop bx dx
	mov al,5
	out 0Ah,al
	xor al,al
	out 0Ch,al
	mov al,bl
	out 2,al
	mov al,bh
	out 2,al
	mov al,dl
	out 83h,al
	mov ax,BUFSZ-1
	out 3,al
	mov al,ah
	out 3,al
	mov al,58h+1
	out 0Bh,al
	mov al,1
	out 0Ah,al
	mov edi,[dmabuf]
	mov ecx,BUFSZ
	push ds
	pop es
	mov al,80h
	cld
	rep stosb
	mov ah,40h
	call near sbout
	mov ah,[sbfreq]
	call near sbout
	mov ah,48h
	call near sbout
	mov cx,BUFSZ/2-1
	mov ah,cl
	call near sbout
	mov ah,ch
	call near sbout
	mov ah,1Ch
	call near sbout
	xor cx,cx
	sti
	ret
@@x:	xor cx,cx
	sti
	ret
endp

end
