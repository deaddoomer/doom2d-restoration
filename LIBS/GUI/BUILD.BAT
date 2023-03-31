@SET CPP=wpp386 -d1 -w2 -s -olrt -3s -j -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET FNT2OBJ=..\..\utils\fnt2obj3.exe
@SET LIB=..\dos\gui.lib

del /f /q %LIB%
del /f /q *.obj

%CPP% gui.cpp
%AS% input.asm
%FNT2OBJ% smallfnt.vga smallfnt.obj smallfont
%WL% -n %LIB% +gui +input +smallfnt
