ideal
p386
model flat,cpp

MAXI=16

extrn T_init:proc,T_done:proc
extrn T_set_timeproc:proc

extrn __none_drv:proc

dataseg

label note_tab word
include "notetab.dat"
public note_tab

snd_vol dw 128
mus_vol dw 64
s_interp db 0

public snd_vol,mus_vol,s_interp

sfreq dw 11025

extrn dmi:dword
extrn seq:byte,seqn:byte,pat:dword,patp:dword

active db 0
snd_port dw 0
snd_dma dw 0
snd_irq dw 0
snd_type dw -1
snd_drv dd offset __none_drv
snd_par dd offset none_par
snd_tp dd 0

public snd_port,snd_dma,snd_irq,snd_type,snd_drv,snd_par

const

none db 'none',0
none_par dw 0,0,0

public sfreq

udataseg

mfreq dw ?
mcnt dw ?

curseq db ?
playst db ?

iptab db 8192 dup(?)

irate dd MAXI dup(?)
ilen dd MAXI dup(?)
iofs dd MAXI dup(?)
ierr dd MAXI dup(?)
ivol db MAXI dup(?,?,?,?)
ilpofs dd MAXI dup(?)
ilplen dd MAXI dup(?)

chnp dd 8 dup(?)
chnt dd 8 dup(?)
ciofs dd 8 dup(?)

public curseq,chnp,chnt

codeseg

proc S_play uses ebx esi, @@p:dword,@@c:dword,@@r:dword,@@v:dword
public S_play
	cmp [snd_type],-1
	je @@x
	cmp [@@p],0
	je @@x
	movsx esi,[word @@c]
	or esi,esi
	jg @@ok
	mov esi,8*4
@@1:	cmp [word irate+esi],0
	je @@1x
	add esi,4
	cmp esi,MAXI*4
	jb @@1
	cmp [word @@c],0
	jl @@over
@@x:	xor eax,eax
	ret
@@over:
	mov esi,8*4
	mov eax,[ilen+esi]
	mov ebx,9*4
@@3:	cmp [ilen+ebx],eax
	ja @@3a
	mov esi,ebx
	mov eax,[ilen+ebx]
@@3a:	add ebx,4
	cmp ebx,MAXI*4
	jb @@3
	jmp @@1x
@@ok:	add esi,7
	shl esi,2
@@1x:	cli
	mov ebx,[@@p]
	mov eax,[ebx]
	mov [ilen+esi],eax
	mov eax,[ebx+8]
	mov [ilpofs+esi],eax
	mov eax,[ebx+12]
	mov [ilplen+esi],eax
	mov ax,[snd_vol]
	mul [word @@v]
	mov [byte ivol+esi],ah
	mov eax,[ebx+4]
	mul [word @@r]
	shr dx,1
	rcr ah,1
	shr dx,1
	rcr ah,1
	mov al,ah
	mov ah,dl
	or dh,dh
	jz @@2a
	mov ax,0FFFFh
@@2a:	cmp ax,2000
	jae @@2b
	mov ax,2000
@@2b:	mov [word irate+esi],ax
	add ebx,16
	mov [iofs+esi],ebx
	add [ilpofs+esi],ebx
	mov [word ierr+esi],0
	shr esi,2
	sub esi,7
	mov eax,esi
	sti
	ret
endp

proc S_stop uses ebx, @@c:dword
public S_stop
	cli
	movzx ebx,[word @@c]
	or ebx,ebx
	jz @@x
	add ebx,7
	shl ebx,2
	mov [word irate+ebx],0
@@x:	sti
	ret
endp

proc S_setrate uses ebx, @@c:dword,@@r:dword
public S_setrate
	cli
	movzx ebx,[word @@c]
	or ebx,ebx
	jz @@x
	add ebx,7
	shl ebx,2
	cmp [word irate+ebx],0
	je @@x
	mov ax,[word @@r]
	mov [word irate+ebx],ax
@@x:	sti
	ret
endp

proc S_setvolume uses ebx, @@c:dword,@@v:dword
public S_setvolume
	cli
	movzx ebx,[word @@c]
	or ebx,ebx
	jz @@x
	add ebx,7
	shl ebx,2
	cmp [word irate+ebx],0
	je @@x
	mov ax,[snd_vol]
	mul [word @@v]
	mov [byte ivol+ebx],ah
