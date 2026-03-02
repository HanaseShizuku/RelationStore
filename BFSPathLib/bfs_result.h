#pragma once
#include<string>
#include<map>
#include "bfs_path_lib_export.h"
namespace BFSPathLib
{
    struct NodeInfo
    {
            std::string parent;
            int hops = -1;
    };
    class BFS_PATH_LIB_EXPORT BFSResult{
        private:
            std::map<std::string, NodeInfo> _resultMap;
            std::string _begin;
        public:
            BFSResult(std::map<std::string, NodeInfo> resultMap,const std::string &begin);
            std::vector<std::string> GetPath(const std::string &dest);
            int GetHopCount(const std::string &dest);
    };
}