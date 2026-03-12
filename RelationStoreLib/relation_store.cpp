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
#include "utils/file.hpp"
#include "utils/string.hpp"
#include "include/relation_store.h"
#include "include/syntax_nodes.h"
#include "include/syntax_parse.h"

namespace RelationStoreLib
{
    using namespace std;
    using namespace shizuku::io;
    using namespace shizuku::util::string;
    using namespace Syntax;
    namespace fs = std::filesystem;
    using Graph = map<string, set<string>>;
    using Path = fs::path;

    void RelationStore::_SetName(const string &name)
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
    void RelationStore::_AddUniConnection(const string &beginPos, span<const string> endPoses)
    {
        // 行内除连接方式标识符以外的所有元素与第一个元素单向连接 方向:第一个元素->别的元素
        for (const auto &endPos : endPoses)
        {
            _graph[beginPos].insert(endPos);
        }
    }
    void RelationStore::_AddBidConnection(span<const string> vertexs)
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
    void RelationStore::_RemoveUniConnection(const string &beginPos, span<const string> endPoses)
    {
        // 删除行内除连接方式标识符以外的所有元素与第一个元素的单向连接 删除方向:第一个元素->别的元素
        for (const auto &endPos : endPoses)
        {
            _graph[beginPos].erase(endPos);
        }
    }
    void RelationStore::_RemoveBidConnection(span<const string> vertexs)
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

    void RelationStore::_AddUniConnection(const DoConnectionOpArgPack &arg)
    {
        _AddUniConnection(shizuku::util::string::view::FromView(arg.UniBeginPos), arg.UniEndPoses);
    }

    void RelationStore::_AddBidConnection(const DoConnectionOpArgPack &arg)
    {
        _AddBidConnection(arg.BidVertexs);
    }

    void RelationStore::_RemoveUniConnection(const DoConnectionOpArgPack &arg)
    {
        _RemoveUniConnection(shizuku::util::string::view::FromView(arg.UniBeginPos), arg.UniEndPoses);
    }

    void RelationStore::_RemoveBidConnection(const DoConnectionOpArgPack &arg)
    {
        _RemoveBidConnection(arg.BidVertexs);
    }


    void RelationStore::_SetTextsToVector()
    {
        std::string lines = File::ReadAllText(_tablePath);
        lines=Replace(lines,"\n","");
        std::vector<std::unique_ptr<GeneralSyntaxNode>> nodes=SyntaxParser::Parse(lines);
        for (auto &line : nodes)
        {
            string name = line->RelationName;
            _SetName(name);
            _graphText.push_back(std::move(line));
        }
    }

    RelationStore::RelationStore(Path tablePath) : _tablePath(move(tablePath))
    {
        if (!File::Exists(_tablePath))
        {
            throw std::runtime_error("File not exist: " + _tablePath.string());
        }
        _isInstance = true;
    }
    RelationStore RelationStore::NewGraphToFile(Path tablePath)
    {
        vector<uint8_t> x;
        File::WriteAllBytes(tablePath, x);
        return RelationStore(tablePath);
    }
    void RelationStore::AddUni(const string &name, const string &beginPos, const vector<string> &endPoses)
    {
        _MakeRelationship<OpType::Add, GraphType::Uni>(name, UniArgPack{.BeginPos = beginPos, .EndPoses = endPoses});
    }
    void RelationStore::AddBid(const string &name, const vector<string> &vertexs)
    {
        _MakeRelationship<OpType::Add, GraphType::Bid>(name, BidArgPack{.Poses = vertexs});
    }
    void RelationStore::RemoveUni(const string &name, const string &beginPos, const vector<string> &endPoses)
    {
        _MakeRelationship<OpType::Rem, GraphType::Uni>(name, UniArgPack{.BeginPos = beginPos, .EndPoses = endPoses});
    }
    void RelationStore::RemoveBid(const string &name, const vector<string> &vertexs)
    {
        _MakeRelationship<OpType::Rem, GraphType::Bid>(name, BidArgPack{.Poses = vertexs});
    }

    void RelationStore::UndoAddUni(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Add, GraphType::Uni>(name);
    }

    void RelationStore::UndoAddBid(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Add, GraphType::Bid>(name);
    }

    void RelationStore::UndoRemoveUni(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Rem, GraphType::Uni>(name);
    }

