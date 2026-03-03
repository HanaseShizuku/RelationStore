#include "include/syntax_parse.h"
#include "include/syntax_nodes.h"
#include <span>
#include <concepts>
#include <memory>
#include <string>

namespace RelationStoreLib::Syntax
{

    void AddUniNode::Init(const std::span<std::string> &sa, const std::span<std::string> &fa)
    {
        _Optype = OpType::Add;
        _GraphType = GraphType::Uni;
        _ArgPack = GetArgPack<GraphType::Uni>(sa, fa);
    }
    std::string AddUniNode::ToString()
    {
        return SyntaxParser::ArgPackToScriptArg<GraphType::Uni>(_ArgPack, OpName, RelationName);
    }
    void AddBidNode::Init(const std::span<std::string> &sa, const std::span<std::string> &fa)
    {
        _Optype = OpType::Add;
        _GraphType = GraphType::Bid;
        _ArgPack = GetArgPack<GraphType::Bid>(sa, fa);
    }
    std::string AddBidNode::ToString()
    {
        return SyntaxParser::ArgPackToScriptArg<GraphType::Bid>(_ArgPack, OpName, RelationName);
    }
    void RemUniNode::Init(const std::span<std::string> &sa, const std::span<std::string> &fa)
    {
        _Optype = OpType::Rem;
        _GraphType = GraphType::Uni;
        _ArgPack = GetArgPack<GraphType::Uni>(sa, fa);
    }
    std::string RemUniNode::ToString()
    {
        return SyntaxParser::ArgPackToScriptArg<GraphType::Uni>(_ArgPack, OpName, RelationName);
    }
    void RemBidNode::Init(const std::span<std::string> &sa, const std::span<std::string> &fa)
    {
        _Optype = OpType::Rem;
        _GraphType = GraphType::Bid;
        _ArgPack = GetArgPack<GraphType::Bid>(sa, fa);
    }
    std::string RemBidNode::ToString()
    {
        return SyntaxParser::ArgPackToScriptArg<GraphType::Bid>(_ArgPack, OpName, RelationName);
    }
    DoConnectionOpArgPack GeneralSyntaxNode::GetArgs()
    {
        return _ArgPack;
    }
    OpType GeneralSyntaxNode::GetOpType(){
        return _Optype;
    }
    GraphType GeneralSyntaxNode::GetGraphType(){
        return _GraphType;
    }
} // namespace RelationStoreLib
