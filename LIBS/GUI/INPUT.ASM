ideal
p386
model flat,cpp

dataseg

mouse_buttons dw 0
public mouse_buttons

codeseg

proc I_init uses ebx es
public I_init
  mov ax,3533h
  int 21h
  mov ax,es
  movzx eax,ax
  or eax,ebx
  jz @@no
  xor ax,ax
  int 33h
  or ax,ax
  jz @@no
  mov [mouse_buttons],bx
  ret
@@no:
  mov [mouse_buttons],0
  ret
endp

proc I_close
public I_close
  cmp [mouse_buttons],0
  je @@x
@@x:
  ret
endp

proc I_mon
public I_mon
  cmp [mouse_buttons],0
  je @@x
  mov ax,1
  int 33h
@@x:
  ret
endp

proc I_moff
public I_moff
  cmp [mouse_buttons],0
  je @@x
  mov ax,2
  int 33h
@@x:
  ret
endp

proc I_mget uses ebx ecx, @@x:dword,@@y:dword
public I_mget
  cmp [mouse_buttons],0
  je @@err
  mov ax,3
  int 33h
  movsx ecx,cx
  movsx edx,dx
  sar cx,1
  movzx eax,bx
  mov ebx,[@@x]
  mov [ebx],ecx
  mov ebx,[@@y]
  mov [ebx],edx
  ret
@@err:
  mov ebx,[@@x]
  mov [dword ebx],160
  mov ebx,[@@y]
  mov [dword ebx],100
  xor eax,eax
  ret
endp

proc I_iskey
public I_iskey
  mov ah,1
  int 16h
  je @@no
  mov eax,-1
  ret
@@no:
  xor eax,eax
  ret
endp

proc I_getkey
public I_getkey
  mov ah,0
  int 16h
  or al,al
  jz @@x
  xor ah,ah
@@x:
  movzx eax,ax
  ret
endp

end
