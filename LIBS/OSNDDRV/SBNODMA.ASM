ideal
p386
model flat,cpp

const

drv_ftab dd get_info,detect,init,shutdown,get_time_proc
drv_name db 'Sound Blaster (no DMA)',0
drv_par dw 7, 220h,210h,230h,240h,250h,260h,280h
	dw 0,0

dataseg

port dw 22Ch

codeseg

proc time_proc near
	push dx ax
	mov dx,[port]
	mov ah,10h
	call near sbout
	pop ax
	push ax
	mov ah,al
	call near sbout
	pop ax dx
	ret
endp

proc __sbnodma_drv
public __sbnodma_drv
	cmp ebx,4
	ja @@err
	shl ebx,2
	jmp [drv_ftab+ebx]
@@err:	xor ax,ax
	ret
endp

proc sbout near
@@1:	in al,dx
	or al,al
	js @@1
	mov al,ah
	out dx,al
	ret
endp

proc get_info
	mov ax,0
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	ret
endp

proc detect
	mov dx,ax
	add dx,12
	mov ecx,500
@@1:	in al,dx
	or al,al
	jns @@ok
	loop @@1
	xor ax,ax
	ret
@@ok:	mov ax,1
	ret
endp

proc init
	add ax,12
	mov [port],ax
	mov dx,ax
	mov ah,0D1h
	call near sbout
	ret
endp

proc shutdown
	mov dx,[port]
	mov ah,0D3h
	call near sbout
	ret
endp

proc get_time_proc
	mov ebx,offset time_proc
	ret
endp

end
