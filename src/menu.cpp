#ifndef CAC_MENU
#define CAC_MENU

#include "system.cpp"
#include "cfg.cpp"
#include <conio.h>

//virtual escape sequences are still a WIP on windows, but nice to use them in future

typedef void(*fnPtr)();
typedef fnPtr(*ptrFnPtr)();
namespace menu {

    HANDLE conScreenBuf;
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    DWORD cw;
    COORD origin;
    DWORD t_s=3000; //time to sleep, ms

    char sl;
    char select(std::string inputs){
        char sl;
        do{
            sl=_getch();
        } while(inputs.find(sl)==-1);
        return sl;
    }

    bool init(){
        conScreenBuf=CreateFileA("CONOUT$",GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,NULL,OPEN_EXISTING,NULL,NULL);
        if(!GetConsoleScreenBufferInfo(conScreenBuf,&conInfo)){
            logErr(); return false;
        }
        cw=conInfo.dwSize.X*conInfo.dwSize.Y;
        ZeroMemory(&origin,sizeof(origin));
        return true;
    }

    void cls(){
        DWORD cn;
        if(!FillConsoleOutputCharacterA(conScreenBuf,' ',cw,origin,&cn)){
            logErr();
            return;
        } else {
            if(cw!=cn){
                std::cerr<<"Failed to clear whole screen.\n";
            }
            if(!SetConsoleCursorPosition(conScreenBuf,origin)){
                logErr();
            }
        }
        
    }

    fnPtr end(){
        std::wstring logo;
        getVar(logo,L"CACCore\\logo.txt");
        cls();
        std::wcout<<logo<<L"\n";
        Sleep(t_s);
        return nullptr;
    }

    fnPtr main(){
        cls();
        std::cout<<"\nVERSION: CAC++ 0.1 EXPERIMENTAL\n\n";
        std::cout<<" 0 Exit CAC++ Launcher\n";
        return (fnPtr)end;
    }

    fnPtr start(){
        cls();
        if(steamRunning()){
            std::cout<<"STEAM IS RUNNING\n";
            Sleep(t_s);
            return (fnPtr)main;
            
        } else {
            std::wstring ws; getVar(ws,L".\\CACCore\\memory.txt"); steamPath=ws;
            if(steamPath==L""){
                cls(); std::cout<<"SEARCHING FOR STEAM, PLEASE WAIT...\n";
                if(steamFind()){
                    cls(); std::wcout<<L"STEAM FOUND AT: "<<steamPath.wstring();
                    Sleep(t_s);
                    setVar(steamPath.wstring(),L"CACCore\\memory.txt");
                } else {
                    bool found=false;
                    do{
                        cls();
                        std::cout<<"\nFAILED TO FIND STEAM.\n";
                        std::cout<<"\n 1 Enter steam.exe path\n";
                        std::cout<<" 2 Exit\n";
                        sl=select("12");
                        if(sl=='1'){
                            do{
                                cls();
                                std::cout<<"\nPlease enter path of your steam.exe. (Example: \"C:\\Steam.exe\")\n";
                                std::wcin>>steamPath;   
                            } while();
                        }else if(sl=='2'){
                            return (fnPtr) end;
                        }
                    }while(!found);
                }
            }
            //steam should be found.
            cls(); std::cout<<"STEAM IS NOT RUNNING, ATTEMPTING TO RUN STEAM...\n";
            if(launch(steamPath,L"")){
                cls(); std::cout<<"STEAM LAUNCHED SUCCESSFULLY.\n";
                Sleep(t_s);
                return (fnPtr)main;
            } else{
                std::cout<<"FAILED TO LAUNCH STEAM. LAUNCH STEAM MANUALLY IF ISSUE PERSISTS.\n";
                Sleep(t_s);
                return (fnPtr)end;
            }
        }
    }

    fnPtr settings(){
        cls();
        return nullptr;
    }

    fnPtr optMods(){
        cls();
        return nullptr;
    }

    fnPtr optModsSelect(){
        cls();
        return nullptr;
    }

    fnPtr modCheck(){
        cls();
        //get the mods for each server and check against them.
        return nullptr;
    }

    fnPtr exilePwd(){
        cls();
        return nullptr;
    }

    fnPtr modDir(){
        cls();
        return nullptr;
    }

    fnPtr userName(){
        cls();
        return nullptr;
    }
}

#endif