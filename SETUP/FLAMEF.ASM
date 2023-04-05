ideal
p386
model flat,cpp

extrn bright:byte

codeseg

proc flame_sprf uses ebx
public flame_sprf
	inc al
	shl al,1
	mov bl,[edi]
	xor bh,bh
	movzx ebx,bx
	add al,[bright+ebx]
	add al,0C0h
	stosb
	ret
endp

end
