call "D:\Program Files\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat"
cd ..
cl /EHsc /MTd /W4 /O2 /std:c++17 /Febin\CAC.exe /Fobin\ /Fdbin\ /Zi src\main.cpp kernel32.lib Advapi32.lib Userenv.lib
cd bin
pause
