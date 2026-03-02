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
        using Graph = std::map<std::string, std::set<std::string>>;
        using Path = std::filesystem::path;
        struct RelationshipLine
        {
            std::string name;
            std::string content;
        };
        
        struct DoConnectionOpArgPack{
            std::string_view UniBeginPos;
            std::span<const std::string> UniEndPoses;
            std::span<const std::string> BidVertexs;
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
        public:
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
        private:
        using MemberHandler=void (BFSPath::*)(const DoConnectionOpArgPack&);
        static constexpr MemberHandler HandlerTable[2][2] = {
        { &BFSPath::_AddUniConnection, &BFSPath::_AddBidConnection }, // Add 对应 [0][0] 和 [0][1]
        { &BFSPath::_RemoveUniConnection, &BFSPath::_RemoveBidConnection }  // Rem 对应 [1][0] 和 [1][1]
        };

        template <BFSPath::OpType optype, BFSPath::GraphType graphtype,bool opReverse>
        constexpr int static _GetFuncIndexByTemplate(){
            constexpr int base = (static_cast<int>(optype) << 1) | static_cast<int>(graphtype);
            return opReverse ? (base ^ 0b10) : base;
        }

        template <OpType optype, GraphType graphtype>
        static constexpr std::string _GetPrefixByTemplate() {
            if constexpr (optype == OpType::Add) {
                return (graphtype == GraphType::Uni) ? "+" : "*";
            } else {
                return (graphtype == GraphType::Uni) ? "-" : "/";
            }
        }
        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void _AddRelationshipText(const std::string &name, const std::vector<std::string> &endposes, const std::string &beginPos="")
        {
            
            std::string fullName = _GetPrefixByTemplate<optype,graphtype>() + name;
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
        void _SetTextsToVector();

    public:
        BFSPath(Path tablePath);
        static BFSPath NewGraphToFile(Path tablePath);
        void AddUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void AddBid(const std::string &name, const std::vector<std::string> &vertexs);
        void RemoveUni(const std::string &name, const std::string &beginPos, const std::vector<std::string> &endPoses);
        void RemoveBid(const std::string &name, const std::vector<std::string> &vertexs);
        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void RemoveByNameAndType(const std::string &relationName)
        {
            std::string fullLine;
            std::string name=_GetPrefixByTemplate<optype,graphtype>()+relationName;
            if (_names.contains(name))
            {
                _names.erase(name);
                auto it=std::find_if(_graphText.begin(),_graphText.end(),[&](const RelationshipLine &relation)
                         { 
                            return relation.name == name; });
                if(it!=_graphText.end()){
                    fullLine=it->content;
                    _graphText.erase(it);
                }
            }
            else
            {
                throw std::runtime_error("Relationship does not exist: " + name);
            }
            auto linetoken=shizuku::util::string::Split(fullLine,' ');
            int constexpr op=_GetFuncIndexByTemplate<optype,graphtype,true>();
            DoConnectionOpArgPack arg={std::span(linetoken)[1],std::span(linetoken).subspan(2),std::span(linetoken).subspan(1)};

            constexpr MemberHandler handler = HandlerTable[((op >> 1) & 0b01)][(op&0b01)];
            (this->*handler)(arg);
            
        }
        bool ReadGraph();
        Graph GetGraph();
        Graph GetGraphWithoutIsolatedNode();

        void SaveGraphTo(Path tablePath);
        void SaveGraph();
        BFSResult Traverse(const std::string &src);
        
    };

}