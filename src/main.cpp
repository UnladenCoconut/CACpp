
#include <exception>
#include <iostream>
#include <string>

#include "username.cpp"
#include "menu.cpp"
#include "server.cpp"

//note wide strings are used predominantly for compatibility with windows system and paths, which are UTF-16.
int main(int argc, char *argv[]){

try {

    //-------- init --------
    freopen("CACCore\\log.txt","w",stderr); //write error messages to log file
    if(!(menu::init() && varInit() && userInit())){
        return 1;
    }
    
    //--------
    menu::start();

    return 0;

} catch(std::exception &e){
    std::cerr<<e.what()<<'\n';
    std::cerr<<strerror(errno);
    return 1;
}
}