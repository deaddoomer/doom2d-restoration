ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for PLAYER.C
;       layout same as in original doom2d.exe v1.40
;

udataseg

;public wpn,spr,snd,aisnd,pdsnd,aitime,sprd,pl2,pl1
public wpn,spr,snd,aisnd,pdsnd,aitime,sprd,pl2

wpn    dd (11*6) dup(?)
spr    dd (27*2) dup(?)
snd    dd (11)   dup(?)
aisnd  dd (3)    dup(?)
pdsnd  dd (5)    dup(?)
aitime dd ?
sprd   db (27*2) dup(?)
pl2    db (138)  dup(?)
;pl1    db (138)  dup(?)

end
