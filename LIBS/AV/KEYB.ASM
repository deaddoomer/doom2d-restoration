ideal
p386
model flat,cpp

dataseg

key_up db 0
spec db 0
keys db 256 dup(0)
key_proc dd offset def_key_proc
drv_on db 0

udataseg

int9ofs dd ?
int9sel dw ?

codeseg

proc K_init uses ebx ds es
	cmp [drv_on],0
	jne @@x
	cli
	mov ax,3509h
	int 21h
	mov [int9ofs],ebx
	mov [int9sel],es
	mov [drv_on],1
	push cs
	pop ds
	mov edx,offset KeyDriver
	mov ax,2509h
	int 21h
	sti
@@x:    ret
endp

proc K_done uses ds
	cmp [drv_on],0
	je @@x
	cli
	mov edx,[int9ofs]
	mov ds,[int9sel]
	mov ax,2509h
	int 21h
	sti
@@x:    ret
endp

proc KeyDriver
	push eax ebx ds
	mov ax,DGROUP
	mov ds,ax
	in al,60h
	mov ah,al
	in al,61h
	xor al,80h
	out 61h,al
	xor al,80h
	out 61h,al
	cmp [key_up],0
	jne @@up
	cmp ah,0F0h
	jne @@ok
	mov [key_up],1
	jmp @@end
@@up:   or ah,80h
	mov [key_up],0
@@ok:   cmp ah,0E0h
	je @@spec
	cmp ah,0E1h
	je @@spec
	mov bl,ah
	and ebx,7Fh
	mov al,-1
	rol ah,1
	adc al,0
	cmp [spec],0
	je @@1
	or bl,80h
	mov [spec],0
@@1:    cmp al,[keys+ebx]
	je @@end
	mov [keys+ebx],al
	or al,al
	jz @@end
	push ecx edx esi edi ebp es fs gs bx
	call [key_proc]
	pop ax gs fs es ebp edi esi edx ecx
	jmp @@end
@@spec: mov [spec],1
@@end:  mov al,20h
	out 20h,al
	pop ds ebx eax
	iretd
endp

proc def_key_proc
	ret
endp

proc K_setkeyproc near @@p:dword
	cli
	mov eax,[@@p]
	mov [key_proc],eax
	sti
	ret
endp

proc K_getkeyproc
	mov eax,[key_proc]
	ret
endp

ifdef __DLL__
publicdll key_proc,keys,K_setkeyproc,def_key_proc,K_done,K_init
else
public    key_proc,keys,K_setkeyproc,def_key_proc,K_done,K_init
endif

end
