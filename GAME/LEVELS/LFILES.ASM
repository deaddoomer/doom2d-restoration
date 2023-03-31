ideal
p386
model flat,cpp

;
; HACK: this file used to allocate global variables for FILES.C
;       layout same as in original editor.exe
;

udataseg

public blk,wadh,wad,f_path,wads,savo,savh,w_end,d_end
public s_start,d_start,s_end,m_end,m_start,w_start
public wad_num,f_dir,f_name,f_ext,f_drive
public readstr_c,readstrz_c,we

pad db 12 dup(?)

blk db 8 dup(?)
wadh dd 20 dup(?)
wad db (3000*20) dup(?)
f_path db 144 dup(?)
wads db (20*144) dup(?)
savo dd ?
savh dd ?
w_end dd ?
d_end dd ?
s_start dd ?
d_start dd ?
s_end dd ?
m_end dd ?
m_start dd ?
w_start dd ?
wad_num dd ?
f_dir db 130 dup(?)
f_name db 9 dup(?)
f_ext db 5 dup(?)
f_drive db 3 dup(?)
readstr_c db ?
readstrz_c db ?
pad2 db 3 dup(?)
we db 16 dup(?)

end
