ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for FILES.C
;       layout same as in original doom2d.exe
;

udataseg

public blk,wadh,wad,savname,wads,d_end,s_start,s_end
public m_end,m_start,patp,d_start,dmi,wad_num,f_dir
public f_name,f_ext,f_drive,savok,seq,seqn
public readstr_c,readstrz_c

blk db 8 dup(?)
wadh dd 20 dup(?)
wad db (2000*20) dup(?)
pad db 144 dup(?) ; unused path string?
savname db (7*24) dup(?)
wads db (20*144) dup(?)
d_end dd ?
s_start dd ?
s_end dd ?
m_end dd ?
m_start dd ?
patp dd ?
d_start dd ?
dmi dd ?
wad_num dd ?
f_dir db 130 dup(?)
f_name db 9 dup(?)
f_ext db 5 dup(?)
f_drive db 3 dup(?)
savok db 7 dup(?)
seq db 255 dup(?)
seqn db ?
readstr_c db ?
readstrz_c db ?

end
