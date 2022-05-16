#ifndef CAC_MENU
#define CAC_MENU

#include "system.cpp"
#include "cfg.cpp"
#include "server.cpp"
#include <conio.h>

/*
typedef void(*fnPtr)();
typedef fnPtr(*ptrFnPtr)();
*/

WORD FOREGROUND_YELLOW=FOREGROUND_RED | FOREGROUND_GREEN;

namespace menu {

//-------- forward declarations --------
    void start(), main(), end(), settings(), optMods(), optModsSelect(), modCheck(), exilePwd(), modDir(), username();

    HANDLE conScreenBuf;
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    DWORD cw;
    COORD origin;
    DWORD t_s=2000; //time to sleep, ms

    char sl;
    char select(std::string inputs){
        do{
            sl=_getch();
        } while(inputs.find(sl)==-1);
        return sl;
    }

    bool init(){
        if(!SetConsoleTitleA("Arma 3 CAC Launcher")) logErr();
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

    void settings(){
        bool ret=false;
        while(!ret){
            cls(); SetConsoleTextAttribute(menu::conScreenBuf,FOREGROUND_GREEN);
            std::cout<<"\n 1 Optional Mods Management ["<<(getVarB(L"CACCore\\memory2.txt")? "ENABLED" : "DISABLED")<<"]\n";
            std::cout<<" 2 Change Username/Profile [?]\n";
            std::cout<<" 3 Change Exile Password [?]\n";
            std::cout<<" 4 Change Mods Directory [?]\n";
            std::cout<<" 5 Mandatory Mods Check\n";
            std::cout<<"\n 6 Return\n";
            std::cout<<"\n Choose Option (1-6): ";
            sl=select("123456");
            switch(sl){
                case '1':
                optMods();
                break;

                case '2':
                username();
                break;

                case '3':
                exilePwd();
                break;

                case '4':
                modDir();
                break;
                
                case '5':
                modCheck();
                break;

                case '6':
                ret=true;
                break;
            }
        }
    }

    void start(){//never returns
        if(steamRunning()){
            cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_GREEN);
            std::cout<<"\nSTEAM IS RUNNING\n";
            Sleep(t_s);
            main();
            
        } else {
            std::wstring steamPath=getVarWS(L"CACCore\\memory.txt");
            //TODO run steamPath first before search
            if(steamPath==L"" || !std::filesystem::exists(steamPath)){
                cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_YELLOW);
                std::cout<<"\nSEARCHING FOR STEAM, PLEASE WAIT...\n";
                steamPath=steamFind();
                if(steamPath!=L""){
                    cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_GREEN);
                    std::wcout<<L"\nSTEAM FOUND AT: "<<steamPath;
                    setVar(steamPath,L"CACCore\\memory.txt");
                    Sleep(t_s);
                } else {
                    bool found=false;
                    do{
                        cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_RED);
                        std::cout<<"\nFAILED TO FIND STEAM.\n";
                        std::cout<<"\n 1 Enter steam.exe path\n";
                        std::cout<<" 2 Exit\n";
                        sl=select("12");
                        if(sl=='1'){
                            cls();
                            std::cout<<"\nPlease enter path of your steam.exe. (Example: \"C:\\Steam.exe\")\n";
                            std::wcin>>steamPath;
                            if(std::filesystem::exists(steamPath)){//TODO should check that the filename is steam.exe or enter dir instead.
                                cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_YELLOW);
                                std::cout<<"\nSTEAM FOUND\n"; Sleep(t_s);
                                setVar(steamPath,L"CACCore\\memory.txt");
                                found=true;
                            } else{
                                cls(); std::wcout<<L'\n'<<steamPath<<" DOES NOT EXIST.\n";
                            }
                        }else if(sl=='2'){
                            end();
                        }
                    }while(!found);
                }
            }
            //steam should be found.
            cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_YELLOW);
            std::cout<<"\nSTEAM IS NOT RUNNING, ATTEMPTING TO RUN STEAM...\n";
            if(launch(steamPath,L"")){
                cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_GREEN);
                std::cout<<"\nSTEAM LAUNCHED SUCCESSFULLY.\n";
                Sleep(t_s);
                main();
            } else{
                cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_RED);
                std::cout<<"\nFAILED TO LAUNCH STEAM. LAUNCH STEAM MANUALLY IF ISSUE PERSISTS.\n";/*TODO allow user to launch steam manually
                and then continue.*/
                Sleep(t_s);
                end();
            }
        }
    }

    void main(){ //will never return
        while(true){
            cls(); SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_GREEN);
            std::cout<<"\nArma 3 CAC Launcher - discord.gg/dNGcyEYK8F\n";
            std::cout<<"\nVERSION: CAC++ 0.1 EXPERIMENTAL\n";
            std::cout<<"\nUSERNAME: ?\n";
            std::cout<<"\nOPTIONAL MODS: "<<(getVarB(L"CACCore\\memory2.txt")? "ENABLED" : "DISABLED")<<'\n';
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
            std::cout<<"\nChoose option (0-9): ";
            sl=select("1234567890");
            switch(sl){
                //TODO handling if fail to launch arma (e.g. arma3_x64.exe not found)
                case '1':
                server::launchPwd(server::exileAltis); end();
                break;
                case '2':
                server::launchPwd(server::exileTanoa); end();
                break;
                case '3':
                server::launchServer(server::coop); end();
                break;
                case '4':
                server::launchServer(server::KoTH); end();
                break;
                case '5':
                server::launchServer(server::DRO); end();
                break;
                case '6':
                server::launchServer(server::antistasi); end();
                break;
                case '7':
                server::launchServer(server::exileEscape); end();
                break;
                case '8':
                server::launchVN(server::prairieFire); end();
                break;
                case '9':
                settings();
                break;
                case '0':
                end();
                break;
            }
        }    
    }

    void optMods(){
        bool ret=false;
        while(!ret){
            bool opt=getVarB(L"CACCore\\memory2.txt");
            cls();
            if(opt){
                SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_GREEN);
            }else{
                SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_RED);
            }
            std::cout<<"OPTIONAL MODS: "<<(opt ? "ENABLED" : "DISABLED")<<'\n';
            std::cout<<" 1 ENABLE\n";
            std::cout<<" 2 DISABLE\n";
            if(opt){
                std::cout<<" 3 SELECT OPTIONAL MODS\n";
            }
            std::cout<<" "<<opt+3<<" RETURN\n";
            if(opt) sl=select("1234"); else sl=select("123");
            switch(sl){
                case '1':
                setVar(true,L"CACCore\\memory2.txt");
                break;
                case '2':
                setVar(false,L"CACCore\\memory2.txt");
                break;
                case '3':
                opt ? optModsSelect() : ret=true;
                case '4':
                ret=true;
                break;
            }
        } 
    }

    void optModsSelect(){
        bool ret=false;
        while(!ret){
            cls();
            std::string choice;
            int i=0;
            for(;i<server::optMods.num;++i){
                std::cout<<" "<<i; std::wcout<<server::optMods.dirs[i]<<"\t\t\tSTATUS: ";
                if(!std::filesystem::exists(server::optMods.dirs[i]+L"\\")) std::cout<<"NOT FOUND\n";
                else{
                    std::cout<<(getVarB(server::optMods.dirs[i]+L".txt")? "ENABLED" : "DISABLED")<<'\n';
                    choice+=i+48;
                }
            }
            std::cout<<"\n "<<i<<" Return\n";
            choice+=i+48;
            std::cout<<" Choose option: [ "; for(auto &c: choice) std::cout<<c<<' '; std::cout<<']';
            sl=select(choice);
            if(sl==i-48) ret=true;
            else{
                std::wstring ws=server::optMods.dirs[i-48]+L".txt";
                setVar(!getVarB(ws),ws);
            }//TODO proper testing
        }
    }

    void modCheck(){
        bool ret=false;
        while(!ret){
            cls();
        }
    }

    void exilePwd(){
        bool ret=false;
        while(!ret){
            cls();
        }
    }

    void modDir(){
        bool ret=false;
        while(!ret){
            cls();
        }
    }

    void username(){
        bool ret=false;
        while(!ret){
            cls();
        }
    }

    void end(){//will never return
        std::wstring logo=getVarWS(L"CACCore\\logo.txt");
        cls();
        std::wcout<<logo<<L"\n";
        Sleep(t_s);
        SetConsoleTextAttribute(menu::conScreenBuf, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
        if(!CloseHandle(conScreenBuf)) logErr();
        exit(0);
    }
}

#endif