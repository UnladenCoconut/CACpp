#ifndef CAC_USER
#define CAC_USER

#include <windows.h>
#include <lmcons.h>
#include <userenv.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

std::wstring altUserPath(L""), defaultUser(L"");

bool userInit(){
    HANDLE hProc=GetCurrentProcessToken();
    wchar_t *wcs = new wchar_t[512];
    DWORD n=1024;
    if(!GetUserProfileDirectoryW(hProc,wcs,&n)){
        if(GetLastError()==122){//if size of wcs array is too small
            delete wcs; wcs=new wchar_t[n];
            if(GetUserProfileDirectoryW(hProc,wcs,&n)){
                altUserPath=wcs;
            }else{std::cerr<<"Failed to find logged on user path."; delete wcs; return false;}
        } else {std::cerr<<"Failed to find logged on user path."; delete wcs; return false;}
    }else altUserPath=wcs;
    delete wcs;
    if(altUserPath!=L"") altUserPath+=L"\\Documents\\Arma 3 - Other Profiles\\";
    n=UNLEN+1;
    wcs=new wchar_t[UNLEN+1];
    if(!GetUserNameW(wcs,&n)) {std::cerr<<"Failed to get logged on user's name."; delete wcs; return false;}
    defaultUser=wcs;
    delete wcs;
    return true;
}

std::vector<std::wstring> getAltUsers(){
    std::vector<std::wstring> ret;
    if(std::filesystem::exists(altUserPath)){    
        for(auto &entry: std::filesystem::directory_iterator(altUserPath,std::filesystem::directory_options::skip_permission_denied)){
            if(entry.is_directory()) ret.push_back(entry.path().filename().wstring());
        }
    }
    return ret;
}


#endif