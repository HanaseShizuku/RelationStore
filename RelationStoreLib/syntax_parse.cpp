#include<string>
#include<vector>

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
//[begin,end)
std::string SubString(std::string &str,int begin,int end){
    return str.substr(begin,end-begin+1);
}

void TokenParse(std::string strs){
    int b=0;
    int i=0;
    bool quote=false;
    bool escape=false;
    std::vector<std::string> nonprecessedTokens;
    while(i<strs.size()){
        if (quote)
        {
            if(strs[b]=='"'){
                //遇到引号下的引号
                if(escape){
                    //转义引号
                    //什么也不干
                }else{
                    //引号结束
                    //获取之前的东西
                    //并把转义符号换成正常符号
                    //接着塞进token表
                }
            }else if(strs[b]=='\\'){
                //引号内转义号
                if(escape){
                    //反斜杠
                    //转义关 然后什么也不干
                }else{
                    //转义号
                    //转义开 然后什么也不干
                }
            }else{
                //引号内的正常字符
                //什么也不干
                //当作一整个token解析就行了
            }
        }else{
            if(strs[b]=='"'){
                //字符串的开始
                //把之前的东西塞进token表里并去除空格
                //然后引用 开
            }else if(IsSingleToken(strs[b])){
                //结构字符
                //把之前的东西塞进token表里并去除空格
                //然后把自身塞进token表里
            }else{
                //非字符串情况下
                //遇到非结构符号什么都不做就行了
                //当一整个token解析了
                //空格也一样
            }
        }

        b++;
        
    }
    
}

void Parse(std::string strs){

}