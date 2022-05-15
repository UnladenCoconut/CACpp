call "D:\Program Files\Microsoft Visual Studio\Community\VC\Auxiliary\Build\vcvars64.bat"
cd ..
cl /EHsc /MT /W4 /O2 /std:c++17 /Febin\arma3_x64.exe /Fobin\ src\arma.cpp
cd bin
pause