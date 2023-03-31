ideal
p386
model flat,cpp

extrn V_spr:proc,V_spr1color:proc

dataseg

vf_x dw 0
vf_y dw 0
vf_step dw 1
vf_font dd 0
vf_color db 0

public vf_x,vf_y,vf_step,vf_font,vf_color

codeseg

proc V_putch uses ebx ecx, @@c:dword
public V_putch
	mov ebx,[vf_font]
	movzx eax,[byte @@c]
	add eax,eax
	add eax,eax
	add ebx,eax
	mov ecx,[ebx]
	jecxz @@1
	add ecx,[vf_font]
	push ecx
	mov al,[vf_color]
	or al,al
	jz @@1a
	push eax ecx
	movsx eax,[vf_y]
	push eax
	movsx eax,[vf_x]
	push eax
	call V_spr1color
	add sp,16
@@1b:	pop ebx
	mov ax,[ebx]
	sub ax,[ebx+4]
@@2:	add ax,[vf_step]
	add [vf_x],ax
	ret
@@1a:	push ecx
	movsx eax,[vf_y]
	push eax
	movsx eax,[vf_x]
	push eax
	call V_spr
	add sp,12
	jmp @@1b
@@1:	mov ebx,[vf_font]
	mov eax,[ebx]
	jmp @@2
endp

proc V_strlen uses ebx si, @@s:dword
public V_strlen
	xor si,si
@@1:	mov ebx,[@@s]
	mov al,[ebx]
	or al,al
	jz @@x
	mov ebx,[vf_font]
	xor ah,ah
	movzx eax,ax
	add eax,eax
	add eax,eax
	add ebx,eax
	mov ebx,[ebx]
	or ebx,ebx
	jz @@1a
	add ebx,[vf_font]
	mov ax,[ebx]
	sub ax,[ebx+4]
@@1b:	add ax,[vf_step]
	add si,ax
	inc [@@s]
	jmp @@1
@@1a:	mov ebx,[vf_font]
	mov eax,[ebx]
	jmp @@1b
@@x:	mov ax,si
	ret
endp

proc V_chrlen uses ebx, @@c:dword
public V_chrlen
	mov al,[byte @@c]
	mov ebx,[vf_font]
	xor ah,ah
	movzx eax,ax
	add eax,eax
	add eax,eax
	add ebx,eax
	mov ebx,[ebx]
	or ebx,ebx
	jz @@1a
	add ebx,[vf_font]
	mov ax,[ebx]
	sub ax,[ebx+4]
@@1b:	add ax,[vf_step]
	ret
@@1a:	mov ebx,[vf_font]
	mov eax,[ebx]
	jmp @@1b
endp

proc V_puts uses ebx, @@s:dword
public V_puts
@@1:	mov ebx,[@@s]
	mov al,[ebx]
	or al,al
	jz @@x
	push ax
	call V_putch
	pop ax
	inc [@@s]
	jmp @@1
@@x:	ret
endp

end
