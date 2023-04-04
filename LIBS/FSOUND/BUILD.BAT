@SET CC=wcc386 -zp1 -3s -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\fsound.lib

del /f /q %LIB%
del /f /q *.obj

%AS% timer.asm
%AS% sound.asm
%CC% loadsnd.c

%WL% -n %LIB% +timer +sound +loadsnd
