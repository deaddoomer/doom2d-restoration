@SET CC=wcc386 -zp1 -3s -zq -i=..
@SET AS=tasm /ml/z/q/t/m5
@SET WL=wlib -q -b
@SET LIB=..\dos\vga.lib

del /f /q *.obj

%CC% loadvga.c
%CC% redraw.c
%AS% scrbuf.asm
%AS% vga.asm
%AS% vgafont.asm
%AS% vgapal.asm

%WL% -n %LIB% +loadvga +redraw +scrbuf +vga +vgafont +vgapal
