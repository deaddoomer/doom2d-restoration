@SET CC=wcc386 -zp1 -5s -s -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\vga.lib

del /f /q %LIB%
del /f /q *.obj

%CC% loadvga.c
%WL% %LIB% -+loadvga

%CC% redraw.c
%WL% %LIB% -+redraw

%AS% scrbuf.asm
%WL% %LIB% -+scrbuf

%AS% vga.asm
%WL% %LIB% -+vga

%AS% vgafont.asm
%WL% %LIB% -+vgafont

%AS% vgapal.asm
%WL% %LIB% -+vgapal

