#pragma once
#include<string>
#include<span>
#include <ranges>
#include<vector>
#include<format>
namespace RelationStoreLib::Syntax{
    enum class OpType
        {
            Add,
            Rem
        };
        enum class GraphType
        {
            Uni,
            Bid
        };
    struct UniArgPack
        {
            std::string_view BeginPos;
            std::vector<std::string> EndPoses;
            std::vector<float> Weights;
        };
        struct BidArgPack
        {
            std::vector<std::string> Poses;
            std::vector<float> Weight;
        };
    struct DoConnectionOpArgPack
        {

            std::string_view UniBeginPos;
            std::vector<std::string> UniEndPoses;
            std::vector<std::string> BidVertexs;
            std::vector<float> Weights;
            DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<std::string> &uniEndPoses, const std::span< std::string> &BidVertexs,const std::span<float> &weights);
            DoConnectionOpArgPack(const UniArgPack &p);
            DoConnectionOpArgPack(const BidArgPack &p);
            DoConnectionOpArgPack() = default;
        };
    std::string Join(std::vector<std::string> v,std::string s);
    class SyntaxParser{
        public:
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
        return std::format("{}({}),[{}]", opName, Join(sa, ","), vecFloatToVecString(arg.Weights));
    }
};
    
}
