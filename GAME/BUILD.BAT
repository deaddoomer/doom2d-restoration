@SET AS=tasm /ml/z/q/t/m5
@SET CC=wcc386 -w0 -zq -d1 -s -olrte -5s -j -zp1 -i=..\libs
@SET PP=wpp386 -w0 -zq -d1 -s -omilert -5s -j -zp1 -i=..\libs
@SET PP1=wpp386 -w0 -zq -d1 -s -omilert -5s -j -zp1 -i=..\libs
@SET LD=wlink SYSTEM dos4g OP stack=32K LIBP ..\libs\dos

@del /f /q doom2d.exe
@del /f /q *.obj

%CC% main.c
%PP1% -zm botplay.cpp
%PP% bots.cpp
%AS% lplayer.asm
%CC% -dUSE_LAYOUT_HACK player.c
%CC% dots.c
%CC% smoke.c
%CC% weapons.c
%CC% items.c
%AS% lmonster.asm
%CC% -dUSE_LAYOUT_HACK monster.c
%CC% switch.c
%CC% fx.c
%CC% bmap.c
%CC% view.c
%AS% lmenu.asm
%CC% -dUSE_LAYOUT_HACK menu.c
%AS% lgame.asm
%CC% -dUSE_LAYOUT_HACK game.c
%CC% files.c
%CC% config.c
%CC% error.c
%CC% memory.c
%AS% vga2.asm
%CC% a8.c
%AS% misca.asm
%AS% lmiscc.asm
%CC% -dUSE_LAYOUT_HACK miscc.c

@echo file main,botplay,bots,lplayer,player,dots,smoke,weapons,    >  link.tmp
@echo items,lmonster,monster,switch,fx,bmap,view,lmenu,menu,lgame, >> link.tmp
@echo game,files,config,error,memory,vga2,a8,misca,lmiscc,miscc    >> link.tmp

%LD% NAME doom2d @link.tmp

@del /f /q link.tmp
