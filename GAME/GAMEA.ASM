ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for GAME.C
;       layout same as in original doom2d.exe
;

udataseg

public endanim2,endanim1,telepsnd,scrnh
public dm_pos,dm_pl1p,dm_pl2p,g_time,dm_pnum

endanim2 dd ?
endanim1 dd ?
telepsnd dd ?
scrnh dd 3 dup(?)
dm_pos db (9*100) dup(?)
dm_pl1p dd ?
dm_pl2p dd ?
g_time dd ?
dm_pnum dd ?

end
