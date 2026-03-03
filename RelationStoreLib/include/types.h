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
    }