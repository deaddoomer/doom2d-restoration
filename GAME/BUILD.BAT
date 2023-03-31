@REM
@REM Doom2D 1.40
@REM
@REM To build you need:
@REM - Watcom C/C++ 11.0 (Release version without patches)
@REM - Turbo Assembler from Borland C++ 3.1
@REM - WSTUB.EXE grom Watcom C/C++ 10.0 (Release version without patches)
@REM   (put wstub.exe into directory with sources)
@REM
@REM Do not forget to correctly setup environment variables:
@REM - %PATH%    (tasm & watcom)
@REM - %INCLUDE% (watcom)
@REM - %WATCOM%  (watcom)
@REM - %EDPATH%  (watcom)
@REM
@REM Sources are encoded using IBM866 (CP866)
@REM 

@SET AS=tasm /ml/z/q/t/m5
@SET CC=wcc386 -w0 -zq -d1 -s -olrte -5s -j -zp1 -i=..\libs
@SET PP=wpp386 -w0 -zq -d1 -s -omilert -5s -j -zp1 -i=..\libs
@SET LD=wlink SYSTEM dos4g OP stack=32K LIBP ..\libs\dos

@del /f /q doom2d.exe
@del /f /q *.obj

%CC% main.c
%PP% -zm botplay.cpp
%PP% bots.cpp
%CC% player.c
%CC% dots.c
%CC% smoke.c
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
%CC% a8.c
%AS% misca.asm
%CC% miscc.c

@echo file main,botplay,bots,player,dots,smoke,weapons,  >  link.tmp
@echo items,monster,switch,fx,bmap,view,menu,            >> link.tmp
@echo game,files,config,error,memory,vga2,a8,misca,miscc >> link.tmp

%LD% NAME doom2d @link.tmp

@del /f /q link.tmp
