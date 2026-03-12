#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include "include/bfs_result.h"
#include "include/relation_store.h"
namespace RelationStoreLib::algorithm
{
    BFSResult::BFSResult(std::map<std::string, NodeInfo> resultMap,const std::string &begin):
    _resultMap(move(resultMap)),
    _begin(move(begin))
    {}
    std::vector<std::string> BFSResult::GetPath(const std::string &dest){
        std::vector<std::string> result;
        std::string current=dest;
        while (_resultMap.contains(current)&&_resultMap[current].parent!="")
        {
            result.push_back(current);
            current=_resultMap[current].parent;
            
        }
        result.push_back(current);
        std::reverse(result.begin(),result.end());
        if(current!=_begin){
            return std::vector<std::string>();
        }
        return result;
    } 
    int BFSResult::GetHopCount(const std::string &dest){
        if(_resultMap.contains(dest)){
            return GetPath(dest).size()-1;
        }
        return -1;//不可达
    }
    BFSResult BFSResult::TraverseFromStore(const std::string &src,RelationStore &graph)
    {
        auto _graph=graph.GetGraph();
        std::queue<std::string> tovisit;
        std::map<std::string, NodeInfo> result;
        tovisit.push(src);
        result[src] = {.parent = "", .hops = 0};

        while (!tovisit.empty())
        {
            std::string current = tovisit.front();
            tovisit.pop();

            if (!_graph.contains(current))
                continue;
            for (const auto &nb : _graph.at(current))
            {
                if (!result.contains(nb.AdjacentNodeName))
                {
                    result[nb.AdjacentNodeName] = {
                        .parent = current,
                        .hops = result[current].hops + 1};
                    tovisit.push(nb.AdjacentNodeName);
                }
            }
        }
        return BFSResult(result, src);
    }
    
}