@@x:	sti
	ret
endp

proc getsample near
	xor edi,edi
	cmp [snd_vol],0
	jne @@go
	cmp [mus_vol],0
	jne @@go0
	xor eax,eax
	ret
@@nom:	xor edi,edi
	mov esi,8*4
	jmp @@g2
@@go:
	cmp [mus_vol],0
	je @@nom
@@go0:	dec [mcnt]
	jnz @@g1
	mov ax,[mfreq]
	mov [mcnt],ax
	call near playnext
	xor edi,edi
@@g1:	xor esi,esi
@@g2:	mov bp,[sfreq]
@@1:	movzx eax,[word irate+esi]
	or ax,ax
	jz @@1z
	add ax,[word ierr+esi]
	jc @@1a
	cmp ax,bp
	jb @@1b
@@1a:	sub ax,bp
	inc [iofs+esi]
	dec [ilen+esi]
	jnz @@1c
	mov ecx,[ilplen+esi]
	jecxz @@1d
	mov [ilen+esi],ecx
	mov edx,[ilpofs+esi]
	mov [iofs+esi],edx
	jmp @@1c
@@1d:	mov [word irate+esi],0
	jmp @@1z
@@1c:	cmp ax,bp
	jae @@1a
@@1b:	mov [word ierr+esi],ax
	cmp [s_interp],0
	je @@noip
	shr ax,3
	movzx eax,[iptab+eax]
	mov ecx,eax
	mul [byte ivol+esi]
	xchg eax,ecx
	not al
	mul [byte ivol+esi]
	mov ebx,[iofs+esi]
	movsx edx,[byte ebx]
	imul edx
	xchg eax,ecx

	cmp [ilen+esi],1
	jne @@ip2
	cmp [ilplen+esi],0
	jne @@ip3
	mov eax,ecx
	jmp @@ip1
@@ip3:
	mov ebx,[ilpofs+esi]
	movsx ebx,[byte ebx]
	imul ebx
	add eax,ecx
	jmp @@ip1
@@ip2:
	movsx ebx,[byte ebx+1]
	imul ebx
	add eax,ecx
@@ip1:
	sar eax,8
	add edi,eax
	jmp @@1z
@@noip:	mov ebx,[iofs+esi]
	mov al,[ebx]
	imul [byte ivol+esi]
	movsx eax,ax
	add edi,eax
@@1z:	add esi,4
	cmp esi,8*4
	je @@1zz
	cmp esi,MAXI*4
	jb @@1
	cmp edi,32767
	jl @@aa
	mov eax,32767
	ret
@@aa:	cmp di,-32768
	jg @@bb
	mov eax,-32768
	ret
@@bb:	mov eax,edi
	ret
@@1zz:	movzx eax,[mus_vol]
	imul edi
	sar eax,8
	mov edi,eax
	jmp @@1
endp

proc playblk near
	mov bl,[curseq]
@@1:	cmp bl,[seqn]
	jae @@x
	xor bh,bh
	movzx ebx,bx
	cmp [seq+ebx],0FFh
	jne @@ok
	mov bl,[seq+ebx+1]
	mov [curseq],bl
	jmp @@1
@@ok:
	mov bl,[seq+ebx]
	shl ebx,5
	mov edi,[patp]
	add edi,ebx
	mov ecx,8
	xor ebx,ebx
@@2:	mov eax,[edi]
	add eax,[pat]
	mov [chnp+ebx],eax
	mov [chnt+ebx],1
;	mov [ciofs+ebx],0
	add ebx,4
	add edi,4
	loop @@2
	push ds
	pop es
	lea edi,[irate]
	xor eax,eax
	mov ecx,8
	cld
	rep stosd
	mov [playst],03h
@@x:	ret
endp

proc playnext near
@@beg:
	xor esi,esi
@@1:	cmp [chnp+esi],0
	je @@1z
	dec [word chnt+esi]
	jnz @@1z
	mov edi,[chnp+esi]
	add [chnp+esi],4
	cmp [byte edi+2],80h
	jne @@1a
	mov [chnp+esi],0
	mov cx,si
	shr cl,2
	mov al,0FEh
	rol al,cl
	and [playst],al
	jnz @@1z
	inc [curseq]
	call near playblk
	jmp @@beg
