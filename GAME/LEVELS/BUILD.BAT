@SET AS=tasm /ml/z/q/t/m5
@SET CC=wcc386 -zq -w3 -d1 -s -olrt -3s -j -zp1 -i=..\..\libs
@SET CPP=wpp386 -zq -w3 -d1 -s -olrt -3s -j -zp1 -i=..\..\libs
@SET LD=wlink @pmodew.lnk SYSTEM pmode OP stack=16K LIBP ..\..\libs\dos
@SET VGA2OBJ=..\..\utils\vga2obj3.exe

@del /f /q editor.exe
@del /f /q *.obj

%CC% main.c
%CPP% edit.cpp
%CPP% view.cpp
%CC% thing.c
%CC% switch.c
%CC% files.c
%CC% config.c
%CC% error.c
%CC% memory.c
%AS% misca.asm
%AS% vga2.asm
%VGA2OBJ% editor.vga editor.obj
%CC% miscc.c

@echo file main,edit,view,thing,switch,files,config, >  link.tmp
@echo error,memory,miscc,misca,vga2,editor           >> link.tmp

%LD% NAME editor @link.tmp

@del /f /q link.tmp
