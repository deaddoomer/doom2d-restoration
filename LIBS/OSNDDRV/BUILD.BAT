@SET CC=wcc386 -zp1 -5s -s -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\osnddrv.lib

del /f /q *.obj

%AS% none.asm
%AS% adlib.asm
%AS% covox.asm
%AS% pc1.asm
%AS% pc8.asm
%AS% sb.asm
%AS% sb16.asm
%CC% drvtab.c

%WL% -n %LIB% +none +adlib +covox +pc1 +pc8 +sb +sb16 +drvtab
