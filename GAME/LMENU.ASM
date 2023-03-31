ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for MENU.C
;       layout same as in original doom2d.exe v1.40
;

udataseg

public msklh,cbuf,ibuf,qsnd,mbarl,mbarm,mbaro,mbarr
public mslotl,mslotm,mslotr,csnd1,csnd2,msnd1,msnd3
public msnd2,msnd5,msnd4,msnd6,icur

msklh  dd 2  dup(?)
cbuf   db 32 dup(?)
ibuf   db 24 dup(?)
qsnd   dd 14 dup(?)
mbarl  dd ?
mbarm  dd ?
mbaro  dd ?
mbarr  dd ?
mslotl dd ?
mslotm dd ?
mslotr dd ?
csnd1  dd ?
csnd2  dd ?
msnd1  dd ?
msnd3  dd ?
msnd2  dd ?
msnd5  dd ?
msnd4  dd ?
msnd6  dd ?
icur   dd ?

end
