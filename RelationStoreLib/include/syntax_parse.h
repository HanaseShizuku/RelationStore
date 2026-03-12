#pragma once
#include<string>
#include<span>
#include <ranges>
#include<vector>
#include<format>
#include<memory>
#include"include/syntax_nodes.h"
#include "include/types.h"
#include <iostream>
namespace RelationStoreLib::Syntax{
    
        
        std::string Join(std::vector<std::string> v, std::string s);
        class SyntaxParser
        {
        public:
        static std::vector<std::unique_ptr<GeneralSyntaxNode>> Parse(std::string strs);
        static bool IsSingleToken(const char &c);
    static std::string EraseEscape(const std::string &str);
    static std::vector<std::string> TokenParse(std::string strs);
    template <typename T>
    static std::vector<std::span<T>> SplitVectorToSpans(std::vector<T> &vec, const T &delimiter);
    template <GraphType graph>
    static std::string ArgPackToScriptArg(const DoConnectionOpArgPack &arg, const std::string &opName, const std::string &relationName)
    {
        auto vecFloatToVecString = [](const std::vector<float> &fs)
        {
            return fs | std::views::transform([](float f)
                                              {
                                                  return std::format("{}", f); 
                                              }) |
                   std::ranges::to<std::vector<std::string>>();
        };
        std::vector<std::string> sa;
        if (graph == GraphType::Uni)
        { // Uni
            sa.reserve(2 + arg.UniEndPoses.size());
            sa.push_back(relationName);
            sa.push_back(std::string(arg.UniBeginPos));
            sa.insert(sa.end(), arg.UniEndPoses.begin(), arg.UniEndPoses.end());
        }
        else
        {
            sa.reserve(1 + arg.UniEndPoses.size());
            sa.push_back(relationName);
            sa.insert(sa.end(), arg.BidVertexs.begin(), arg.BidVertexs.end());
        }
        std::string result= std::format("{}({}){};", opName, Join(sa, ","), vecFloatToVecString(arg.Weights));
        return result;
    }
};
    
}
