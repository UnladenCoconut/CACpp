call "D:\Program Files\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat"
cd ..
cl /EHsc /MT /W4 /O2 /std:c++17 /Febin\CAC.exe /Fosrc\ src\main.cpp kernel32.lib
cd bin
pause
CAC.exe

pause