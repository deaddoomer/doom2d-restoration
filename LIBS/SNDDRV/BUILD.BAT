@SET CC=wcc386 -zp1 -5s -s -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\snddrv.lib

del /f /q %LIB%
del /f /q *.obj

%CC% drvtab.c
%WL% %LIB% -+drvtab

%AS% none.asm
%WL% %LIB% -+none

%AS% adlib.asm
%WL% %LIB% -+adlib

%AS% covox.asm
%WL% %LIB% -+covox

%AS% pc1.asm
%WL% %LIB% -+pc1

%AS% pc8.asm
%WL% %LIB% -+pc8

%AS% sb.asm
%WL% %LIB% -+sb

%AS% sb16.asm
%WL% %LIB% -+sb16

