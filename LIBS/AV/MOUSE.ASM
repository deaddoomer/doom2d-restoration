ideal
p386
model flat,cpp

dataseg

m_buttons dw 0
public m_buttons
ifdef __DLL__
publicdll m_buttons
endif

codeseg

proc M_init uses bx ds
public M_init
ifdef __DLL__
publicdll M_init
endif
	xor ax,ax
	int 33h
	or ax,ax
	jnz @@yes
@@no:   xor bx,bx
@@yes:  mov [m_buttons],bx
	mov ax,bx
	ret
endp

proc M_getx uses bx cx
public M_getx
ifdef __DLL__
publicdll M_getx
endif
	xor ax,ax
	cmp [m_buttons],0
	je @@x
	mov ax,3
	int 33h
	mov ax,cx
	sar ax,1
@@x:    ret
endp

proc M_gety uses bx cx
public M_gety
ifdef __DLL__
publicdll M_gety
endif
	xor ax,ax
	cmp [m_buttons],0
	je @@x
	mov ax,3
	int 33h
	mov ax,dx
@@x:    ret
endp

proc M_getb uses bx cx
public M_getb
ifdef __DLL__
publicdll M_getb
endif
	xor ax,ax
	cmp [m_buttons],0
	je @@x
	mov ax,3
	int 33h
	mov ax,bx
@@x:    ret
endp

proc M_gets uses ebx cx, @@x:dword,@@y:dword
public M_gets
ifdef __DLL__
publicdll M_gets
endif
	xor cx,cx
	xor dx,dx
	cmp [m_buttons],0
	je @@ex
	mov ax,11
	int 33h
	mov ebx,[@@x]
	or ebx,ebx
	jz @@1
	mov [ebx],cx
@@1:    mov ebx,[@@y]
	or ebx,ebx
	jz @@ex
	mov [ebx],dx
@@ex:   ret
endp

proc M_getpos uses ebx cx, @@x:dword,@@y:dword
public M_getpos
ifdef __DLL__
publicdll M_getpos
endif
	xor ax,ax
	xor cx,cx
	xor dx,dx
	cmp [m_buttons],0
	je @@ex
	mov ax,3
	int 33h
	sar cx,1
	push bx
	mov ebx,[@@x]
	or ebx,ebx
	jz @@1
	mov [ebx],cx
@@1:    mov ebx,[@@y]
	or ebx,ebx
	jz @@2
	mov [ebx],dx
@@2:    pop ax
@@ex:   ret
endp

end
