#pragma once
#include "relation_store_lib_export.h"
#include <set>
#include <filesystem>
#include <vector>
#include<string>
#include<map>
#include<span>
namespace RelationStoreLib
{

    class RELATION_STORE_LIB_EXPORT RelationStore
    {
    private:
        struct UniArgPack
        {
            std::string_view BeginPos;
            std::span<const std::string> EndPoses;
        };
        struct BidArgPack
        {
            std::span<const std::string> Poses;
        };
        using Graph = std::map<std::string, std::set<std::string>>;
        using Path = std::filesystem::path;
        struct RelationshipLine
        {
            std::string name;
            std::string content;
        };

        struct DoConnectionOpArgPack
        {
            std::string_view UniBeginPos;
            std::span<const std::string> UniEndPoses;
            std::span<const std::string> BidVertexs;
            DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<const std::string> &uniEndPoses, const std::span<const std::string> &BidVertexs);
            DoConnectionOpArgPack(const UniArgPack &p);
            DoConnectionOpArgPack(const BidArgPack &p);
        };
        Graph _graph;
        Path _tablePath;
        std::vector<RelationshipLine> _graphText;
        std::set<std::string> _names;
        bool _isInstance = false;
        void _SetName(const std::string &name);
        void _AddUniConnection(const std::string &beginPos, std::span<const std::string> endPoses);
        void _AddBidConnection(std::span<const std::string> vertexs);
        void _RemoveUniConnection(const std::string &beginPos, std::span<const std::string> endPoses);
        void _RemoveBidConnection(std::span<const std::string> vertexs);

        void _AddUniConnection(const DoConnectionOpArgPack &arg);
        void _AddBidConnection(const DoConnectionOpArgPack &arg);
        void _RemoveUniConnection(const DoConnectionOpArgPack &arg);
        void _RemoveBidConnection(const DoConnectionOpArgPack &arg);
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

        template <OpType optype, GraphType graphtype>
        static constexpr std::string_view _GetPrefixByTemplate();
        template <RelationStore::OpType optype, RelationStore::GraphType graphtype>
        void _AddRelationshipText(const std::string &name, const std::vector<std::string> &endposes, const std::string &beginPos = "");
        void _AddRelationshipText(const std::string &fullName, const std::vector<std::string> &fullVector);
        void _SetTextsToVector();
        template <RelationStore::OpType optype, RelationStore::GraphType graphType>
        void _FuncHandler(const DoConnectionOpArgPack &arg);
        template <RelationStore::OpType optype, RelationStore::GraphType graphtype>
        void _MakeRelationship(const std::string &relationName, std::conditional_t<graphtype == GraphType::Uni, UniArgPack, BidArgPack> graphArg);
        template <RelationStore::OpType optype, RelationStore::GraphType graphtype>
        void _RemoveByNameAndType(const std::string &relationName);

    public:
        RelationStore(Path tablePath);
        static RelationStore NewGraphToFile(Path tablePath);
        void AddUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void AddBid(const std::string &name, const std::vector<std::string> &vertexs);
        void RemoveUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void RemoveBid(const std::string &name, const std::vector<std::string> &vertexs);

        void UndoAddUni(const std::string &name);
        void UndoAddBid(const std::string &name);
        void UndoRemoveUni(const std::string &name);
        void UndoRemoveBid(const std::string &name);

        bool ReadGraph();
        Graph GetGraph();
        Graph GetGraphWithoutIsolatedNode();

        void SaveGraphTo(Path tablePath);
        void SaveGraph();
    };

}