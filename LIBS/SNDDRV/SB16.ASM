ideal
p386
model flat,cpp

BUFSZ=2048

const

drv_ftab dd get_info,detect,init,shutdown,set_dma
drv_name db 'Sound Blaster 16/AWE32',0
drv_par dw 7, 220h,210h,230h,240h,250h,260h,280h
	dw 8, 1,5,0,2,3,6,7,4
	dw 9, 7,5,3,4,9,10,11,12,15

dmapage db 87h,83h,81h,82h,8Fh,8Bh,89h,8Ah
dmaofs db 0,2,4,6,0C0h,0C4h,0C8h,0CCh
dmacnt db 1,3,5,7,0C2h,0C6h,0CAh,0CEh
dmamask db 0Ah,0Ah,0Ah,0Ah,0D4h,0D4h,0D4h,0D4h
dmamode db 0Bh,0Bh,0Bh,0Bh,0D6h,0D6h,0D6h,0D6h
dmaff db 0Ch,0Ch,0Ch,0Ch,0D8h,0D8h,0D8h,0D8h

dataseg

port dw 220h
dma dd 1
irq db 7
label irqsav pword
  dd 0
  dw 0
mproc dd 0
dmabuf dd 0
bufsel dw 0
dmaf db 0
sbfreq db 0

codeseg

proc __sb16_drv
public __sb16_drv
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
	mov ax,3
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
	and ecx,0Fh
	mov [dma],ecx
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
	shr ecx,1
	call [mproc]
	xor [dmaf],1
	mov dx,[port]
	add dx,10
	in al,dx
	add dx,4
	in al,dx
	inc dx
	in al,dx

	mov ah,047h
	call near sbout

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
	movzx ecx,cx
;	shl ecx,2
	mov [dmaf],0
	mov [mproc],eax
	xor edx,edx
	mov eax,0F4240h
	div ecx
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
@@ok:
	cmp [dma],4
	jb @@1
	shr eax,1
	push eax
	pop bx cx
	shl cl,1
	push cx bx
	pop eax
@@1:	push eax
	pop bx cx

	xor edx,edx
	mov eax,[dma]
	mov dl,[dmamask+eax]
	and al,3
	or al,4
	out dx,al

	xor al,al
	mov edx,[dma]
	mov dl,[dmaff+edx]
	out dx,al

	mov al,cl
	mov edx,[dma]
	mov dl,[dmapage+edx]
	out dx,al

	mov al,bl
	mov edx,[dma]
	mov dl,[dmaofs+edx]
	out dx,al
	mov al,bh
	out dx,al

	mov ax,BUFSZ
	cmp [dma],4
	jb @@2
	shr ax,1
@@2:	dec ax
	mov edx,[dma]
	mov dl,[dmacnt+edx]
	out dx,al
	mov al,ah
	out dx,al

	mov eax,[dma]
	and al,3
	or al,58h
	mov edx,[dma]
	mov dl,[dmamode+edx]
	out dx,al

	mov eax,[dma]
	and al,3
	mov edx,[dma]
	mov dl,[dmamask+edx]
	out dx,al

	mov edi,[dmabuf]
	mov ecx,BUFSZ
	push ds
	pop es
	mov al,80h
	cld
	rep stosb

	mov ah,0DAh
	call near sbout

	mov ah,40h
	call near sbout
	mov ah,[sbfreq]
	call near sbout

	mov ah,047h
	call near sbout

	mov ah,0B6h
	call near sbout
	mov ah,00h
	call near sbout
	mov cx,BUFSZ/4-1
	mov ah,cl
	call near sbout
	mov ah,ch
	call near sbout

	mov ah,047h
	call near sbout

	xor cx,cx
	sti
	ret
@@x:	xor cx,cx
	sti
	ret
endp

end
