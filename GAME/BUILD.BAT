@SET AS=tasm /ml/z/q/t/m5
@SET CC=wcc386 -zq -w3 -d1 -s -olrt -3s -j -zp1 -i=..\libs
@SET LD=wlink @pmodew.lnk SYSTEM pmode OP stack=16K LIBP ..\libs\dos

@del /f /q doom2d.exe
@del /f /q *.obj

%CC% main.c
%CC% player.c
%CC% dots.c
%CC% weapons.c
%CC% items.c
%CC% monster.c
%CC% switch.c
%CC% fx.c
%CC% bmap.c
%CC% view.c
%CC% menu.c
%CC% game.c
%CC% files.c
%CC% config.c
%CC% error.c
%CC% memory.c
%AS% vga2.asm
%CC% -oe anim.c
%AS% misca.asm
%CC% miscc.c

@echo file main,player,dots,weapons,items,         >  link.tmp
@echo monster,switch,fx,bmap,view,menu,game,files, >> link.tmp
@echo config,error,memory,vga2,anim,misca,miscc    >> link.tmp

%LD% NAME doom2d @link.tmp

@del /f /q link.tmp
