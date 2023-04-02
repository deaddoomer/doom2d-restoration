@SET PP=wpp386 -zp1 -d1 -w2 -s -omilert -5s -j -zq -i=..
@SET WL=wlib -q -b
@SET LIB=..\dos\avcpp.lib

del /f /q *.obj

%PP% tab.cpp

%WL% -n %LIB% +tab
