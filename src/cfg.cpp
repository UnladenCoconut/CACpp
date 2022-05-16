#ifndef CAC_CFG
#define CAC_CFG

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

//these functions get and store variables stored in CACCore config txt files
//in future, it would be better to use either registry keys or a single configuration file
//TODO better file exception handling

void setVar(const std::wstring var, std::wstring path){
    std::wofstream f;
    f.exceptions(std::wfstream::badbit | std::wfstream::failbit);
    f.open(path,std::wfstream::out | std::wfstream::trunc);
    f<<var;
    f.close();
}

//fixes problems with c string resolving to bool instead of string
void setVar(const wchar_t* var, std::wstring path){
    setVar(std::wstring(var),path);
}

void setVar(const bool b, std::wstring path){
    b? setVar(L"ENABLED",path): setVar(L"DISABLED",path);
}

std::wstring getVarWS(std::wstring path){
    std::wstring var;
    if(!std::filesystem::exists(path)){
        return L"";
    }
    std::wifstream f;
    f.exceptions(std::wfstream::badbit | std::wfstream::failbit);
    f.open(path,std::wfstream::in);
    std::wostringstream fs;
    fs<<f.rdbuf();
    var=fs.str();
    f.close();
    return var;
}

bool getVarB(std::wstring path){
    auto var=getVarWS(path);
    if(var==L"ENABLED"){
        return true;
    }else{
        if(var!=L"DISABLED"){
            setVar(L"DISABLED",path);
        }
        return false;
    }
}

#endif