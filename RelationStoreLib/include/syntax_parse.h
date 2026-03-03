#pragma once
#include<string>
#include<span>
namespace RelationStoreLib{
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
            std::span<const std::string> EndPoses;
            std::span<float> Weights;
        };
        struct BidArgPack
        {
            std::span<const std::string> Poses;
            std::span<float> Weight;
        };
    struct DoConnectionOpArgPack
        {
            std::string_view UniBeginPos;
            std::span<const std::string> UniEndPoses;
            std::span<const std::string> BidVertexs;
            std::span<float> Weights;
            DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<const std::string> &uniEndPoses, const std::span<const std::string> &BidVertexs,std::span<float> weights);
            DoConnectionOpArgPack(const UniArgPack &p);
            DoConnectionOpArgPack(const BidArgPack &p);
        };
}
