#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "bfs_result.h"
namespace BFSPathLib
{
    using namespace std;
    BFSResult::BFSResult(map<string, NodeInfo> resultMap,const string &begin):
    _resultMap(move(resultMap)),
    _begin(move(begin))
    {}
    std::vector<std::string> BFSResult::GetPath(const std::string &dest){
        vector<string> result;
        string current=dest;
        while (_resultMap.contains(current)&&_resultMap[current].parent!="")
        {
            result.push_back(current);
            current=_resultMap[current].parent;
            
        }
        result.push_back(current);
        std::reverse(result.begin(),result.end());
        if(current!=_begin){
            return vector<string>();
        }
        return result;
    } 
    int BFSResult::GetHopCount(const string &dest){
        if(_resultMap.contains(dest)){
            return GetPath(dest).size()-1;
        }
        return -1;//不可达
    }
    
}
