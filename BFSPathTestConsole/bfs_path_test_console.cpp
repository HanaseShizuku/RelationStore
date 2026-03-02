#include "bfs_path.h"
#include "bfs_result.h"
#include<iostream>
#include<sstream>

using namespace BFSPathLib;
using namespace std;

unique_ptr<BFSPath> bfsp;
unique_ptr<BFSPath> bfspafter;

void VectorPrintSingleLine(const vector<string> &v,const string &append="",const string &prefix="[",const string &postfix="]"){
    cout<<prefix;
    for(const auto &s:v){
        cout<<s<<" ";
    }
    cout<<postfix<<append<<endl;
    
}

void ConnectTest(const string &src,const string &dest,std::ostream &s=cout,unique_ptr<BFSPath> &bfs=bfsp){
    s<<"测试项目:"<<src<<"->"<<dest<<endl;
    auto resultP=bfs->Traverse(src);
    auto resultN=bfs->Traverse(dest);
    s<<"正向("<<src<<"->"<<dest<<")";
    VectorPrintSingleLine(resultP.GetPath(dest),to_string(resultP.GetHopCount(dest))+"跳");
    s<<"反向("<<dest<<"->"<<src<<")";
    VectorPrintSingleLine(resultN.GetPath(src),to_string(resultN.GetHopCount(src))+"跳");
}

void TestLogic(){
    cout<<endl<<"#####添加测试#####"<<endl;
    //无向图测试
    bfsp->AddBid("无向边双向连接测试",{"无向图节点1","无向图节点2"});
    cout<<endl<<"===无向边双向连通测试==="<<endl;
    ConnectTest("无向图节点1","无向图节点2");

    //全连接测试
    bfsp->AddBid("全连通测试",{"全连通1","全连通2","全连通3"});
    cout<<endl<<"===全连通测试==="<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");

    //有向图测试
    bfsp->AddUni("有向边单向连接测试","有向边起点",{"有向边终点"});
    cout<<endl<<"===有向边连通测试==="<<endl;
    ConnectTest("有向边起点","有向边终点");
    
    cout<<endl<<"===单起点多终点有向图连通测试==="<<endl;
    bfsp->AddUni("有向图多终点连接测试","有向起点",{"有向图终点1","有向图终点2"});
    ConnectTest("有向起点","有向图终点1");
    ConnectTest("有向起点","有向图终点2");

    cout<<endl<<"#####剪边测试#####"<<endl;

    //Bid
    bfsp->RemoveBid("无向边移除测试",{"全连通1","全连通2"});
    cout<<endl;
    cout<<"===无向边移除测试==="<<endl;
    cout<<"预期:\n1<-/->2\n1->3->2\n2->3->1\n1<->3\n2<->3"<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");

    //Uni
    bfsp->RemoveUni("有向边移除测试","全连通1",{"全连通3"});
    cout<<endl;
    cout<<"===有向边移除测试==="<<endl;
    cout<<"预期:\n1<-/->2\n1-/->3 2不可达\n2->3->1\n1<-/->3\n2<->3"<<endl;
    ConnectTest("全连通1","全连通2");
    ConnectTest("全连通1","全连通3");
    ConnectTest("全连通2","全连通3");
}

void TestNew(){
    bfsp=make_unique<BFSPath>(BFSPath::NewGraphToFile("test.txt"));
}

void TestRead(){
    bfspafter=make_unique<BFSPath>(BFSPath("test.txt"));
}

void TestSave(){
    bfsp->SaveGraph();
}

void DeleteRelation(){
    bfsp->RemoveByNameAndType("*无向边双向连接测试");
}

void SaveFinalResultStr(stringstream &ss,unique_ptr<BFSPath> &bfs){
    ConnectTest("全连通1","全连通2",ss,bfs);
    ConnectTest("全连通1","全连通3",ss,bfs);
    ConnectTest("全连通2","全连通3",ss,bfs);
}

int main(){
    //先新建
    TestNew();
    //测逻辑
    TestLogic();
    
    cout<<endl<<"#####保存读取#####"<<endl;
    stringstream ss1;
    SaveFinalResultStr(ss1,bfsp);
    string beforesave;
    ss1>>beforesave;
    TestSave();
    //原样读取
    TestRead();
    bfspafter->ReadGraph();
    stringstream ss2;
    string aftersave;
    SaveFinalResultStr(ss2,bfspafter);
    ss2>>aftersave;

    cout<<"Before==After:"<<(beforesave==aftersave?"Yes":"No")<<endl;
    DeleteRelation();
    bfsp->SaveGraphTo("test_afterdel.txt");


}