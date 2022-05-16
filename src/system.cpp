#ifndef CAC_STEAM
#define CAC_STEAM

#include <iostream>
#include <filesystem>
#include <windows.h>
#include <TlHelp32.h>
#include <filesystem>
#include "cfg.cpp"

void logErr(){//this logs windows system error messages to stderr (which is redirected to log file.)
    /* 
    log file of format: 'System error code: message' e.g.
    6: Invalid handle value

    I read somewhere that error messages mght come from Ntdsbmsg.dll ?
    */
    char *msg=NULL;
    DWORD err=GetLastError();
    DWORD chars;
    chars=FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err,0,(LPTSTR)&msg,0,NULL);
    //casting a pointer to a pointer, to a pointer, reeks of a bug caused by someone getting confused about pointers.
    if(chars==0){
        std::cerr<<err<<'\n'<<GetLastError()<<": format message failed.\n";
    }
    else std::cerr<<err<<": "<<msg<<'\n';
    LocalFree(msg);
}

//std::filesystem::path steamPath; //global

bool steamRunning(){
    bool ret=false;
    HANDLE hProcSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hProcSnap==INVALID_HANDLE_VALUE){
        std::cerr<<GetLastError()<<' ';
        return false;
    } else {
        PROCESSENTRY32 procInfo;
        procInfo.dwSize=sizeof(procInfo);
        if(Process32First(hProcSnap,&procInfo)){
            do{
                if(!strcmp(procInfo.szExeFile,"steam.exe")){
                    ret=true;
                    break;
                }
            } while(Process32Next(hProcSnap,&procInfo));
        } else{
            logErr();
        }
        CloseHandle(hProcSnap);
        return ret;
    }
}

std::wstring steamFind(){
    wchar_t drives[104];
    int chr_n=GetLogicalDriveStringsW(104,drives);
    if(chr_n>104 || chr_n==0){
        std::cerr<<"failed to read drives.";
        return L"";
    }

    wchar_t drive[4];
    drive[3]='\0';
    int ci=0;
    for(int i=0;i<chr_n;++i){
        if(drives[i]!='\0'){
            drive[ci]=drives[i];
            ++ci;
        } else{
            ci=0;
            std::filesystem::recursive_directory_iterator file_i(drive,std::filesystem::directory_options::skip_permission_denied);
            for(auto &&file: file_i){
                if(file.path().filename()=="steam.exe"){
                    return file.path().wstring();
                }
            }
        }
    }
    return false;
}

//this is also used by server to launch arma.
bool launch(std::filesystem::path path,std::wstring args){
    path=std::filesystem::absolute(path);
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si,sizeof(si));
    ZeroMemory(&pi,sizeof(pi));
    std::wstring cmdStr=path.wstring()+L" "+args;
    wchar_t * cmdCStr=new wchar_t[cmdStr.length()+1];
    wcscpy(cmdCStr,cmdStr.c_str());
    bool launched = CreateProcessW(path.c_str(),cmdCStr,NULL,NULL,false,CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE,NULL,path.parent_path().c_str(),&si,&pi);
    //TODO for actual arma 3 use CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS
    //TODO revise the process creation flags.
    delete cmdCStr;
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    if(!launched){
        logErr();
        return false;
    }
    return true;
}

#endif