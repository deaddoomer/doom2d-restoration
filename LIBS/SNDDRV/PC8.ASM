ideal
p386
model flat,cpp

MAXZ=1000

const

drv_ftab dd get_info,detect,init,shutdown,get_time_proc
drv_name db 'PC Speaker (8 bit)',0
drv_par dw 0,0,0
xtab	db  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
	db  2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4
	db  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5
	db  5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
	db  6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8
	db  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9
	db  9, 9,10,10,10,10,11,11,12,12,13,14,14,15,16,17
	db 17,18,19,20,21,22,23,24,26,27,28,29,30,31,33,34
	db 35,36,38,39,40,41,43,44,45,46,48,49,50,51,52,53
	db 54,55,57,58,58,59,60,61,62,63,64,64,65,66,66,67
	db 67,67,67,67,67,68,68,68,68,68,68,68,68,68,68,68
	db 68,68,69,69,69,69,69,69,69,69,69,69,69,70,70,70
	db 70,70,70,70,70,70,70,70,70,70,71,71,71,71,71,71
	db 71,71,71,71,71,72,72,72,72,72,72,72,72,72,72,72
	db 72,72,73,73,73,73,73,73,73,73,73,73,73,74,74,74
	db 74,74,74,74,74,74,74,74,74,74,75,75,75,75,75,75

dataseg

lastz dw 0

codeseg

proc time_proc near
	push ax ebx
	cmp al,80h
	jne @@1
	cmp [lastz],0
	je @@2
	dec [lastz]
	lea ebx,[xtab]
	xlat
	jmp @@out
@@2:	mov al,0
@@out:	out 42h,al
	pop ebx ax
	ret
@@1:	mov [lastz],MAXZ
	lea ebx,[xtab]
	xlat
	jmp @@out
endp

proc __pc8_drv
public __pc8_drv
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
	mov al,0B6h
	out 43h,al
	mov al,2
	out 42h,al
	mov al,0
	out 42h,al
	mov al,90h
	out 43h,al
	mov al,0
	out 42h,al
	in al,61h
	or al,3
	out 61h,al
	ret
endp

proc shutdown
	mov al,0B6h
	out 43h,al
	mov al,2
	out 42h,al
	mov al,0
	out 42h,al
	in al,61h
	and al,0FCh
	out 61h,al
	ret
endp

proc get_time_proc
	mov ebx,offset time_proc
	ret
endp

end
