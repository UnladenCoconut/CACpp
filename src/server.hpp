#ifndef CAC_SERVER_IMPL
#define CAC_SERVER_IMPL

#include <exception>
#include <filesystem>

#include <WinSock2.h>
#include <WinDNS.h>

#include "ssq/a2s.h"

#include "system.hpp"
#include "cfg.hpp" 

namespace server{

//-------- declarations --------

std::string dns="unladencoconut.ddns.net",ip;
bool up(false);
std::wstring armaArgs=L"-skipIntro -noSplash -world=empty -exThreads=7 -enableHT -hugepages -connect=cacservers.ddns.net";
//-------- getter/setter for modDir --------

std::wstring getModDir(){ 
    std::wstring ret=getVarWS(L"CACCore\\moddir.txt");
    bool b=false;
    for(auto &c: ret){
        if(c==L'/'){
            c=L'\\';
            b=true;
        }
    }
    if(ret.back()!=L'\\'){
        ret+=L'\\';
        b=true;
    }
    if(b) setVar(ret,L"CACCore\\moddir.txt");
    return ret;
}

void setModDir(std::wstring &s){ //sets in file, but also formats s, s can be used as a return 
    for(auto &c: s){
        if(c==L'/') c=L'\\';
    }
    if(s.back()!=L'\\') s+=L'\\';
    setVar(s,L"CACCore\\moddir.txt");
}

//names of optional mods. Name should be the same as the mod folder.
const struct{
    const std::wstring dirs[5]={L"@ARM",L"@JSRS_SOUNDMOD",L"@DUI",L"@Blastcore",L"@VanillaSmokeForBlastcore"}; 
    //mod folder name should be same as CACCore .txt filename, e.g. @ARM, @ARM.txt.
    const int num=5; 
} optMods;

//-------- Server class --------
struct Server{
    int port;
    int numMods;
    std::wstring *mods=nullptr;
    SSQ_SERVER *querier;
    Server(int p, int n, std::wstring *wsp): port(p), numMods(n), mods(wsp) {
        if((querier=ssq_server_new(ip.c_str(),port))==nullptr){
            delete mods;
            throw std::runtime_error("failed to initialise querier.");
        }
        ssq_server_timeout(querier,SSQ_TIMEOUT_SEND,500);//set the timeout for getting servr count here, in ms
        ssq_server_timeout(querier,SSQ_TIMEOUT_RECV,500);
        if(!ssq_server_eok(querier)){
            std::cerr<<"SSQ Error: "<<ssq_server_emsg(querier)<<'\n';
            }
    }
    ~Server(){
        delete[] mods;
        if(querier!=nullptr) ssq_server_free(querier);
    }

    std::wstring mkModArg(){
        std::wstring ret,modDir=getVarWS(L"CACCore\\moddir.txt");
        for(int i=0;i<numMods;++i){
            ret+=modDir+mods[i]+L';';
        }
        if(getVarB(L"CACCore\\memory2.txt")){
            for(int i=0;i<optMods.num;++i){
                std::wstring fname(optMods.dirs[i]); fname+=L".txt";
                if(getVarB(fname)) ret+=modDir+optMods.dirs[i]+L';';
            }
        }
        ret.pop_back();
        return ret;
    }

    unsigned int modCheck(){ //returns a boolean bit mask of found mods.
        unsigned int ret=0;
        if(numMods>32) throw std::exception("numMods exceeds bit mask size.");
        for(int i=0;i<numMods;++i){
            if(std::filesystem::exists(getModDir()+mods[i]+L"\\")) {
                ret|=(1<<i);
            }
        }
        return ret;
    }

