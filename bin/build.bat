call "D:\Program Files\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat"
cd ..
cl /EHsc /MT /W4 /O2 /std:c++17 /Febin\CAC.exe /Fobin\ /Ilib\libssq\include src\main.cpp kernel32.lib Advapi32.lib Userenv.lib ws2_32.lib Dnsapi.lib lib\libssq\src\a2s\*.c lib\libssq\src\*.c
cd bin
pause
CAC.exe

pause