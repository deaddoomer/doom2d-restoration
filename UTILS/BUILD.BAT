@SET CC=bcc -ml

@del /f /q vga2obj3.exe
@del /f /q fnt2obj3.exe
@del /f /q *.obj

%CC% vga2obj3.c
%CC% fnt2obj3.c
