#pragma once
#include "relation_store_lib_export.h"
#include <set>
#include <filesystem>
#include <vector>
#include<string>
#include<map>
#include<span>
#include<memory>
#include"syntax_parse.h"
#include"syntax_nodes.h"
namespace RelationStoreLib
{

    struct AdjNode
        {
            std::string AdjacentNodeName;
            int Weight;
            AdjNode(const std::string& nodeName) 
                : AdjacentNodeName(nodeName), Weight(0.0f) {}
            AdjNode()=default;
            AdjNode(const std::string &nodeName,int weight)
                : AdjacentNodeName(nodeName),Weight(weight){}
        };
        struct AdjNodeComparator {
            bool operator()(const AdjNode& a, const AdjNode& b) const {
                return a.AdjacentNodeName<b.AdjacentNodeName;
            }

            bool operator()(const AdjNode& a, const std::string& b) const {
                return a.AdjacentNodeName<b;
            }

            bool operator()(const std::string& a, const AdjNode& b) const {
                return a<b.AdjacentNodeName;
            }
        };
        
        using Graph = std::map<std::string, std::set<AdjNode,AdjNodeComparator>>;
    class RELATION_STORE_LIB_EXPORT RelationStore
    {
    private:
        
        
        using Path = std::filesystem::path;
        struct RelationshipLine
        {
            std::string name;
            std::string content;
        };

        
        Graph _graph;
        Path _tablePath;
        std::vector<std::unique_ptr<Syntax::GeneralSyntaxNode>> _graphText;
        std::set<std::string> _names;
        bool _isInstance = false;
        void _SetName(const std::string &name);
        void _AddUniConnection(const std::string &beginPos, std::span<const std::string> endPoses,const std::vector<int> &weight);
        void _AddBidConnection(std::span<const std::string> vertexs,const std::vector<int> &weight);
        void _RemoveUniConnection(const std::string &beginPos, std::span<const std::string> endPoses);
        void _RemoveBidConnection(std::span<const std::string> vertexs);

        void _AddUniConnection(const Syntax::DoConnectionOpArgPack &arg);
        void _AddBidConnection(const Syntax::DoConnectionOpArgPack &arg);
        void _RemoveUniConnection(const Syntax::DoConnectionOpArgPack &arg);
        void _RemoveBidConnection(const Syntax::DoConnectionOpArgPack &arg);
        bool _ReadGraph(Path tablePath);

        template <Syntax::OpType optype, Syntax::GraphType graphtype>
        static constexpr std::string_view _GetPrefixByTemplate();
        void _SetTextsToVector();
        template <Syntax::OpType optype, Syntax::GraphType graphType>
        void _FuncHandler(const Syntax::DoConnectionOpArgPack &arg);
        template <Syntax::OpType optype, Syntax::GraphType graphtype>
        void _MakeRelationship(const std::string &relationName, std::conditional_t<graphtype == Syntax::GraphType::Uni, Syntax::UniArgPack, Syntax::BidArgPack> graphArg);
        template <Syntax::OpType optype, Syntax::GraphType graphtype>
        void _RemoveByNameAndType(const std::string &relationName);
        RelationStore(Path tablePath);

    public:
        
        static RelationStore NewGraphToFile(Path tablePath);
        void AddUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void AddUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses,const std::vector<int> &weight);
        void AddBid(const std::string &name, const std::vector<std::string> &vertexs);
        void AddBid(const std::string &name, const std::vector<std::string> &vertexs,int weight);
        void RemoveUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void RemoveBid(const std::string &name, const std::vector<std::string> &vertexs);

        void UndoAddUni(const std::string &name);
        void UndoAddBid(const std::string &name);
        void UndoRemoveUni(const std::string &name);
        void UndoRemoveBid(const std::string &name);

        static RelationStore ReadGraph(Path tablePath);
        Graph GetGraph();
        Graph GetGraphWithoutIsolatedNode();

        void SaveGraphTo(Path tablePath);
        void SaveGraph();
    };

}