    void RelationStore::UndoRemoveBid(const std::string &name)
    {
        _RemoveByNameAndType<OpType::Rem, GraphType::Bid>(name);
    }

    bool RelationStore::ReadGraph()
    {
        _SetTextsToVector();
        for (const auto &s : _graphText)
        {
            auto arg = s->GetArgs();
            auto optype = s->GetOpType();
            auto graphtype = s->GetGraphType();
            if (optype == OpType::Add)
            {
                if (graphtype == GraphType::Uni)
                {
                    _FuncHandler<OpType::Add, GraphType::Uni>(arg);
                }
                else
                {
                    _FuncHandler<OpType::Add, GraphType::Bid>(arg);
                }
            }
            else
            {
                if (graphtype == GraphType::Uni)
                {
                    _FuncHandler<OpType::Rem, GraphType::Uni>(arg);
                }
                else
                {
                    _FuncHandler<OpType::Rem, GraphType::Bid>(arg);
                }
            }
        }
        return true;
    }
    Graph RelationStore::GetGraph()
    {
        return _graph;
    }
    Graph RelationStore::GetGraphWithoutIsolatedNode()
    {
        auto returnGraph = _graph | std::views::filter([](const auto &kv)
                                                       { return !kv.second.empty(); });
        return std::ranges::to<Graph>(returnGraph);
    }

    void RelationStore::SaveGraphTo(Path tablePath)
    {
        
        std::vector<string> s;
        for(const auto &x:_graphText){
            s.push_back(x->ToString());
        }
        std::string result=shizuku::util::string::Join(s, "\n");
        File::WriteAllText(tablePath, result);
    }
    void RelationStore::SaveGraph()
    {
        SaveGraphTo(_tablePath);
    }

    template <OpType optype, GraphType graphtype>
    inline constexpr std::string_view RelationStore::_GetPrefixByTemplate()
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

    
    template <OpType optype, GraphType graphType>
    void RelationStore::_FuncHandler(const DoConnectionOpArgPack &arg)
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
    template <OpType optype, GraphType graphtype>
    void RelationStore::_MakeRelationship(const std::string &relationName, std::conditional_t<graphtype == GraphType::Uni, UniArgPack, BidArgPack> graphArg)
    {
        std::string fullName = relationName;
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
        std::unique_ptr<GeneralSyntaxNode> node;
        
        
        if constexpr(optype==OpType::Add){
            if constexpr(graphtype==GraphType::Uni){
                node=std::make_unique<AddUniNode>(graphArg,fullName);
            }else{
                node=std::make_unique<AddBidNode>(graphArg,fullName);
            }
        }else{
            if constexpr(graphtype==GraphType::Uni){
                node=std::make_unique<RemUniNode>(graphArg,fullName);
            }else{
                node=std::make_unique<RemBidNode>(graphArg,fullName);
            }
        }
        node->RelationName=relationName;
        _graphText.push_back(std::move(node));
    }
    template <OpType optype, GraphType graphtype>
    void RelationStore::_RemoveByNameAndType(const std::string &relationName)
    {
        std::string fullLine;
        std::string name = relationName;
        std::unique_ptr<RelationStoreLib::Syntax::GeneralSyntaxNode> nodeobj;
        std::vector<std::unique_ptr<RelationStoreLib::Syntax::GeneralSyntaxNode>>::iterator it;
        if (_names.contains(name))
        {
            _names.erase(name);
            it = std::find_if(_graphText.begin(), _graphText.end(), [&](const std::unique_ptr<GeneralSyntaxNode> &relation)
                                   { return relation->RelationName == name; });
            if (it != _graphText.end())
            {
                nodeobj = std::move(*it);
                _graphText.erase(it);
                
            }
        }
        else
        {
            throw std::runtime_error("Relationship does not exist: " + name);
        }
        //auto linetoken = Syntax::SyntaxParser::TokenParse(fullLine);
        
        std::vector<float> default_weights = {1.0f};
        //DoConnectionOpArgPack arg = DoConnectionOpArgPack(std::span(linetoken)[1], std::span(linetoken).subspan(2), std::span(linetoken).subspan(1), std::span(default_weights));
        DoConnectionOpArgPack arg=nodeobj->GetArgs();
        constexpr OpType trueOP = ((optype == OpType::Add) ? OpType::Rem : OpType::Add);
        
        _FuncHandler<trueOP, graphtype>(arg);
    }
}
