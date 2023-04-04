@SET AS=tasm /ml/z/q/t/m5
@SET CC=wcc386 -zq -w3 -d1 -s -olrt -3s -j -zp1 -i=..\libs
@SET LD=wlink @pmodew.lnk SYSTEM pmode OP stack=16K LIBP ..\libs\dos
@SET VGA2OBJ=..\utils\vga2obj3.exe
@SET FNT2OBJ=..\utils\fnt2obj3.exe

@del /f /q setup.exe
@del /f /q *.obj

%VGA2OBJ% setup.vga setup_v.obj doompal
%FNT2OBJ% doomfnt1.vga doomfnt1.obj doom_sfnt
%FNT2OBJ% doomfnt2.vga doomfnt2.obj doom_bfnt
%CC% setup.c

%LD% NAME setup FILE setup,setup_v,doomfnt1,doomfnt2
