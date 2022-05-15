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
//TODO some files are ascii, some store wide char path

void setVar(std::wstring var, std::wstring path){//THIS DOESNT WORK!!
    std::wofstream f;
    f.exceptions(std::wfstream::badbit | std::wfstream::failbit);
    f.open(path,std::wfstream::out | std::wfstream::trunc);
    f<<var;
    f.close();
}

void setVar(bool b, std::wstring path){
    b? setVar(L"ENABLED",path): setVar(L"DISABLED",path);
}

void getVar(std::wstring &var,std::wstring path){ //return via args as cant overload via return type
    if(!std::filesystem::exists(path)){
        var=L""; return;
    }
    std::wifstream f;
    f.exceptions(std::wfstream::badbit | std::wfstream::failbit);
    f.open(path,std::wfstream::in);
    std::wostringstream fs;
    fs<<f.rdbuf();
    var=fs.str();
    f.close();
}

void getVar(bool &var,std::wstring path){
    if(!std::filesystem::exists(path)){
        var=false; return;
    }
    std::wstring boolStr;
    getVar(boolStr,path);
    if(boolStr==L"ENABLED"){
        var=true;
    } else {
        if(boolStr!=L"DISABLED"){
            setVar(L"DISABLED",path);
        }
        var=false;
    }
}

#endif