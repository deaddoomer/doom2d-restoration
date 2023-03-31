ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for VIEW.C
;       layout same as in original doom2d.exe
;

udataseg

public walh,walf,walp,walani,fldf,fldb,fld,walswp,anih,w_y,w_x,w_o

pad db 8 dup(?) ; unused variables?

walh dd 256 dup(?)
walf dd 256 dup(?)
walp dd 256 dup(?)
walani db 256 dup(?)
fldf db (100*100) dup(?)
fldb db (100*100) dup(?)
label fld
_fld db (100*100) dup(?)
walswp db 256 dup(?)
anih dd (5*5) dup(?)
w_y dd ?
w_x dd ?
w_o dd ?

end
