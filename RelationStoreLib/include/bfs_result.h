#pragma once
#include <vector>
#include <set>
#include <string>
#include <map>
#include "relation_store_lib_export.h"
#include "relation_store.h"
namespace RelationStoreLib::algorithm
{

    class RELATION_STORE_LIB_EXPORT BFSResult
    {
    private:
        struct NodeInfo
        {
            std::string parent;
            int hops = -1;
        };
        std::map<std::string, NodeInfo> _resultMap;
        std::string _begin;
        BFSResult(std::map<std::string, NodeInfo> resultMap, const std::string &begin);
    public:
        std::vector<std::string> GetPath(const std::string &dest);
        int GetHopCount(const std::string &dest);
        static BFSResult TraverseFromStore(const std::string &src, RelationStore &graph);
    };
}