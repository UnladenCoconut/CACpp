
#include <exception>
#include <iostream>
#include <string>

#include "menu.cpp"
#include "server.cpp"

//note wide strings are used predominantly for compatibility with windows system and paths, which are UTF-16.
int main(int argc, char *argv[]){

try {

    //-------- init --------
    freopen("CACCore\\log.txt","w",stderr); //write error messages to log file
    if(!menu::init()){
        return 1;
    }
    server::init();
    //--------

    fnPtr menuNext=(fnPtr) menu::start;
    while(menuNext!=nullptr){
        menuNext= ((ptrFnPtr)menuNext)();
    }

    return 0;

} catch(std::exception &e){
    std::cerr<<e.what()<<'\n';
    std::cerr<<strerror(errno);
    return 1;
}
}