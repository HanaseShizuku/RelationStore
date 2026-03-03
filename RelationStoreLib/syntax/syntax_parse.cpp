#include <string>
#include <vector>
#include "utils/string.hpp"
#include <span>
#include <memory>
#include <concepts>
#include "include/syntax_parse.h"
#include <format>
#include <ranges>
#include "include/syntax_nodes.h"
namespace RelationStoreLib::Syntax
{
    using namespace shizuku::util::string;
    /*
    语法
    Uni(Name,begin,end1,end2)[weight1,weight2]; 有向
    Bid(Name,P1,P2,P3,P4,...)[same_weight]; 全连接/无向
    RmUni(Name,begin,end)[]
    RmBid(Name,P1,P2)[]
    */

    DoConnectionOpArgPack::DoConnectionOpArgPack(
        const std::string_view &uniBeginPos,
        const std::span<std::string> &uniEndPoses,
        const std::span<std::string> &BidVertexs,
        const std::span<float> &weights)
        : UniBeginPos(uniBeginPos),
          UniEndPoses(std::vector(uniEndPoses.begin(), uniEndPoses.end())),
          BidVertexs(std::vector(BidVertexs.begin(), BidVertexs.end())),
          Weights(std::vector(weights.begin(), weights.end())) {}

    DoConnectionOpArgPack::DoConnectionOpArgPack(const UniArgPack &p) : UniBeginPos(p.BeginPos), UniEndPoses(p.EndPoses), Weights(p.Weights) {}
    DoConnectionOpArgPack::DoConnectionOpArgPack(const BidArgPack &p) : BidVertexs(p.Poses), Weights(p.Weight) {}

    std::unique_ptr<GeneralSyntaxNode> TokenToSyntaxNode(std::span<std::string> span)
    {
        auto opName = span[0];
        auto sab = std::find(span.begin(), span.end(), "(");
        auto sae = std::find(span.begin(), span.end(), ")");
        if (sab > sae)
        {
            throw std::runtime_error("syntax error");
        }
        auto _StringArr = std::span(&(*(sab + 1)), &(*sae));
        auto fab = std::find(span.begin(), span.end(), "[");
        auto fae = std::find(span.begin(), span.end(), "]");
        if (fab > fae)
        {
            throw std::runtime_error("syntax error");
        }
        auto _FloatArr = std::span(&(*(fab + 1)), &(*fae));
        if (opName == "Uni")
        {
            return Create<AddUniNode>(std::span(_StringArr), std::span(_FloatArr), opName);
        }
        else if (opName == "Bid")
        {
            return Create<AddBidNode>(std::span(_StringArr), std::span(_FloatArr), opName);
        }
        else if (opName == "RemUni")
        {
            return Create<RemUniNode>(std::span(_StringArr), std::span(_FloatArr), opName);
        }
        else if (opName == "RemBid")
        {
            return Create<RemBidNode>(std::span(_StringArr), std::span(_FloatArr), opName);
        }
        else
        {
            throw std::runtime_error("syntax error");
        }
    }
    std::vector<std::unique_ptr<GeneralSyntaxNode>> TokensToSytaxNodes(const std::vector<std::span<std::string>> &nodes)
    {
        std::vector<std::unique_ptr<GeneralSyntaxNode>> r;
        for (const auto &n : nodes)
        {
            r.push_back(TokenToSyntaxNode(n));
        }
        return r;
    }
    std::vector<std::unique_ptr<GeneralSyntaxNode>> RelationStoreLib::Syntax::SyntaxParser::Parse(std::string strs)
    {
        std::vector<std::string> tokens = SyntaxParser::TokenParse(strs);
        const std::vector<std::span<std::string>> nodes = SyntaxParser::SplitVectorToSpans<std::string>(tokens, ";");
        return TokensToSytaxNodes(nodes);
        // Uni ( 名字 , 起点 , 终点 ) ;
        // UniWeight ( 带引号的名字(") , 带转义符号的起点\ , 终点 ) [ weight ] ;
    }
    bool RelationStoreLib::Syntax::SyntaxParser::IsSingleToken(const char &c)

    {
        constexpr char car[] = {'(', ')', '[', ']', ',', ';', '\n', '\r'};
        for (const char &x : car)
        {
            if (c == x)
            {
                return true;
            }
        }
        return false;
    }

    std::string RelationStoreLib::Syntax::SyntaxParser::EraseEscape(const std::string &str)
    {
        return Replace(Replace(str, "\\\\", "\\"), "\\\"", "\"");
    }

    std::vector<std::string> RelationStoreLib::Syntax::SyntaxParser::TokenParse(std::string strs)
    {
        int b = 0, i = 0;
        bool quote = false, escape = false;
        std::vector<std::string> tokens;
        auto addtoken = [&](const std::string &t)
        {
            std::string cleaned = Trim(t);
            if (!cleaned.empty())
                tokens.push_back(cleaned);
        };

        while (i < strs.size())
        {
            if (quote)
            {
                if (strs[i] == '"' && !escape)
                {
                    addtoken(EraseEscape(strs.substr(b + 1, i - b - 1)));
                    b = i + 1;
                    quote = false;
                }
                else if (strs[i] == '\\' && !escape)
                {
                    escape = true;
                }
                else
                {
                    escape = false;
                }
            }
            else
            {
                if (strs[i] == '"')
                {
                    addtoken(strs.substr(b, i - b));
                    b = i;
                    quote = true;
                }
                else if (IsSingleToken(strs[i]) || std::isspace(strs[i]))
                {
                    addtoken(strs.substr(b, i - b));
                    if (IsSingleToken(strs[i]))
                    {
                        tokens.push_back(std::string(1, strs[i]));
                    }
                    b = i + 1;
                }
            }
            i++;
        }
        addtoken(strs.substr(b));
        return tokens;
    }
    template <typename T>
    std::vector<std::span<T>> SyntaxParser::SplitVectorToSpans(std::vector<T> &vec, const T &delimiter)
    {
        std::vector<std::span<T>> result;
        if (vec.empty())
        {
            return result;
        }
        T *start = vec.data();
        T *current = vec.data();
        T *end = vec.data() + vec.size();
        while (current < end)
        {
            if (*current == delimiter)
            {
                if (start != current)
                {
                    result.emplace_back(start, current - start);
                }
                start = current + 1;
            }
            current++;
        }
        if (start < end)
        {
            result.emplace_back(start, end - start);
        }
        return result;
    }
    std::string Join(std::vector<std::string> v, std::string s)
    {
        return shizuku::util::string::Join(v, s);
    }

}
