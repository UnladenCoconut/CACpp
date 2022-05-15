#ifndef CAC_SERVER
#define CAC_SERVER

#include <filesystem>

#include "system.cpp"
#include "cfg.cpp"

//TODO username, optional mods

namespace server{

struct{
    const std::wstring dirs[5]={L"@ARM",L"@JSRS_SOUNDMOD",L"@DUI",L"@Blastcore",L"@VanillaSmokeForBlastcore"}; //mod folder name should be same as CACCore .txt filename, e.g. @ARM, @ARM.txt
    int num=5; 
} optMods;

struct Server{
    int port;
    const int numMods;
    std::wstring *mods=nullptr;
    
    ~Server(){
        delete mods;
    }

    std::wstring mkModArg(){
        std::wstring ret;
        for(int i=0;i<numMods;++i){
            ret+=modDir+mods[i]+L';';
        }
        bool b; getVar(b,L"CACCore\\memory2.txt");
        if(b){
            for(int i=0;i<optMods.num;++i){
                std::wstring fname(optMods.dirs[i]); fname+=L".txt";
                bool b1; getVar(b1,fname);
                if(b1) ret+=modDir+optMods.dirs[i]+L';';
            }
        }
        ret.pop_back();
        return ret;
    }

    unsigned int modCheck(Server s){ //returns a boolean bit mask of found mods.
        unsigned int ret;
        if(s.numMods>32) throw std::exception("numMods exceeds bit mask size.");
        for(int i;i<s.numMods;++i){
            if(std::filesystem::exists(s.mods[i])){
                ret|=(1<<i);
            }
        }
        return ret;
    }

    static std::wstring getModDir();
    static void setModDir(std::wstring &);

    private:
        static std::wstring modDir; //private as need to process slashes properly, use getter/setter instead.
};

//-------- declarations --------
std::wstring Server::modDir=L"", password=L"", ip=L"cacservers.ddns.net",
armaArgs=L"-skipIntro -noSplash -world=empty -exThreads=7 -enableHT -hugepages -connect="+ip;//TODO username

void init(){
    getVar(Server::getModDir(),L".\\CACCore\\moddir.txt");
    if(Server::getModDir()==L"") Server::setModDir(std::wstring(L".\\Mods\\"));
    getVar(password,L"CACCore\\password.txt");
}
 
//-------- getter/setter for modDir --------
std::wstring Server::getModDir(){ 
    std::wstring ret; getVar(ret,L"CACCore\\moddir.txt");
    for(auto &c: ret){
        if(c==L'/') c=L'\\';
    }
    if(ret.back()!=L'\\') ret+=L'\\';
    return ret;
}

void Server::setModDir(std::wstring &s){ //sets in file, but also processes s, s can be used as a return
    for(auto &c: s){
        if(c==L'/') c=L'\\';
    }
    if(s.back()!=L'\\') s+=L'\\';
    setVar(s,L"CACCore\\moddir.txt");
}

//--------- Server launch functions --------
bool launchServer(Server &s){
    std::wstring args=armaArgs+L" -mod="+s.mkModArg();
    return launch(L".\\arma3_x64.exe", args);
}

bool launchPwd(Server &s){ //for password protected servers
    std::wstring args=armaArgs+L" -password="+password+L" -mod="+s.mkModArg();
    return launch(".\\arma3_x64.exe", args);
}

bool launchVN(Server &s){
    std::wstring args=armaArgs+L" -mod=vn;"+s.mkModArg();
    return launch(L".\\arma3_x64.exe", args);
}

//-------- create servers --------
//if the number of mods is not right, then will cause out of bounds access

Server antistasi = {
    3302,
    24,
    new std::wstring[24] {L"@CBA_A3",L"@Anizay",L"@KunduzAfgFD",L"@TembelanIsland",L"@VirolahtiValtatie7",
    L"@CUPTerrainsCore",L"@CUPTerrainsMaps",L"@DSHouses",L"@InteriorsforCUP",L"@SM_Sheds",
    L"@RHSAFRF",L"@RHSGREF",L"@RHSUSAF",L"@EnhancedMovement",L"@EnhancedMovementRework",
    L"@MfHealAbort",L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@ace",
    L"@ACEComRHSAFRF",L"@ACEComRHSGREF",L"@ACEComRHSUSAF",L"@Blastcore"}
};

Server coop = {
    2702,
    9,
    new std::wstring[9] {L"@CBA_A3",L"@ace",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@MfHealAbort",
    L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@Blastcore"}
};

Server DRO = {
    3502,
    11,
    new std::wstring[11] {L"@CBA_A3",L"@CUPTerrainsCore",L"@CUPTerrainsMaps",L"@Fallujah",L"@ace",
    L"@CUPWeapons",L"@CUPUnits",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@AdvancedRappelling",
    L"@AdvancedUrbanRappelling"}
};

Server KoTH = {
    2322,
    4,
    new std::wstring[4] {L"@CBA_A3",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@MfHealAbort"}
};

Server exileAltis = {
    2302,
    9,
    new std::wstring[9] {L"@Exile",L"@CBA_A3",L"@DualArms",L"@EnhancedMovement",L"@EnhancedMovementRework",
    L"@Extended_Base_Mod",L"@X66-MammothTank",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling"}
};

Server exileTanoa = {
    2602,
    exileAltis.numMods,
    exileAltis.mods
};

Server exileEscape = {
    2372,
    5,
    new std::wstring[5] {L"@Exile",L"@CBA_A3",L"@DualArms",L"@EnhancedMovement",L"@EnhancedMovementRework"},
};

Server prairieFire = {
    3402,
    10,
    new std::wstring[10] {L"@CBA_A3",L"@ace",L"@ace_compat_sog",L"@EnhancedMovement",L"@EnhancedMovementRework",
    L"@MfHealAbort",L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@Blastcore"}
};

}



#endif