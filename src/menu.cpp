#ifndef CAC_MENU
#define CAC_MENU

#include "system.cpp"
#include "cfg.cpp"
#include "server.cpp"
#include <conio.h>

//virtual escape sequences are still a WIP on windows, but nice to use them in future

typedef void(*fnPtr)();
typedef fnPtr(*ptrFnPtr)();
namespace menu {

    HANDLE conScreenBuf;
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    DWORD cw;
    COORD origin;
    DWORD t_s=2000; //time to sleep, ms

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
        if(!CloseHandle(conScreenBuf)) logErr();
        return nullptr;
    }

    fnPtr settings(){
        cls();
        return nullptr;
    }
    
    fnPtr main(){
        cls();
        std::cout<<"Arma 3 CAC Launcher\n";
        std::cout<<"\nVERSION: CAC++ 0.1 EXPERIMENTAL\n";
        std::cout<<"\nUSERNAME: ?\n";
        std::cout<<"\nOPTIONAL MODS: ?\n";
        std::cout<<"\nSERVER STATUS: ?\n";
        std::cout<<"\n 1 Exile Altis\n";
        std::cout<<" 2 Exile Tanoa\n";
        std::cout<<" 3 Coop PVE\n";
        std::cout<<" 4 King of The Hill TVT\n";
        std::cout<<" 5 Dynamic Recon Ops CUP\n";
        std::cout<<" 6 Antistasi RHS\n";
        std::cout<<" 7 Exile Escape\n";
        std::cout<<" 8 Antistasi S.O.G. Prairie Fire\n";
        std::cout<<"\n 9 CAC Settings\n";
        std::cout<<"\n 0 Exit CAC++ Launcher\n";
        std::cout<<"Choose option (0-9): ";
        sl=select("1234567890");
        switch(sl){
            //TODO handling if fail to launch arma (e.g. arma3_x64.exe not found)
            case '1':
            server::launchPwd(server::exileAltis);
            break;
            case '2':
            server::launchPwd(server::exileTanoa);
            break;
            case '3':
            server::launchServer(server::coop);
            break;
            case '4':
            server::launchServer(server::KoTH);
            break;
            case '5':
            server::launchServer(server::DRO);
            break;
            case '6':
            server::launchServer(server::antistasi);
            break;
            case '7':
            server::launchServer(server::exileEscape);
            break;
            case '8':
            server::launchVN(server::prairieFire);
            break;
            case '9':
            return (fnPtr)settings;
            break;
            case '0':
            return (fnPtr)end;
            break;
        }
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
                            cls();
                            std::cout<<"\nPlease enter path of your steam.exe. (Example: \"C:\\Steam.exe\")\n";
                            std::wcin>>steamPath;
                            if(std::filesystem::exists(steamPath)){//TODO should check that the filename is steam.exe or enter dir instead.
                                cls(); std::cout<<"STEAM FOUND\n"; Sleep(t_s);
                                setVar(steamPath.wstring(),L"CACCore\\memory.txt");
                                found=true;
                            } else{
                                cls(); std::wcout<<steamPath.wstring()<<" DOES NOT EXIST.\n";
                            }
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
                cls(); std::cout<<"FAILED TO LAUNCH STEAM. LAUNCH STEAM MANUALLY IF ISSUE PERSISTS.\n";/*TODO allow user to launch steam manually
                and then continue.*/
                Sleep(t_s);
                return (fnPtr)end;
            }
        }
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