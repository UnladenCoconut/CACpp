#ifndef CAC_MENU
#define CAC_MENU

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <conio.h>
#include <filesystem>
#include "system.cpp"
#include "cfg.cpp"
#include "server.cpp"
#include "username.cpp"

WORD FOREGROUND_YELLOW=FOREGROUND_RED | FOREGROUND_GREEN,
FOREGROUND_WHITE=FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;

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
            if(sl==27) end(); //esc key quit
        } while(inputs.find(sl)==-1 && !(inputs==""));
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
            cls(); SetConsoleTextAttribute(conScreenBuf,FOREGROUND_GREEN);
            std::cout<<"\n 1 Optional Mods Management ["<<(getVarB(L"CACCore\\memory2.txt")? "ENABLED" : "DISABLED")<<"]\n";
            std::wcout<<L" 2 Change Username/Profile ["<<getVarWS(L"CACCore\\username.txt")<<L"]\n";
            std::wcout<<L" 3 Change Exile Password ["<<getVarWS(L"CACCore\\password.txt")<<L"]\n";
            std::cout<<" 4 Change Mods Directory ["; std::wcout<<server::getModDir(); std::cout<<"]\n";
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
            cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_GREEN);
            std::cout<<"\nSTEAM IS RUNNING\n";
            Sleep(t_s);
            main();
            
        } else {
            std::wstring steamPath=getVarWS(L"CACCore\\memory.txt");
            //TODO run steamPath first before search
            if(steamPath==L"" || !std::filesystem::exists(steamPath)){
                cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_YELLOW);
                std::cout<<"\nSEARCHING FOR STEAM, PLEASE WAIT...\n";
                steamPath=steamFind();
                if(steamPath!=L""){
                    cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_GREEN);
                    std::wcout<<L"\nSTEAM FOUND AT: "<<steamPath;
                    setVar(steamPath,L"CACCore\\memory.txt");
                    Sleep(t_s);
                } else {
                    bool found=false;
                    do{
                        cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_RED);
                        std::cout<<"\nFAILED TO FIND STEAM.\n";
                        std::cout<<"\n 1 Enter steam.exe path\n";
                        std::cout<<" 2 Exit\n";
                        sl=select("12");
                        if(sl=='1'){
                            cls();
                            std::cout<<"\nPlease enter path of your steam.exe. (Example: \"C:\\Steam.exe\")\n";
                            std::wcin>>steamPath;
                            if(std::filesystem::exists(steamPath)){//TODO should check that the filename is steam.exe or enter dir instead.
                                cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_YELLOW);
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
            cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_YELLOW);
            std::cout<<"\nSTEAM IS NOT RUNNING, ATTEMPTING TO RUN STEAM...\n";
            if(launch(steamPath,L"")){
                cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_GREEN);
                std::cout<<"\nSTEAM LAUNCHED SUCCESSFULLY.\n";
                Sleep(t_s);
                main();
            } else{
                cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_RED);
                std::cout<<"\nFAILED TO LAUNCH STEAM. LAUNCH STEAM MANUALLY IF ISSUE PERSISTS.\n";/*TODO allow user to launch steam manually
                and then continue.*/
                Sleep(t_s);
                end();
            }
        }
    }

    void main(){ //will never return
        while(true){
            cls(); SetConsoleTextAttribute(conScreenBuf, FOREGROUND_GREEN);
            std::cout<<"\nArma 3 CAC Launcher - discord.gg/dNGcyEYK8F\n";
            std::cout<<"\nVERSION: CAC++ 0.1 EXPERIMENTAL\n";
            std::wcout<<L"\nUSERNAME: "<<getVarWS(L"CACCore\\username.txt")<<L'\n';
            std::cout<<"\nOPTIONAL MODS: "<<(getVarB(L"CACCore\\memory2.txt")? "ENABLED" : "DISABLED")<<'\n';
            std::cout<<"\nSERVER STATUS: ?\n";
            std::cout<<"\n 1 Exile Altis "<<server::exileAltis->count();
            std::cout<<" 2 Exile Tanoa "<<server::exileTanoa->count();
            std::cout<<" 3 Coop PVE "<<server::coop->count();
            std::cout<<" 4 King of The Hill TVT "<<server::KoTH->count();
            std::cout<<" 5 Dynamic Recon Ops CUP "<<server::DRO->count();
            std::cout<<" 6 Antistasi RHS "<<server::antistasi->count();
            std::cout<<" 7 Exile Escape "<<server::exileEscape->count();
            std::cout<<" 8 Antistasi S.O.G. Prairie Fire "<<server::prairieFire->count();
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

        //TODO
        //mod may not be found, in this case should not be added to arma 3 launch. This requires changes in server.cpp.
        bool ret=false;
        while(!ret){
            bool opt=getVarB(L"CACCore\\memory2.txt");
            cls();
            if(opt){
                SetConsoleTextAttribute(conScreenBuf, FOREGROUND_GREEN);
            }else{
                SetConsoleTextAttribute(conScreenBuf, FOREGROUND_RED);
            }
            std::cout<<"\nOptional Mods: "<<(opt ? "Enabled" : "Disabled")<<'\n';
            std::cout<<" 1 Enable\n";
            std::cout<<" 2 Disable\n";
            if(opt){
                std::cout<<" 3 Select Optional Mods\n";
            }
            std::cout<<" "<<opt+3<<" Return\n";
            if(opt){
                std::cout<<"\nChoose Option (1-4): ";
                sl=select("1234");
            } else{
                std::cout<<"\nChoose Option (1-3): ";
                sl=select("123");
            }
            switch(sl){
                case '1':
                setVar(true,L"CACCore\\memory2.txt");
                break;
                case '2':
                setVar(false,L"CACCore\\memory2.txt");
                break;
                case '3':
                opt ? optModsSelect() : ret=true;
                break;
                case '4':
                ret=true;
                break;
            }
        } 
    }

    void optModsSelect(){
        while(true){
            cls();
            std::cout<<"\nSelect Mod To Switch:\n";
            //status can be enabled, disabled or not found.
            //TODO vanillasmokeforblastcore requires blastcore
            int len=0;
            for(int i=0;i<server::optMods.num;++i){
                if(len<server::optMods.dirs[i].length()) len=server::optMods.dirs[i].length();
            }

            std::string choices, status;
            int j=1;
            for(int i=0;i<server::optMods.num;++i){
                std::cout<<' ';
                std::wstring path=server::getModDir()+server::optMods.dirs[i]+L'\\';
                if(!std::filesystem::exists(path)){
                    std::cout<<' ';
                    status="Not Found\n";
                } else{
                    std::cout<<j; choices+='0'+j; ++j;
                    status=getVarB(L"CACCore\\"+server::optMods.dirs[i]+L".txt") ? "Enabled\n" : "Disabled\n";
                }
                std::wcout<<L' '+server::optMods.dirs[i];
                std::cout<<std::string(len-server::optMods.dirs[i].length()+1,' ')<<"Status: "<<status;
            }
            std::cout<<"\n "<<j<<" Return\n";
            choices+='0'+j;
            std::cout<<"\nChoose Option (1-"<<j<<"): ";
            sl=select(choices);
            if(sl=='0'+j) return;
            else{
                std::wstring ws=L"CACCore\\"+server::optMods.dirs[sl-1-'0']+L".txt";
                setVar(!getVarB(ws),ws);
            }
        }
    }

    std::wstring missedMods(server::Server *s){ //helper function
        std::wstring ret=L"";
        unsigned int mask=s->modCheck();
        for(unsigned int i=0;i<s->numMods;++i){
            if((mask&1U)==0) ret+=L' '+s->mods[i]+L"\n";//1U denotes unsigned literal, not signed
            mask>>=1;
        }
        return ret;
    }

    void modCheck(){
            cls();
            SetConsoleTextAttribute(conScreenBuf, FOREGROUND_YELLOW);
            auto check=[](server::Server *s) mutable {std::wstring ws=missedMods(s); return ws==L"" ?   L" No Missing Mods.\n" : ws; };

            std::cout<<"\nExile Altis\n";
            std::wcout<<check(server::exileAltis);

            std::cout<<"\nExile Tanoa\n";
            std::wcout<<check(server::exileTanoa);

            std::cout<<"\nCoop PVE\n";
            std::wcout<<check(server::coop);

            std::cout<<"\nKing of The Hill TVT\n";
            std::wcout<<check(server::KoTH);

            std::cout<<"\nDynamic Recon Ops CUP\n";
            std::wcout<<check(server::DRO);

            std::cout<<"\nAntistasi RHS\n";
            std::wcout<<check(server::antistasi);

            std::cout<<"\nExile Escape\n";
            std::wcout<<check(server::exileEscape);

            std::cout<<"\nAntistasi S.O.G. Prairie Fire\n";
            std::wcout<<check(server::prairieFire);

            std::cout<<"\nPress any key to return.\n";
            select("");
    }

    void exilePwd(){
        bool ret=false;
        while(!ret){
            cls();
            std::wcout<<L"Current Password: "<<getVarWS(L"CACCore\\password.txt")<<'\n';
            std::cout<<"\n 1 Change Password\n";
            std::cout<<" 2 Remove Password\n";
            std::cout<<" 3 Return\n";
            std::cout<<"\n Choose Option (1-3): ";
            sl=select("123");
            std::wstring ws;
            switch(sl){
                case '1':
                std::cout<<"\n New Password: ";
                std::getline(std::wcin,ws);
                setVar(ws,L"CACCore\\password.txt");
                break;
                case '2':
                setVar(L"",L"CACCore\\password.txt");
                break;
                case '3':
                ret=true;
                break;
            }
        }
    }

    void modDir(){
        bool ret=false;
        while(!ret){
            cls();
            std::cout<<"Current Mod Directory: "; std::wcout<<server::getModDir()+L'\n';
            std::cout<<"\n 1 Change Mod Directory\n";
            std::cout<<" 2 Default Directory\n";
            std::cout<<" 3 Return\n";
            std::cout<<"\n Choose Option (1-3): ";
            sl=select("123");
            std::wstring ws;
            switch(sl){
                case '1':
                std::cout<<"\nNew Mod Directory: ";
                std::getline(std::wcin,ws); 
                server::setModDir(ws);
                break;
                case '2':
                server::setModDir(std::wstring(L"Mods\\"));
                break;
                case '3':
                ret=true;
                break;
            }
        }
    }

    void username(){
        //TODO check that username input is valid, remove surrounding whitespace
        //same goes for entering mod dir.
        bool ret=false;
        while(!ret){
            cls();
            std::wstring username=getVarWS(L"CACCore\\username.txt");
            std::string txt;
            if(username==defaultUser){
                SetConsoleTextAttribute(conScreenBuf,FOREGROUND_BLUE);
                txt="\n Profile: Exists, is the system default.\n";
            }else if(std::filesystem::is_directory(altUserPath+username)){
                txt="\n Profile: Exists.\n"; SetConsoleTextAttribute(conScreenBuf,FOREGROUND_GREEN);
            }else{
                SetConsoleTextAttribute(conScreenBuf,FOREGROUND_YELLOW);
                txt="\n Warning: Profile non-existent, will be created at Arma 3 launch.\n";
            }
            std::cout<<"\n Note: different usernames will use seperate save games/profile folders.\n";
            std::wcout<<L"\n Current Username: "<<getVarWS(L"CACCore\\username.txt")<<((username==defaultUser)?L" (Default)":L"")<<L'\n';
            std::cout<<txt;
            std::cout<<"\n Existing Profiles: \n\n";
            for(auto &i: getAltUsers()) std::wcout<<L"  "<<i<<((i==defaultUser)?L" (Default)":L"")<<'\n';
            std::cout<<"\n 1 Set Username\n";
            std::cout<<" 2 Reset username to system default\n";
            std::cout<<" 3 Return\n";
            std::cout<<"\n Choose Option (1-3): "; 
            switch(select("123")){
                case '1':
                std::cout<<"\n Username: "; std::getline(std::wcin,username);
                setVar(username,L"CACCore\\username.txt");
                break;
                case '2':
                setVar(defaultUser,L"CACCore\\username.txt");
                break;
                case '3':
                ret=true;
                break;
            }
        }
    }

    void end(){//will never return
        SetConsoleTextAttribute(conScreenBuf, FOREGROUND_WHITE);
        std::wstring logo=getVarWS(L"CACCore\\logo.txt");
        cls();
        std::wcout<<logo<<L"\n";
        Sleep(t_s);
        if(!CloseHandle(conScreenBuf)) logErr();
        WSACleanup();
        exit(0);
    }
}

#endif