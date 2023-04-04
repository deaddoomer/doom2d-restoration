ideal
p386
model flat,cpp

extrn T_init:proc,T_done:proc
extrn T_set_timeproc:proc

extrn __none_drv:proc

dataseg

sfreq dw 11025

active db 0
snd_port dw 0
snd_dma dw 0
snd_irq dw 0
snd_type dw -1
snd_drv dd offset __none_drv
snd_par dd offset none_par
snd_tp dd 0

snd_ptr dd 0
snd_len dd 0
snd_ls dd 0
snd_ll dd 0

public snd_port,snd_dma,snd_irq,snd_type,snd_drv,snd_par

const

none db 'none',0
none_par dw 0,0,0

public sfreq

codeseg

proc getsample near
	mov al,80h
	mov ebx,[snd_ptr]
	or ebx,ebx
	jz @@ok
	add al,[ebx]
	inc [snd_ptr]
	dec [snd_len]
	jnz @@ok
	mov [snd_ptr],0
	mov ebx,[snd_ll]
	or ebx,ebx
	jz @@ok
	mov [snd_len],ebx
	mov ebx,[snd_ls]
	mov [snd_ptr],ebx
@@ok:	ret
endp

;es:edi=>dmabuf,cx=len
proc mapdma near uses ds
	mov ax,DGROUP
	mov ds,ax
	cld
@@1:	call getsample
	stosb
	loop @@1
	ret
endp

proc timeproc near uses eax ebx ecx edx esi edi ds es
	push ebp
	mov ax,DGROUP
	mov ds,ax
	call getsample
	call [snd_tp]
	pop ebp
	ret
endp

proc S_play uses ebx, @@p:dword
public S_play
	cli
	mov ebx,[@@p]
	add ebx,8
	mov [snd_ptr],ebx
	movzx eax,[word ebx-8]
	mov [snd_len],eax
	movzx eax,[word ebx-2]
	mov [snd_ll],eax
	movzx eax,[word ebx-4]
	add eax,ebx
	mov [snd_ls],eax
	sti
	ret
endp

proc S_stop
public S_stop
	xor eax,eax
	mov [snd_ptr],eax
	mov [snd_len],eax
	ret
endp

proc S_issound
public S_issound
	mov eax,[snd_ptr]
	ret
endp

proc S_init uses ebx ecx esi edi
public S_init
	call T_init
	cmp [active],0
	jne @@ex
	cmp [snd_type],-1
	je @@ex
	mov ebx,2
	mov ax,[snd_port]
	mov cx,[snd_dma]
	mov dx,[snd_irq]
	call [snd_drv]
	cli
	test [snd_type],1
	jnz @@1
	mov ebx,4
	call [snd_drv]
	mov [snd_tp],ebx
	push offset timeproc
	movzx eax,[sfreq]
	push eax
	call T_set_timeproc
	add sp,8
	jmp @@ok
@@1:	mov eax,offset mapdma
	mov cx,[sfreq]
	mov ebx,4
	call [snd_drv]
	cmp cx,20
	jb @@ok
	push eax
	movzx ecx,cx
	push ecx
	call T_set_timeproc
	add sp,8
@@ok:	sti
	mov [active],1
@@ex:	ret
endp

proc S_done uses ebx ecx esi edi
public S_done
	call T_done
	cmp [active],0
	je @@ex
	mov ebx,3
	call [snd_drv]
	mov [active],0
@@ex:	ret
endp

proc S_detect uses ebx ecx esi edi
public S_detect
	cmp [snd_type],-1
	je @@err
	mov ebx,1
	mov ax,[snd_port]
	mov cx,[snd_dma]
	mov dx,[snd_irq]
	call [snd_drv]
	ret
@@err:	mov ax,1
	ret
endp

proc S_getinfo uses ebx ecx esi edi
public S_getinfo
	xor ebx,ebx
	call [snd_drv]
	mov [snd_type],ax
	cmp ax,-1
	je @@err
	mov eax,ebx
	mov [snd_par],ecx
	ret
@@err:	lea eax,[none_par]
	mov [snd_par],eax
	lea eax,[none]
	ret
endp

end
