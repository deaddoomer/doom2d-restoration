ideal
p386
model flat,cpp

STKSZ=2048

udataseg

hptr dd ?

stk db STKSZ dup(?)

_ss dw ?
_esp dd ?

codeseg

proc harderr_inst uses edx ds, @@p:dword
public harderr_inst
	mov edx,[@@p]
	mov [hptr],edx
	cli
	push cs
	pop ds
	mov edx,offset handler
	mov ax,2524h
	int 21h
	sti
@@x:	ret
endp

proc handler
	push ebx ds
	mov bx,DGROUP
	mov ds,bx
	mov [_ss],ss
	mov [_esp],esp
	mov ss,bx
	lea esp,[stk+STKSZ-4]
	push ecx edx esi edi ebp es fs gs
	mov es,bx
	movzx ebx,ah
	push ebx
	movzx ebx,al
	push ebx
	and edi,0FFh
	push edi
	cld
	call [hptr]
	pop ebx ebx ebx
	pop gs fs es ebp edi esi edx ecx
	mov ss,[_ss]
	mov esp,[_esp]
	pop ds ebx
	iretd
endp

end
