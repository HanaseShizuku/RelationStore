#pragma once
#include<string>
#include<span>
#include <ranges>
#include<vector>
#include<format>
#include<memory>
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
            std::vector<int> Weights;
        };
        struct BidArgPack
        {
            std::vector<std::string> Poses;
            std::vector<int> Weight;
        };
    struct DoConnectionOpArgPack
        {

            std::string UniBeginPos;
            std::vector<std::string> UniEndPoses;
            std::vector<std::string> BidVertexs;
            std::vector<int> Weights;
            DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<std::string> &uniEndPoses, const std::span< std::string> &BidVertexs,const std::span<int> &weights);
            DoConnectionOpArgPack(const UniArgPack &p);
            DoConnectionOpArgPack(const BidArgPack &p);
            DoConnectionOpArgPack() = default;
        };
    }