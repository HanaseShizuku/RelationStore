#pragma once
#include <span>
#include <concepts>
#include <memory>
#include <string>
#include "include/types.h"

namespace RelationStoreLib::Syntax
{
    class GeneralSyntaxNode
    {
    protected:
        DoConnectionOpArgPack _ArgPack;
        OpType _Optype;
        GraphType _GraphType;

    public:
        std::string OpName;
        std::string RelationName;
        virtual void Init(const std::span<std::string> &sa, const std::span<std::string> &fa) = 0;
        virtual std::string ToString() = 0;
        DoConnectionOpArgPack GetArgs();
        OpType GetOpType();
        GraphType GetGraphType();
    };

    template <typename T>
    concept IsSyntaxNode = std::is_base_of_v<GeneralSyntaxNode, T>;
    template <IsSyntaxNode T>
    std::unique_ptr<T> Create(std::span<std::string> sa, std::span<std::string> fa, const std::string &opname)
    {
        std::vector<std::string> sa_filtered, fa_filtered;
        for (auto &s : sa)
            if (s != ",")
                sa_filtered.push_back(s);
        for (auto &f : fa)
            if (f != ",")
                fa_filtered.push_back(f);
        std::unique_ptr<T> obj = std::make_unique<T>();
        // obj->RelationName = sa_filtered[0];
        // obj->OpName = opname;
        obj->Init(sa_filtered, fa_filtered);
        return obj;
    }
    template <GraphType graph>
    DoConnectionOpArgPack GetArgPack(const std::span<std::string> sa, const std::span<std::string> fa)
    {
        std::vector<int> fs;
        for (const auto &f : fa)
        {
            try{
                fs.push_back(std::stof(f));
            }catch(const std::exception& e){
                throw std::runtime_error("'f' is not a int,f:"+f);
            }
            
        }
        if constexpr (graph == GraphType::Uni)
        {
            auto x = sa.subspan(2);
            std::vector<std::string> pEnd = std::vector(x.begin(), x.end());
            return DoConnectionOpArgPack(UniArgPack{.BeginPos = sa[1], .EndPoses = pEnd, .Weights = fs});
        }
        else
        {
            auto x = sa.subspan(1);
            std::vector<std::string> pEnd = std::vector(x.begin(), x.end());
            return DoConnectionOpArgPack(BidArgPack{.Poses = pEnd, .Weight = fs});
        }
    }
    class AddUniNode : public GeneralSyntaxNode
    {
    public:
        void Init(const std::span<std::string> &sa, const std::span<std::string> &fa) override;
        std::string ToString() override;
        AddUniNode(const DoConnectionOpArgPack &arg,const std::string &relationName);
        AddUniNode()=default;
    };
    class AddBidNode : public GeneralSyntaxNode
    {
    public:
        void Init(const std::span<std::string> &sa, const std::span<std::string> &fa) override;
        std::string ToString() override;
        AddBidNode(const DoConnectionOpArgPack &arg,const std::string &relationName);
        AddBidNode()=default;
    };

    class RemUniNode : public GeneralSyntaxNode
    {
    public:
        void Init(const std::span<std::string> &sa, const std::span<std::string> &fa) override;
        std::string ToString() override;
        RemUniNode(const DoConnectionOpArgPack &arg,const std::string &relationName);
        RemUniNode()=default;
    };
    class RemBidNode : public GeneralSyntaxNode
    {
    public:
        void Init(const std::span<std::string> &sa, const std::span<std::string> &fa) override;
        std::string ToString() override;
         RemBidNode(const DoConnectionOpArgPack &arg,const std::string &relationName);
         RemBidNode()=default;
    };

} // namespace RelationStoreLib
