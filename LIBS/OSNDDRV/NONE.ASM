ideal
p386
model flat,cpp

const

drv_name db 'no sound',0
drv_par dw 0,0,0

codeseg

proc __none_drv
public __none_drv
	cmp ebx,4
	ja @@err
	cmp ebx,1
	je @@detect
	or ebx,ebx
	jnz @@err
	lea ebx,[drv_name]
	lea ecx,[drv_par]
	mov ax,-1
	ret
@@err:	xor ax,ax
	ret
@@detect:
	mov ax,1
	ret
endp

end
