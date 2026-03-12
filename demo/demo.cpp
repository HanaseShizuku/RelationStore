#include "include/relation_store.h"
#include "include/bfs_result.h"
#include<iostream>
#include<format>

using namespace RelationStoreLib;
using namespace std;

unique_ptr<RelationStore> relationStorePtr;
int GetPointDirectConnectWeight(string pos1,string pos2){
    Graph graph=relationStorePtr->GetGraph();
    if(graph[pos1].contains(pos2)){
        for(const RelationStoreLib::AdjNode& x:graph[pos1]){
            if(x.AdjacentNodeName==pos2){
                return x.Weight;
            }
        }
    }
    return -1;
}
string GetRelationResult(string pos1,string pos2){
    auto bfsresult=RelationStoreLib::algorithm::BFSResult::TraverseFromStore(pos1,*relationStorePtr);
    return format(
            "\t\t{}->{}\n\t\t\tPath:{}(Hop Count:{})\n\t\t\tDirectWeight:{}",
            pos1,
            pos2,
            bfsresult.GetPath(pos2),
            bfsresult.GetHopCount(pos2),
            GetPointDirectConnectWeight(pos1,pos2));
}
void ShowRelation(string testName,string pos1,string pos2){
    auto x=format("{} to {}",pos1,pos2);
    std::cout<<format("{}\n\t{}(+):\n{}\n\t{}(-):\n{}",testName,x,GetRelationResult(pos1,pos2),x,GetRelationResult(pos2,pos1))<<endl;
}

void RelationAddTest(){
    relationStorePtr->AddUni("DefaultWeightUni","DWUni.begin",{"DWUni.end1","DWUni.end2"});
    ShowRelation("Default Weight Uni Test","DWUni.begin","DWUni.end1");
    relationStorePtr->AddUni("CustomWeightUniPo","CWUni.P1",{"CWUni.P2"},{9});
    relationStorePtr->AddUni("CustomWeightUniNe","CWUni.P2",{"CWUni.P1"},{1});
    ShowRelation("Custom Weight Uni Test","CWUni.P1","CWUni.P2");
    relationStorePtr->AddBid("WeightBid",{"WBid.P1","WBid.P2","WBid.P3"},114514);
    ShowRelation("Weight Bid Test","WBid.P1","WBid.P2");
    relationStorePtr->RemoveUni("UniRemove","WBid.P1",{"WBid.P2"});
    ShowRelation("Uni Remove Test","WBid.P1","WBid.P2");
    relationStorePtr->RemoveBid("BidRemove",{"WBid.P1","WBid.P3"});
    ShowRelation("Bid Remove Test","WBid.P1","WBid.P3");
}

void SaveAndLoadCompareSample(bool isSave){
    ShowRelation(format("Save&Reload({})",isSave?"Save":"load"),"WBid.P1","WBid.P3");
}

void UndoTest(){
    
    relationStorePtr->UndoRemoveBid("BidRemove");
    ShowRelation("Undo Bid Remove Test","WBid.P1","WBid.P3");
    relationStorePtr->UndoAddUni("CustomWeightUniNe");
    ShowRelation("Custom Weight Uni Test","CWUni.P1","CWUni.P2");
}

int main(){
    std::cout<<"==============Add Logic Test=============="<<std::endl;
    relationStorePtr=make_unique<RelationStore>(RelationStore::NewGraphToFile("relation.txt"));
    RelationAddTest();
    std::cout<<"==============Save & Load Test=============="<<std::endl;
    SaveAndLoadCompareSample(true);
    relationStorePtr->SaveGraph();
    relationStorePtr.release();
    relationStorePtr=make_unique<RelationStore>(RelationStore::ReadGraph("relation.txt"));
    SaveAndLoadCompareSample(false);
    std::cout<<"==============Undo Test=============="<<std::endl;
    UndoTest();
    relationStorePtr->SaveGraphTo("relation_after_delete.txt");
    


}