    std::string count(){//no of players or status of server.
        A2S_INFO *info=ssq_info(querier);
        if(!ssq_server_eok(querier)){
            std::cerr<<ssq_server_emsg(querier)<<'\n';
            ssq_server_eclr(querier);
            return "?/?\n";
        }
        std::string ret=std::to_string(info->players)+'/'+std::to_string(info->max_players)+'\n';
        ssq_info_free(info);
        return ret;
    }
};

//--------- Server launch functions --------

bool launchServer(Server *s){
    std::wstring args=armaArgs+L" -port="+std::to_wstring(s->port)+L" -mod="+s->mkModArg()+L" -name="+getVarWS(L"CACCore\\username.txt");
    return launch(L".\\arma3_x64.exe", args);
}

bool launchPwd(Server *s){ //for password protected servers
    std::wstring args=armaArgs+L" -port="+std::to_wstring(s->port)+L" -password="+getVarWS(L"CACCore\\password.txt")+L" -mod="+s->mkModArg()+L" -name="+getVarWS(L"CACCore\\username.txt");
    return launch(".\\arma3_x64.exe", args);
}

bool launchVN(Server *s){
    //TODO blastcore is both an optional mod and required by VN
    std::wstring args=armaArgs+L" -port="+std::to_wstring(s->port)+L" -mod=vn;"+s->mkModArg()+L" -name="+getVarWS(L"CACCore\\username.txt");
    return launch(L".\\arma3_x64.exe", args);
}

//-------- create servers --------
//if the number of mods is not right, then will cause out of bounds access

Server *antistasi,*coop,*DRO,*KoTH,*exileAltis,*exileTanoa,*exileEscape,*prairieFire;

bool init(){

    //initialise winsock
    WSADATA info;
    int err=WSAStartup(MAKEWORD(2,2),&info);
    switch(err){
        case 0: break;
        std::cerr<<"Failed to initialise winsock: ";

        case WSASYSNOTREADY:
        std::cerr<<"WSASYSNOTREADY\n";
        break;

        case WSAVERNOTSUPPORTED:
        std::cerr<<"WSAVERNOTSUPPORTED\n";
        break;

        case WSAEINPROGRESS:
        std::cerr<<"WSAEINPROGRESS\n";
        break;

        case WSAEPROCLIM:
        std::cerr<<"WSAEPROCLIM\n";
        break;

        case WSAEFAULT:
        std::cerr<<"WSAEFAULT\n";
        break;
    }
    if(err) return false;

    //get ip address of CAC DNS
    DNS_RECORD* records;
    if(DnsQuery_A(dns.c_str(),DNS_TYPE_A,DNS_QUERY_STANDARD,NULL,&records,NULL)){
        std::cerr<<"Failed to get dns records for "<<dns<<'\n';
        logErr();
    } else{
        DWORD ipBit=records->Data.A.IpAddress;
        for(int i=0;i<4;++i){
            ip+=std::to_string(ipBit & 0b11111111);
            ipBit>>=8;
            ip+='.';
        }
        ip.pop_back();
        while(records!=nullptr){
            DNS_RECORD* old=records;
            records=records->pNext;
            DnsRecordListFree(old,DnsFreeRecordList);
        }
    }

    //create servers
    antistasi = new Server{
        3302,
        24,
        new std::wstring[24] {L"@CBA_A3",L"@Anizay",L"@KunduzAfgFD",L"@TembelanIsland",L"@VirolahtiValtatie7",
        L"@CUPTerrainsCore",L"@CUPTerrainsMaps",L"@DSHouses",L"@InteriorsforCUP",L"@SM_Sheds",
        L"@RHSAFRF",L"@RHSGREF",L"@RHSUSAF",L"@EnhancedMovement",L"@EnhancedMovementRework",
        L"@MfHealAbort",L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@ace",
        L"@ACEComRHSAFRF",L"@ACEComRHSGREF",L"@ACEComRHSUSAF",L"@Blastcore"}
    };

    coop = new Server{
        2702,
        9,
        new std::wstring[9] {L"@CBA_A3",L"@ace",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@MfHealAbort",
        L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@Blastcore"}
    };

    DRO = new Server{
        3502,
        11,
        new std::wstring[11] {L"@CBA_A3",L"@CUPTerrainsCore",L"@CUPTerrainsMaps",L"@Fallujah",L"@ace",
        L"@CUPWeapons",L"@CUPUnits",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@AdvancedRappelling",
        L"@AdvancedUrbanRappelling"}
    };

    KoTH = new Server{
        2322,
        4,
        new std::wstring[4] {L"@CBA_A3",L"@EnhancedMovement",L"@EnhancedMovementRework",L"@MfHealAbort"}
    };

    exileAltis = new Server{
        2302,
        9,
        new std::wstring[9] {L"@Exile",L"@CBA_A3",L"@DualArms",L"@EnhancedMovement",L"@EnhancedMovementRework",
        L"@Extended_Base_Mod",L"@X66-MammothTank",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling"}
    };

    exileTanoa = new Server{
        2602,
        exileAltis->numMods,
        exileAltis->mods
    };

    exileEscape = new Server{
        2372,
        5,
        new std::wstring[5] {L"@Exile",L"@CBA_A3",L"@DualArms",L"@EnhancedMovement",L"@EnhancedMovementRework"},
    };

    prairieFire = new Server{
        3402,
        10,
        new std::wstring[10] {L"@CBA_A3",L"@ace",L"@ace_compat_sog",L"@EnhancedMovement",L"@EnhancedMovementRework",
        L"@MfHealAbort",L"@VET_Unflipping",L"@AdvancedRappelling",L"@AdvancedUrbanRappelling",L"@Blastcore"}
    };

}

}

#endif