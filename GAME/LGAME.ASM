ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for GAME.C
;       layout same as in original doom2d.exe v1.40
;

udataseg

public ltnsnd,ltn,lt_force,lt_ypos,lt_side,g_transt
public lt_type,cd_scr,telepsnd,scrnh,lt_time,dm_pos
public dm_pl1p,dm_pl2p,g_time,dm_pnum

ltnsnd   dd (2)   dup(?)
ltn      dd (2*2) dup(?)
lt_force dd ?
lt_ypos  dd ?
lt_side  dd ?
g_transt dd ?
lt_type  dd ?
cd_scr   dd ?
telepsnd dd ?
scrnh    dd (3) dup(?)
lt_time  dd ?
dm_pos   db (9*100) dup(?)
dm_pl1p  dd ?
dm_pl2p  dd ?
g_time   dd ?
dm_pnum  dd ?

end
