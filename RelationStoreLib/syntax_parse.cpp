#include<string>
#include<vector>
#include"utils/string.hpp"
#include<span>
#include<memory>
#include<concepts>
#include "include/syntax_parse.h"
namespace RelationStoreLib{
using namespace shizuku::util::string;    


/*
语法
Uni(Name,begin,end1,end2)[weight1,weight2]; 有向
Bid(Name,P1,P2,P3,P4,...)[same_weight]; 全连接/无向
RmUni(Name,begin,end)[]
RmBid(Name,P1,P2)[]
*/


DoConnectionOpArgPack::DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<std::string> &uniEndPoses, const std::span< std::string> &BidVertexs,const std::span<float> &weights) : UniBeginPos(uniBeginPos),
                                                                                                                                                                                                  UniEndPoses(std::vector(uniEndPoses.begin(),uniEndPoses.end())),
                                                                                                                                                                                                  BidVertexs(std::vector(BidVertexs.begin(),BidVertexs.end())),
                                                                                                                                                                                                  Weights(std::vector(weights.begin(),weights.end())){}

DoConnectionOpArgPack::DoConnectionOpArgPack(const UniArgPack &p) : UniBeginPos(p.BeginPos), UniEndPoses(p.EndPoses),Weights(p.Weights) {}
DoConnectionOpArgPack::DoConnectionOpArgPack(const BidArgPack &p) : BidVertexs(p.Poses),Weights(p.Weight){}
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
template <typename T>
std::vector<std::span<T>> SplitVectorToSpans(std::vector<T>& vec, const T& delimiter) {
    std::vector<std::span<T>> result;
    if (vec.empty()) {
        return result;
    }
    T* start = vec.data();
    T* current = vec.data();
    T* end = vec.data() + vec.size();
    while (current < end) {
        if (*current == delimiter) {
            if (start != current) {
                result.emplace_back(start, current - start);
            }
            start = current + 1;
        }
        current++;
    }
    if (start < end) {
        result.emplace_back(start, end - start);
    }
    return result;
}



class GeneralSyntaxNode{
    protected:
    DoConnectionOpArgPack _ArgPack;
    OpType _Optype;
    GraphType _GraphType;
    
    public:
    std::string OpName;
    std::string RelationName;
    virtual void Init(const std::span<std::string>&sa,const std::span<std::string>&fa) = 0;
    
};
template <typename T>
concept IsSyntaxNode=std::is_base_of_v<GeneralSyntaxNode,T>;
template <IsSyntaxNode T>
    static std::unique_ptr<T> Create(std::span<std::string>sa,std::span<std::string>fa,const std::string &opname) {
        std::vector<std::string> sa_filtered, fa_filtered;
        for(auto &s : sa) if(s != ",") sa_filtered.push_back(s);
        for(auto &f : fa) if(f != ",") fa_filtered.push_back(f);
        std::unique_ptr<T> obj = std::make_unique<T>();
        //obj->RelationName = sa_filtered[0]; 
        //obj->OpName = opname;
        obj->Init(sa_filtered, fa_filtered);
        return obj;
    }
template<GraphType graph>
DoConnectionOpArgPack GetArgPack(const std::span<std::string>sa,const std::span<std::string>fa){
    std::vector<float> fs;
            for(const auto &f:fa){
                fs.push_back(std::stof(f));
            }
    if constexpr(graph==GraphType::Uni){
        auto x=sa.subspan(2);
            std::vector<std::string> pEnd=std::vector(x.begin(),x.end());        
            return DoConnectionOpArgPack (UniArgPack{.BeginPos=sa[1],.EndPoses=pEnd,.Weights=fs});
    }else{
        auto x=sa.subspan(1);
            std::vector<std::string> pEnd=std::vector(x.begin(),x.end());
            return DoConnectionOpArgPack (BidArgPack{.Poses=pEnd,.Weight=fs});
    }
}
class AddUniNode: public GeneralSyntaxNode{
    public:
        void Init(const std::span<std::string>&sa,const std::span<std::string>&fa) override{
            _Optype=OpType::Add;
            _GraphType=GraphType::Uni;
            _ArgPack=GetArgPack<GraphType::Uni>(sa,fa);
            
        }
};
class AddBidNode:public GeneralSyntaxNode{
    public:
        void Init(const std::span<std::string>&sa,const std::span<std::string>&fa) override{
            _Optype=OpType::Add;
            _GraphType=GraphType::Bid;
            _ArgPack=GetArgPack<GraphType::Bid>(sa,fa);
        }
};

class RemUniNode: public GeneralSyntaxNode{
    public:
        void Init(const std::span<std::string>&sa,const std::span<std::string>&fa) override{
            _Optype=OpType::Rem;
            _GraphType=GraphType::Uni;
            _ArgPack=GetArgPack<GraphType::Uni>(sa,fa);
            
        }
};
class RemBidNode:public GeneralSyntaxNode{
    public:
        void Init(const std::span<std::string>&sa,const std::span<std::string>&fa) override{
            _Optype=OpType::Rem;
            _GraphType=GraphType::Bid;
            _ArgPack=GetArgPack<GraphType::Bid>(sa,fa);
        }
};


std::unique_ptr<GeneralSyntaxNode> TokenToSyntaxNode(std::span<std::string> span){
        auto opName=span[0];
        auto sab=std::find(span.begin(),span.end(),"(");
        auto sae=std::find(span.begin(),span.end(),")");
        if(sab>sae){
            throw std::runtime_error("syntax error");
        }
        auto _StringArr = std::span(&(* (sab + 1) ), &(*sae));
        auto fab=std::find(span.begin(),span.end(),"[");
        auto fae=std::find(span.begin(),span.end(),"]");
        if(fab>fae){
            throw std::runtime_error("syntax error");
        }
        auto _FloatArr=std::span(&(* (fab + 1) ), &(*fae));
        if(opName=="Uni"){
            return Create<AddUniNode>(std::span(_StringArr),std::span(_FloatArr),opName);
        }else if(opName=="Bid"){
            return Create<AddBidNode>(std::span(_StringArr),std::span(_FloatArr),opName);
        }else if(opName=="RemUni"){
            return Create<RemUniNode>(std::span(_StringArr),std::span(_FloatArr),opName);
        }else if(opName=="RemBid"){
            return Create<RemBidNode>(std::span(_StringArr),std::span(_FloatArr),opName);
        }else{
            throw std::runtime_error("syntax error");
        }
}
std::vector<std::unique_ptr<GeneralSyntaxNode>> TokensToSytaxNodes(const std::vector<std::span<std::string>> &nodes){
    std::vector<std::unique_ptr<GeneralSyntaxNode>> r;
    for(const auto &n:nodes){
        r.push_back(TokenToSyntaxNode(n));
    }
    return r;
}
std::vector<std::unique_ptr<GeneralSyntaxNode>> Parse(std::string strs){
    std::vector<std::string> tokens=TokenParse(strs);
    const std::vector<std::span<std::string>> nodes=SplitVectorToSpans<std::string>(tokens,";");
    return TokensToSytaxNodes(nodes);
    //Uni ( 名字 , 起点 , 终点 ) ;
    //UniWeight ( 带引号的名字(") , 带转义符号的起点\ , 终点 ) [ weight ] ;
}
}

