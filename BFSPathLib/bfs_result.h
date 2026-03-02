#pragma once
#include<string>
#include<map>
namespace BFSPathLib
{
    struct NodeInfo
    {
            std::string parent;
            int hops = -1;
    };
    class BFSResult{
        private:
            std::map<std::string, NodeInfo> _resultMap;
        public:
            BFSResult(std::map<std::string, NodeInfo> resultMap);
            std::vector<std::string> GetPath(const std::string &dest);
            int GetHopCount(const std::string &dest);
    };
}