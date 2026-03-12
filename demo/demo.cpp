#include "include/relation_store.h"
#include "include/bfs_result.h"
#include<iostream>
#include<sstream>

using namespace RelationStoreLib;
using namespace std;


unique_ptr<RelationStore> p;
unique_ptr<RelationStore> a;

void VectorPrintSingleLine(const vector<string> &v,const string &append="",const string &prefix="[",const string &postfix="]"){
    cout<<prefix;
    for(const auto &s:v){
        cout<<s<<" ";
    }
    cout<<postfix<<append<<endl;
    
}

void ConnectTest(const string &src,const string &dest,std::ostream &s=cout,unique_ptr<RelationStore> &bfs=p){
    s<<"测试项目:"<<src<<"->"<<dest<<endl;
    auto resultP=RelationStoreLib::algorithm::BFSResult::TraverseFromStore(src,*bfs);
    auto resultN=RelationStoreLib::algorithm::BFSResult::TraverseFromStore(dest,*bfs);
    s<<"正向("<<src<<"->"<<dest<<")";
    VectorPrintSingleLine(resultP.GetPath(dest),to_string(resultP.GetHopCount(dest))+"跳");
    s<<"反向("<<dest<<"->"<<src<<")";
    VectorPrintSingleLine(resultN.GetPath(src),to_string(resultN.GetHopCount(src))+"跳");
}

void TestLogic(){
    cout<<endl<<"#####添加测试#####"<<endl;
    //无向图测试
    p->AddBid("无向边双向连接测试",{"无向图节点1","无向图节点2"});
    cout<<endl<<"===无向边双向连通测试==="<<endl;
    ConnectTest("无向图节点1","无向图节点2");

    //全连接测试
    p->AddBid("全连通测试",{"全连通1","全连通2","全连通3"});
    cout<<endl<<"===全连通测试==="<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");

    //有向图测试
    p->AddUni("有向边单向连接测试","有向边起点",{"有向边终点"});
    cout<<endl<<"===有向边连通测试==="<<endl;
    ConnectTest("有向边起点","有向边终点");
    
    cout<<endl<<"===单起点多终点有向图连通测试==="<<endl;
    p->AddUni("有向图多终点连接测试","有向起点",{"有向图终点1","有向图终点2"});
    ConnectTest("有向起点","有向图终点1");
    ConnectTest("有向起点","有向图终点2");

    cout<<endl<<"#####剪边测试#####"<<endl;

    //Bid
    p->RemoveBid("无向边移除测试",{"全连通1","全连通2"});
    cout<<endl;
    cout<<"===无向边移除测试==="<<endl;
    cout<<"预期:\n1<-/->2\n1->3->2\n2->3->1\n1<->3\n2<->3"<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");

    //Uni
    p->RemoveUni("有向边移除测试","全连通1",{"全连通3"});
    cout<<endl;
    cout<<"===有向边移除测试==="<<endl;
    cout<<"预期:\n1<-/->2\n1-/->3 2不可达\n2->3->1\n1<-/->3\n2<->3"<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");
}

void TestNew(){
    p=make_unique<RelationStore>(RelationStore::NewGraphToFile("test.txt"));
}

void TestRead(){
    a=make_unique<RelationStore>(RelationStore("test.txt"));
}

void TestSave(){
    p->SaveGraph();
}

void DeleteRelation(){
    cout<<endl<<"#####删除测试#####"<<endl;
    cout<<endl<<"===删除前==="<<endl;
    ConnectTest("无向图节点1","无向图节点2");
    p->UndoAddBid("无向边双向连接测试");
    cout<<endl<<"===删除后==="<<endl;
    ConnectTest("无向图节点1","无向图节点2");
}

void SaveFinalResultStr(stringstream &ss,unique_ptr<RelationStore> &bfs){
    ConnectTest("全连通1","全连通2",ss,bfs);
    ConnectTest("全连通1","全连通3",ss,bfs);
    ConnectTest("全连通2","全连通3",ss,bfs);
}

int _main(){
    RelationStore x=RelationStore::NewGraphToFile("savetest.txt");
    x.AddUni("测试关系","起点",{"终点1","终点2"});
    x.SaveGraph();
}

int main(){
    //先新建
    TestNew();
    //测逻辑
    TestLogic();
    cout<<endl<<"#####保存读取#####"<<endl;
    stringstream ss1;
    SaveFinalResultStr(ss1,p);
    string beforesave;
    ss1>>beforesave;
    TestSave();
    //原样读取
    TestRead();
    a->ReadGraph();
    stringstream ss2;
    string aftersave;
    SaveFinalResultStr(ss2,a);
    ss2>>aftersave;

    cout<<"Before==After:"<<(beforesave==aftersave?"Yes":"No")<<endl;
    DeleteRelation();
    p->SaveGraphTo("test_afterdel.txt");


}