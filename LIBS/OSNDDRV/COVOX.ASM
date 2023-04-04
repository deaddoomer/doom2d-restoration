ideal
p386
model flat,cpp

const

drv_ftab dd get_info,detect,init,shutdown,get_time_proc
drv_name db 'Covox Speech Thing',0
drv_par dw 4, 1,2,3,4
	dw 0,0

dataseg

port dw 378h

codeseg

proc get_lpt near
	dec ax
	add ax,ax
	movzx ebx,ax
	mov ax,[ds:ebx+408h]
	ret
endp

proc time_proc near
	push dx
	mov dx,[port]
	out dx,al
	pop dx
	ret
endp

proc __covox_drv
public __covox_drv
	cmp ebx,4
	ja @@err
	shl ebx,2
	jmp [drv_ftab+ebx]
@@err:	xor ax,ax
	ret
endp

proc get_info
	mov ax,0
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	ret
endp

proc detect
	call near get_lpt
	ret
endp

proc init
	call near get_lpt
	mov [port],ax
	mov dx,ax
	mov al,80h
	out dx,al
	ret
endp

proc shutdown
	mov dx,[port]
	mov al,80h
	out dx,al
	ret
endp

proc get_time_proc
	mov ebx,offset time_proc
	ret
endp

end
