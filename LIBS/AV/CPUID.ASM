ideal
p386
model flat,cpp

dataseg

_cpu dw 0

ifdef __DLL__
publicdll _cpu
else
public    _cpu
endif

codeseg

proc what_cpu uses ebx ecx
ifdef __DLL__
publicdll what_cpu
else
public    what_cpu
endif
	mov ax,400h
	int 31h
	mov al,cl
	xor ah,ah
	mov [_cpu],ax
	ret
endp

end
