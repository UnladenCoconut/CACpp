#include <windows.h>
#include <iostream>

//fake arma3_x64.exe for testing.

int main(int argc, char* argv[]){
    for(int i=0;i<argc;++i){
        std::cout<<argv[i]<<" ";
    }
    std::cout<<'\n';
    Sleep(10000);
}