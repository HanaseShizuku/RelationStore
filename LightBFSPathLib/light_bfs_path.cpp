#include <iostream>
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <span>
#include "file.hpp"
#include "string.hpp"

namespace LightBFSPathLib
{
    using namespace std;
    using namespace shizuku::io;
    using namespace shizuku::util::string;
    namespace fs = std::filesystem;
    using Graph = map<string, set<string>>;
    using Path = fs::path;

    class LightBFSPath
    {
    private:
        Graph _graph;
        Path _tablePath;
        vector<string> _graphText;
        set<string> _names;
        bool _isInstance = false;
        void _SetName(const string &name)
        {
            if (!_names.contains(name))
            {
                _names.insert(name);
            }
            else
            {
                throw std::runtime_error("Connection relationship names must be unique: " + name);
            }
        }
        void _AddUniConnection(const string &beginPos, span<const string> endPoses)
        {
            // 行内除连接方式标识符以外的所有元素与第一个元素单向连接 方向:第一个元素->别的元素
            for (const auto &endPos : endPoses)
            {
                _graph[beginPos].insert(endPos);
            }
        }
        void _AddBidConnection(span<const string> vertexs)
        {
            // 行内除连接方式标识符以外的元素全连接
            for (size_t i = 0; i < vertexs.size(); ++i)
            {
                for (size_t j = i + 1; j < vertexs.size(); ++j)
                {
                    const auto &a = vertexs[i];
                    const auto &b = vertexs[j];
                    _graph[a].insert(b);
                    _graph[b].insert(a);
                }
            }
        }
        void _RemoveUniConnection(const string &beginPos, span<const string> endPoses)
        {
            // 删除行内除连接方式标识符以外的所有元素与第一个元素的单向连接 删除方向:第一个元素->别的元素
            for (const auto &endPos : endPoses)
            {
                _graph[beginPos].erase(endPos);
            }
        }
        void _RemoveBidConnection(span<const string> vertexs)
        {
            // 删除行内除连接方式标识符以外的所有元素的互相所有连接关系
            for (size_t i = 0; i < vertexs.size(); ++i)
            {
                for (size_t j = i + 1; j < vertexs.size(); ++j)
                {
                    const auto &a = vertexs[i];
                    const auto &b = vertexs[j];
                    auto itA = _graph.find(a);
                    if (itA != _graph.end())
                    {
                        itA->second.erase(b);
                    }
                    auto itB = _graph.find(b);
                    if (itB != _graph.end())
                    {
                        itB->second.erase(a);
                    }
                }
            }
        }
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
        void _AddRelationshipText(const string &name, const vector<string> &endposes, const string &beginPos="")
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
            string fullName = symbols[op_idx] + name;
            vector<string> line;
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
            _graphText.push_back(Join(line, " "));
        }

    public:
        LightBFSPath(Path tablePath) : _tablePath(move(tablePath))
        {
            if (!File::Exists(_tablePath))
            {
                throw std::runtime_error("File not exist: " + _tablePath.string());
            }
            _isInstance = true;
        }
        void AddUni(const string &name, const string &beginPos, const vector<string> &endPoses)
        {
            string fullName = "+" + name;
            _SetName(fullName);
            _AddUniConnection(beginPos, span(endPoses));
            _AddRelationshipText<OpType::Add,GraphType::Uni>(name,endPoses,beginPos);
        }
        void AddBid(const string &name, const vector<string> &vertexs)
        {
            string fullName = "*" + name;
            _SetName(fullName);
            _AddBidConnection(span(vertexs));
            _AddRelationshipText<OpType::Add,GraphType::Bid>(name,vertexs);
        }
        void RemoveUni(const string &name, const string &beginPos, const vector<string> &endPoses) {
            string fullName = "-" + name;
            _SetName(fullName);
            _RemoveUniConnection(beginPos, span(endPoses));
            _AddRelationshipText<OpType::Rem,GraphType::Uni>(name,endPoses,beginPos);
        }
        void RemoveBid(const string &name, const vector<string> &vertexs) {
             string fullName = "/" + name;
            _SetName(fullName);
            _RemoveBidConnection(span(vertexs));
            _AddRelationshipText<OpType::Rem,GraphType::Bid>(name,vertexs);
        }
        
        bool ReadGraph()
        {
            _graphText = File::ReadAllLines(_tablePath);
            for (const auto &s : _graphText)
            {
                auto lineFull = Split(s, ' ');
                if (lineFull.size() < 3)
                {
                    throw std::runtime_error("Each line shall contain at least 2 elements excluding the connection mode identifier: " + s);
                }
                auto lineElement = span(lineFull).subspan(1);
                if (StartsWith(lineFull[0], "*"))
                {
                    _AddBidConnection(lineElement);
                }
                else if (StartsWith(lineFull[0], "+"))
                {
                    _AddUniConnection(lineElement[0], span(lineElement).subspan(1));
                }
                else if (StartsWith(lineFull[0], "-"))
                {
                    _RemoveUniConnection(lineElement[0], span(lineElement).subspan(1));
                }
                else if (StartsWith(lineFull[0], "/"))
                {
                    _RemoveBidConnection(lineElement);
                }
                else
                {
                    throw std::runtime_error("Syntax error in connecting graph elements: " + s);
                }
                _SetName(lineFull[0]);
            }
            return true;
        }
    };

}
