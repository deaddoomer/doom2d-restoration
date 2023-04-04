ideal
p386
model flat,cpp

macro delay
local a
	jmp short a
a:
endm

const

drv_ftab dd get_info,detect,init,shutdown,get_time_proc
drv_name db 'Adlib',0
drv_par dw 0,0,0

codeseg

;al=reg,ah=data
proc adlib near
	mov dx,388h
	out dx,al
rept 6
	in al,dx
endm
	inc dx
	mov al,ah
	out dx,al
	dec dx
rept 35
	in al,dx
endm
	ret
endp

proc time_proc near
	push dx
	mov dx,389h
	shr al,2
	out dx,al
	pop dx
	ret
endp

proc __adlib_drv
public __adlib_drv
	cmp ebx,4
	ja @@err
	shl ebx,2
	jmp [drv_ftab+ebx]
@@err:	xor ax,ax
	ret
endp

proc get_info near
	mov ax,0
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	ret
endp

proc detect near
	mov ax,-1
	ret
endp

proc init near
	mov ax,00000h
	call near adlib
	mov ax,02120h
	call near adlib
	mov ax,0F060h
	call near adlib
	mov ax,0F080h
	call near adlib
	mov ax,001C0h
	call near adlib
	mov ax,000E0h
	call near adlib
	mov ax,03F43h
	call near adlib
	mov ax,001B0h
	call near adlib
	mov ax,08FA0h
	call near adlib
	mov ax,02EB0h
	call near adlib
	mov al,0
	out 43h,al
	delay
	in al,40h
	mov dl,al
	delay
	in al,40h
	mov dh,al
	sub dx,952h
@@1:	mov al,0
	out 43h,al
	delay
	in al,40h
	mov bl,al
	delay
	in al,40h
	mov bh,al
	cmp bx,dx
	ja @@1
	mov ax,020B0h
	call near adlib
	mov ax,000A0h
	call near adlib
	mov dx,388h
	mov al,40h
	out dx,al
	ret
endp

proc shutdown near
	ret
endp

proc get_time_proc near
	mov ebx,offset time_proc
	ret
endp

end
