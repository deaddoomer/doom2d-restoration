@SET CC=wcc386 -zp1 -5s -s -j -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\sound.lib

del /f /q %LIB%
del /f /q *.obj

%AS% sound.asm
%WL% %LIB% -+sound

%AS% timer.asm
%WL% %LIB% -+timer

%CC% loadsnd.c
%WL% %LIB% -+loadsnd
