ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for MONSTER.C
;       layout same as in original doom2d.exe v1.40
;

udataseg

public impsitsnd,mon_spr,mon_sprd,pl_spr,sgun,gsnd,fspr,mn,gspr,impdthsnd
public mon_snd,gsndt,positsnd,pchsnd,hit_yv,telesnd,fsnd,swgsnd
public podthsnd,firsnd,trupsnd,slopsnd,mnum,pauksnd,hit_xv

impsitsnd dd 2         dup(?)
mon_spr   dd (20*29*2) dup(?)
mon_sprd  db (20*29*2) dup(?)
pl_spr    dd 2         dup(?)
sgun      dd 2         dup(?)
gsnd      dd 4         dup(?)
fspr      dd 8         dup(?)
mn        db (78*200)  dup(?)
gspr      dd (13*8)    dup(?)
impdthsnd dd 2         dup(?)
mon_snd   dd (20*5)    dup(?)
gsndt     dd ?
positsnd  dd 3         dup(?)
pchsnd    dd ?
hit_yv    dd ?
telesnd   dd ?
fsnd      dd ?
swgsnd    dd ?
podthsnd  dd 3         dup(?)
firsnd    dd ?
trupsnd   dd ?
slopsnd   dd ?
mnum      dd ?
pauksnd   dd ?
hit_xv    dd ?

end
