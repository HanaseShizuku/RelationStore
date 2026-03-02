#include "bfs_path_lib_export.h"
#include <string>
#include <map>
#include <set>
#include <filesystem>
#include <vector>
#include <span>
#include "bfs_result.h"
#include "string.hpp"
#include <span>
namespace BFSPathLib
{

    class BFS_PATH_LIB_EXPORT BFSPath
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
            DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<const std::string> &uniEndPoses, const std::span<const std::string> &BidVertexs) : UniBeginPos(uniBeginPos),
                                                                                                                                                                          UniEndPoses(uniEndPoses),
                                                                                                                                                                          BidVertexs(BidVertexs) {}
            DoConnectionOpArgPack(const UniArgPack &p) : UniBeginPos(p.BeginPos), UniEndPoses(p.EndPoses) {}
            DoConnectionOpArgPack(const BidArgPack &p) : BidVertexs(p.Poses) {}
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
        static constexpr std::string_view _GetPrefixByTemplate()
        {
            if constexpr (optype == OpType::Add)
            {
                return (graphtype == GraphType::Uni) ? "+" : "*";
            }
            else
            {
                return (graphtype == GraphType::Uni) ? "-" : "/";
            }
        }
        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void _AddRelationshipText(const std::string &name, const std::vector<std::string> &endposes, const std::string &beginPos = "")
        {

            std::string fullName = _GetPrefixByTemplate<optype, graphtype>() + name;
            std::vector<std::string> line;
            line.reserve(endposes.size() + (graphtype == GraphType::Uni ? 2 : 1));
            if constexpr (graphtype == GraphType::Uni)
            {
                line = {fullName, beginPos};
            }
            else
            {
                line = {fullName};
            }

            line.insert(line.end(), endposes.begin(), endposes.end());
            _graphText.push_back(RelationshipLine{
                .name = fullName,
                .content = shizuku::util::string::Join(line, " ")});
        }
        void _AddRelationshipText(const std::string &fullName, const std::vector<std::string> &fullVector)
        {
            _graphText.push_back(RelationshipLine{
                .name = fullName,
                .content = shizuku::util::string::Join(fullVector, " ")});
        }
        void _SetTextsToVector();
        template <BFSPath::OpType optype, BFSPath::GraphType graphType>
        void _FuncHandler(const DoConnectionOpArgPack &arg)
        {
            if constexpr (optype == OpType::Add)
            {
                if constexpr (graphType == GraphType::Uni)
                {
                    _AddUniConnection(arg);
                }
                else
                {
                    _AddBidConnection(arg);
                }
            }
            else
            {
                if constexpr (graphType == GraphType::Uni)
                {
                    _RemoveUniConnection(arg);
                }
                else
                {
                    _RemoveBidConnection(arg);
                }
            }
        }
        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void _MakeRelationship(const std::string &relationName, std::conditional_t<graphtype == GraphType::Uni, UniArgPack, BidArgPack> graphArg)
        {
            constexpr auto prefix = _GetPrefixByTemplate<optype, graphtype>();
            std::string fullName = prefix + relationName;
            _SetName(fullName);
            _FuncHandler<optype, graphtype>(graphArg);
            std::vector<std::string> postelement;
            const int preReserveSize = [&]()
            {
                if constexpr (graphtype == GraphType::Uni)
                {
                    return 2 + graphArg.EndPoses.size();
                }
                else
                {
                    return 1 + graphArg.Poses.size();
                }
            }();
            postelement.reserve(preReserveSize);
            postelement.push_back(fullName);
            if constexpr (graphtype == GraphType::Uni)
            {
                postelement.push_back(std::string(graphArg.BeginPos));
                postelement.insert(postelement.end(), graphArg.EndPoses.begin(), graphArg.EndPoses.end());
            }
            else
            {
                postelement.insert(postelement.end(), graphArg.Poses.begin(), graphArg.Poses.end());
            }
            _AddRelationshipText(fullName, postelement);
        }

        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void _RemoveByNameAndType(const std::string &relationName)
        {
            std::string fullLine;
            std::string name = _GetPrefixByTemplate<optype, graphtype>() + relationName;
            if (_names.contains(name))
            {
                _names.erase(name);
                auto it = std::find_if(_graphText.begin(), _graphText.end(), [&](const RelationshipLine &relation)
                                       { return relation.name == name; });
                if (it != _graphText.end())
                {
                    fullLine = it->content;
                    _graphText.erase(it);
                }
            }
            else
            {
                throw std::runtime_error("Relationship does not exist: " + name);
            }
            auto linetoken = shizuku::util::string::Split(fullLine, ' ');
            DoConnectionOpArgPack arg = DoConnectionOpArgPack(std::span(linetoken)[1], std::span(linetoken).subspan(2), std::span(linetoken).subspan(1));
            constexpr OpType trueOP = ((optype == OpType::Add) ? OpType::Rem : OpType::Add);
            _FuncHandler<trueOP, graphtype>(arg);
        }

    public:
        BFSPath(Path tablePath);
        static BFSPath NewGraphToFile(Path tablePath);
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
        BFSResult Traverse(const std::string &src);
    };

}