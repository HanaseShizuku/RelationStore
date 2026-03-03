#include<string>
#include<vector>
#include"utils/string.hpp"
#include<iostream>

using namespace shizuku::util::string;
bool IsSingleToken(const char &c){
    constexpr char car[]={'(',')','[',']',',',';','\n','\r'};
    for(const char &x:car){
        if (c==x)
        {
            return true;
        }
        
    }
    return false;
}
std::string EraseEscape(const std::string &str){
    return Replace(Replace(str,"\\\\","\\"),"\\\"","\"");
}


std::vector<std::string> TokenParse(std::string strs){
    int b=0, i=0;
    bool quote=false, escape=false;
    std::vector<std::string> tokens;
    auto addtoken= [&](const std::string &t){
        std::string cleaned = Trim(t);
        if(!cleaned.empty()) tokens.push_back(cleaned);
    };

    while(i < strs.size()){
        if (quote) {
            if(strs[i] == '"' && !escape){
                addtoken(EraseEscape(strs.substr(b + 1, i - b - 1)));
                b = i + 1;
                quote = false;
            } else if(strs[i] == '\\' && !escape){
                escape = true;
            } else {
                escape = false;
            }
        } else {
            if(strs[i] == '"'){
                addtoken(strs.substr(b, i - b));
                b = i;
                quote = true;
            } else if(IsSingleToken(strs[i]) || std::isspace(strs[i])){
                addtoken(strs.substr(b, i - b));
                if(IsSingleToken(strs[i])) {
                    tokens.push_back(std::string(1, strs[i]));
                }
                b = i + 1;
            }
        }
        i++;
    }
    addtoken(strs.substr(b));
    return tokens;
}

void Parse(std::string strs){

}