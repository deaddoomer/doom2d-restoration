@SET CC=wcc386 -zp1 -5s -s -olrte -zq -j -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\av.lib

del /f /q *.obj

%CC% averrmsg.c
%CC% config.c
%CC% readstrz.c
%AS% cpuid.asm
%AS% harderr.asm
%AS% keyb.asm
%AS% mouse.asm

%WL% -n %LIB% +averrmsg +config +readstrz +cpuid +harderr +keyb +mouse
