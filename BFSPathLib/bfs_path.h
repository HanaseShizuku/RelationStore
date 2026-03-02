#include "bfs_path_lib_export.h"
#include <string>
#include <map>
#include <set>
#include <filesystem>
#include <vector>
#include <span>
#include "bfs_result.h"
#include "string.hpp"
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
        template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
        void _AddRelationshipText(const std::string &name, const std::vector<std::string> &endposes, const std::string &beginPos="")
        {
            constexpr int ot = (optype == OpType::Add) ? 0 : 1;
            constexpr int og = (graphtype == GraphType::Uni) ? 0 : 1;
            constexpr int op_idx = (ot << 1) | og;

            // 0    1    2    3     Oct
            // 00   01   10   11    Bin
            // AU   AB   RU   RB    Optype+GraphType

            // +    -    *     /    前缀
            // AU   RU   AB   RB    OpType+GraphType
            // 0    2    1    3     Index

            const char *symbols[] = {"+", "*", "-", "/"};
            std::string fullName = symbols[op_idx] + name;
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
        void RemoveByNameAndType(const std::string &name);
        bool ReadGraph();
        Graph GetGraph();
        Graph GetGraphWithoutIsolatedNode();

        void SaveGraphTo(Path tablePath);
        void SaveGraph();
        BFSResult Traverse(const std::string &src);
    };

}