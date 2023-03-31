ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for MISCC.C
;       layout same as in original doom2d.exe v1.40
;

udataseg

public sth,bulsnd,miscc_keys,bfh,sfh,stone
; public wfront


sth    dd 22 dup(?)
bulsnd dd 2  dup(?)
miscc_keys dd 3  dup(?)
bfh    dd (160-'!') dup(?)
sfh    dd (160-'!') dup(?)
stone  dd 2  dup(?)
; wfront db ?

end
