ideal
p386
model flat,cpp

const

drv_ftab dd get_info,detect,init,shutdown,get_time_proc
drv_name db 'PC Speaker (1 bit)',0
drv_par dw 0,0,0

codeseg

proc time_proc near
	push ax
	rol al,1
	rcl ah,1
	rcl ah,1
	and ah,2
	in al,61h
	and al,0FCh
	or al,ah
	out 61h,al
	pop ax
	ret
endp

proc __pc1_drv
public __pc1_drv
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
	mov ax,1
	ret
endp

proc init
	in al,61h
	and al,0FCh
	or al,2
	out 61h,al
	ret
endp

proc shutdown
	ret
endp

proc get_time_proc
	mov ebx,offset time_proc
	ret
endp

end
