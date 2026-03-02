#include <iostream>
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <span>
#include <ranges>
#include <queue>
#include "file.hpp"
#include "string.hpp"
#include "bfs_result.h"
#include "bfs_path.h"

namespace BFSPathLib
{
    using namespace std;
    using namespace shizuku::io;
    using namespace shizuku::util::string;
    namespace fs = std::filesystem;
    using Graph = map<string, set<string>>;
    using Path = fs::path;

    void BFSPath::_SetName(const string &name)
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
    void BFSPath::_AddUniConnection(const string &beginPos, span<const string> endPoses)
    {
        // 行内除连接方式标识符以外的所有元素与第一个元素单向连接 方向:第一个元素->别的元素
        for (const auto &endPos : endPoses)
        {
            _graph[beginPos].insert(endPos);
        }
    }
    void BFSPath::_AddBidConnection(span<const string> vertexs)
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
    void BFSPath::_RemoveUniConnection(const string &beginPos, span<const string> endPoses)
    {
        // 删除行内除连接方式标识符以外的所有元素与第一个元素的单向连接 删除方向:第一个元素->别的元素
        for (const auto &endPos : endPoses)
        {
            _graph[beginPos].erase(endPos);
        }
    }
    void BFSPath::_RemoveBidConnection(span<const string> vertexs)
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

    void BFSPath::_AddUniConnection(const DoConnectionOpArgPack &arg)
    {
        _AddUniConnection(shizuku::util::string::view::FromView(arg.UniBeginPos), arg.UniEndPoses);
    }

    void BFSPath::_AddBidConnection(const DoConnectionOpArgPack &arg)
    {
        _AddBidConnection(arg.BidVertexs);
    }

    void BFSPath::_RemoveUniConnection(const DoConnectionOpArgPack &arg)
    {
        _RemoveUniConnection(shizuku::util::string::view::FromView(arg.UniBeginPos), arg.UniEndPoses);
    }

    void BFSPath::_RemoveBidConnection(const DoConnectionOpArgPack &arg)
    {
        _RemoveBidConnection(arg.BidVertexs);
    }

    void BFSPath::_AddRelationshipText(const std::string &fullName, const std::vector<std::string> &fullVector)
    {
        _graphText.push_back(RelationshipLine{
            .name = fullName,
            .content = shizuku::util::string::Join(fullVector, " ")});
    }

    void BFSPath::_SetTextsToVector()
    {
        vector<string> lines = File::ReadAllLines(_tablePath);
        for (const string &line : lines)
        {
            string name = string(shizuku::util::string::view::Split(line, ' ')[0]);
            _SetName(name);
            _graphText.push_back(RelationshipLine{
                .name = name,
                .content = line});
        }
    }

    BFSPath::BFSPath(Path tablePath) : _tablePath(move(tablePath))
    {
        if (!File::Exists(_tablePath))
        {
            throw std::runtime_error("File not exist: " + _tablePath.string());
        }
        _isInstance = true;
    }
    BFSPath BFSPath::NewGraphToFile(Path tablePath)
    {
        vector<uint8_t> x;
        File::WriteAllBytes(tablePath, x);
        return BFSPath(tablePath);
    }
    void BFSPath::AddUni(const string &name, const string &beginPos, const vector<string> &endPoses)
    {
        _MakeRelationship<OpType::Add, GraphType::Uni>(name, UniArgPack{.BeginPos = beginPos, .EndPoses = endPoses});
    }
    void BFSPath::AddBid(const string &name, const vector<string> &vertexs)
    {
        _MakeRelationship<OpType::Add, GraphType::Bid>(name, BidArgPack{.Poses = vertexs});
    }
    void BFSPath::RemoveUni(const string &name, const string &beginPos, const vector<string> &endPoses)
    {
        _MakeRelationship<OpType::Rem, GraphType::Uni>(name, UniArgPack{.BeginPos = beginPos, .EndPoses = endPoses});
    }
    void BFSPath::RemoveBid(const string &name, const vector<string> &vertexs)
    {
        _MakeRelationship<OpType::Rem, GraphType::Bid>(name, BidArgPack{.Poses = vertexs});
    }

