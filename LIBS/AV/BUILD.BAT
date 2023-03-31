@SET CC=wcc386 -zp1 -3s -s -zq -j -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\av.lib

del /f /q %LIB%
del /f /q *.obj

%CC% averrmsg.c
%WL% %LIB% -+averrmsg

%CC% config.c
%WL% %LIB% -+config

%CC% readstrz.c
%WL% %LIB% -+readstrz

%AS% cpuid.asm
%WL% %LIB% -+cpuid

%AS% harderr.asm
%WL% %LIB% -+harderr

%AS% keyb.asm
%WL% %LIB% -+keyb

%AS% mouse.asm
%WL% %LIB% -+mouse