@@1a:
	mov al,[edi+1]
	or al,al
	jz @@1c
	dec al
	xor ah,ah
	movzx eax,ax
	shl eax,2
	mov ebx,[dmi]
	add ebx,eax
	mov ebx,[ebx]
	mov [ciofs+esi],ebx
@@1c:
	mov al,[edi]
	cmp al,0FFh
	je @@2
	cmp al,0FEh
	jne @@1b
	mov al,[edi+3]
	mov ah,[edi+1]
	mov [word chnt+esi],ax
	jmp @@1z
@@1b:
	mov bl,al
	xor bh,bh
	movzx ebx,bx
	shl ebx,1
	mov ax,[note_tab+ebx]
	mov ebx,[ciofs+esi]
	mul [word ebx+4]
	shr dx,1
	rcr ah,1
	shr dx,1
	rcr ah,1
	mov al,ah
	mov ah,dl
	or dh,dh
	jz @@2a
	mov ax,0FFFFh
@@2a:	cmp ax,2000
	jae @@2b
	mov ax,2000
@@2b:
	mov [word irate+esi],ax
	mov [word ierr+esi],0
	mov eax,[ebx]
	mov [ilen+esi],eax
	mov eax,[ebx+8]
	mov [ilpofs+esi],eax
	mov eax,[ebx+12]
	mov [ilplen+esi],eax
	add ebx,16
	mov [iofs+esi],ebx
	add [ilpofs+esi],ebx
@@2:
	mov al,[edi+2]
	test al,80h
	jnz @@3
	mov [byte ivol+esi],al
	or al,al
	jnz @@2c
	mov [word irate+esi],0
@@2c:
	mov al,[edi+3]
	xor ah,ah
	or al,al
	jnz @@2d
	inc ah
@@2d:
	mov [word chnt+esi],ax
@@3:
@@1z:
	add esi,4
	cmp esi,8*4
	jb @@1
	ret
endp

proc S_startmusic uses ebx ecx esi edi
public S_startmusic
	cmp [pat],0
	je @@x
	cmp [snd_type],-1
	je @@x
	cli
	mov [curseq],0
	mov ax,[mfreq]
	mov [mcnt],ax
	push bp
	call playblk
	call playnext
	pop bp
	sti
@@x:	ret
endp

proc S_stopmusic uses ecx edi
public S_stopmusic
	cmp [snd_type],-1
	je @@x
	cli
	push ds
	pop es
	lea edi,[chnp]
	xor eax,eax
	mov ecx,8
	cld
	rep stosd
	lea edi,[irate]
	mov ecx,8
	rep stosd
	mov [playst],al
	sti
@@x:	ret
endp

;es:edi=>dmabuf,cx=len
proc mapdma
	mov ax,DGROUP
	mov ds,ax
	cld
	test [snd_type],2
	jnz @@2
@@1:	push ecx edi
	call getsample
	pop edi ecx
	mov al,ah
	xor al,128
	stosb
	loop @@1
	ret
@@2:	push ecx edi
	call getsample
	pop edi ecx
	xor ah,80h
	stosw
	loop @@2
	ret
endp

proc timeproc uses eax ebx ecx edx esi edi ds es
	push ebp
	mov ax,DGROUP
	mov ds,ax
	call getsample
	xor ah,128
	test [snd_type],2
	jnz @@1
	mov al,ah
@@1:	call [snd_tp]
	pop ebp
	ret
endp

proc S_init uses ebx ecx esi edi
public S_init
	call T_init
	cmp [active],0
	jne @@ex
	cmp [snd_type],-1
	je @@ex

	xor ebx,ebx
	lea edi,[iptab]
@@l1:	mov ax,255
	mul bx
	div [sfreq]
	mov [byte edi],al
	inc edi
	add ebx,8
	cmp bx,[sfreq]
	jbe @@l1

	push ds
	pop es
	lea edi,[irate]
	mov ecx,MAXI
	xor eax,eax
	cld
	rep stosd
	lea edi,[chnp]
	mov ecx,8
	rep stosd
	mov ebx,2
	mov ax,[snd_port]
	mov cx,[snd_dma]
	mov dx,[snd_irq]
	call [snd_drv]
	cli
	mov [playst],0
	mov ax,[sfreq]
	shr ax,6
	mov [mfreq],ax
	mov [mcnt],1
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