    void BFSPath::UndoAddUni(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Add, GraphType::Uni>(name);
    }

    void BFSPath::UndoAddBid(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Add, GraphType::Bid>(name);
    }

    void BFSPath::UndoRemoveUni(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Rem, GraphType::Uni>(name);
    }

    void BFSPath::UndoRemoveBid(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Rem, GraphType::Bid>(name);
    }

    bool BFSPath::ReadGraph()
    {
        _SetTextsToVector();
        for (const auto &s : _graphText)
        {
            auto lineFull = Split(s.content, ' ');
            if (lineFull.size() < 3)
            {
                throw std::runtime_error("Each line shall contain at least 2 elements excluding the connection mode identifier: " + s.content);
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
                throw std::runtime_error("Syntax error in connecting graph elements: " + s.content);
            }
        }
        return true;
    }
    Graph BFSPath::GetGraph()
    {
        return _graph;
    }
    Graph BFSPath::GetGraphWithoutIsolatedNode()
    {
        auto returnGraph = _graph | std::views::filter([](const auto &kv)
                                                       { return !kv.second.empty(); });
        return std::ranges::to<Graph>(returnGraph);
    }

    void BFSPath::SaveGraphTo(Path tablePath)
    {
        auto savedText = _graphText | std::views::transform([](const RelationshipLine &rl)
                                                            { return rl.content; }) |
                         std::ranges::to<vector<string>>();
        File::WriteAllText(tablePath, Join(savedText, "\n"));
    }
    void BFSPath::SaveGraph()
    {
        SaveGraphTo(_tablePath);
    }

    BFSResult BFSPath::Traverse(const string &src)
    {
        queue<string> tovisit;
        std::map<string, NodeInfo> result;
        tovisit.push(src);
        result[src] = {.parent = "", .hops = 0};

        while (!tovisit.empty())
        {
            string current = tovisit.front();
            tovisit.pop();

            if (!_graph.contains(current))
                continue;

            for (const auto &nb : _graph.at(current))
            {
                if (!result.contains(nb))
                {
                    result[nb] = {
                        .parent = current,
                        .hops = result[current].hops + 1};
                    tovisit.push(nb);
                }
            }
        }
        return BFSResult(result, src);
    }

    BFSPath::DoConnectionOpArgPack::DoConnectionOpArgPack(const std::string_view &uniBeginPos, const std::span<const std::string> &uniEndPoses, const std::span<const std::string> &BidVertexs) : UniBeginPos(uniBeginPos),
                                                                                                                                                                                                  UniEndPoses(uniEndPoses),
                                                                                                                                                                                                  BidVertexs(BidVertexs) {}

    BFSPath::DoConnectionOpArgPack::DoConnectionOpArgPack(const UniArgPack &p) : UniBeginPos(p.BeginPos), UniEndPoses(p.EndPoses) {}
    BFSPath::DoConnectionOpArgPack::DoConnectionOpArgPack(const BidArgPack &p) : BidVertexs(p.Poses) {}
    template <BFSPath::OpType optype, BFSPath::GraphType graphtype>
    inline constexpr std::string_view BFSPath::_GetPrefixByTemplate()
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
    void BFSPath::_AddRelationshipText(const std::string &name, const std::vector<std::string> &endposes, const std::string &beginPos)
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
    template <BFSPath::OpType optype, BFSPath::GraphType graphType>
    void BFSPath::_FuncHandler(const DoConnectionOpArgPack &arg)
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
    void BFSPath::_MakeRelationship(const std::string &relationName, std::conditional_t<graphtype == GraphType::Uni, UniArgPack, BidArgPack> graphArg)
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
    void BFSPath::_RemoveByNameAndType(const std::string &relationName)
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
}
