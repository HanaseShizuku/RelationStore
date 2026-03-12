# RelationStore

一个简单的存储,修改简单图的工具

~~部分代码从[这里](https://github.com/HanaseShizuku/ChinaMapGraphExperiment)偷的~~

其实是做[ChinaMapGraphExperiment](https://github.com/HanaseShizuku/ChinaMapGraphExperiment)实验时发现Patch功能能单独抽出来用

然后就有了这个项目

## 它能干什么

### 我们做到了以下内容
- **多态脚本驱动**：支持通过文本脚本定义图结构，支持 `Add`（添加）、`Remove`（移除）等操作。
- **混合图支持**：同一实例同时支持 **Uni（有向）** 和 **Bid（无向/双向）** 关系。
- **持久化存储**：支持将当前的内存图结构序列化为 `.txt` 脚本文件，并支持无损加载。
- **动态剪边能力**：支持实时移除特定路径而不影响节点的生存，模拟链路中断等场景。

### 我们可能不会做以下内容
- ~~**带权的关系**：我们暂时不考虑给关系添加权重(正在做 但是难度太大不一定能做出来。)~~
- **性能优化**：做不到！
- **任何维护和技术支持**：这只是一个玩具，请知悉。

## 如何使用

> **特别说明**：下文的`关系名`在脚本中必须唯一

### RelationStoreLib

例子见[demo.cpp](/demo/demo.cpp)文件

- 节点的连接
    - 添加双向关系:
        ```
        AddBid("关系名",{"节点1","节点2"},{到节点1的边权重,到节点2的边权重});//参数2元素数需大于等于2，参数2 3元素数量需相等
        AddBid("关系名",{"节点1","节点2"});//默认权重 参数2元素数需大于等于2
        ```
    - 添加单向关系
        ```
        AddUni("关系名","起点",{"终点1","终点2"}，权重);//参数3元素数需大于等于1
        AddUni("关系名","起点",{"终点1","终点2"});//默认权重 参数3元素数需大于等于1
        ```
- 边的剪裁
    - 双向断开多个节点之间的关系:
        ```
        RemBid("关系名",{"节点1","节点2"});//参数2元素数需大于等于2
        ```
    - 单向断开一个节点与多个节点之间的关系
        ```
        AddUni("关系名","起点",{"终点1","终点2"});//参数3元素数需大于等于1
        ```
- 操作撤销并从脚本里删除
    - 从存储的文本脚本里撤销添加双向关系
        ```
        UndoAddBid("'添加双向关系'的关系名");
        ```
    - 从存储的文本脚本里撤销添加单向关系
        ```
        UndoAddUni("'添加单向关系'的关系名");
        ```
    - 从存储的文本脚本里撤销断开双向关系
        ```
        UndoRemoveBid("'断开双向关系'的关系名");
        ```
    - 从存储的文本脚本里撤销断开单向关系
        ```
        UndoRemoveUni("'断开单向关系'的关系名");
        ```
### 文本脚本格式

```
OpName(RelationName,StringArgs...)[IntArgs];
```

OpName:
- Uni 添加单向关系，`StringArgs[0]`为起点，`StringArgs[1:]`为终点，`IntArgs[x]`为`StringArgs[0]`到`StringArgs[x+1]`的边的权重
- Bid 添加双向关系 `StringArgs`为并列的若干个点，`IntArgs`此时只有一个元素，作为互相连接这几个点的边的权重
- RemUni 单向断开一个节点与多个节点之间的关系 参数类似于Uni，此时权重保持为空
- RemBid 双向断开多个节点之间的关系 参数类似于Bid，此时权重保持为空

脚本文件示例

```
Uni("DefaultWeightUni","DWUni.begin","DWUni.end1","DWUni.end2")[1,1];
Uni("CustomWeightUniPo","CWUni.P1","CWUni.P2")[9];
Uni("CustomWeightUniNe","CWUni.P2","CWUni.P1")[1];
Bid("WeightBid","WBid.P1","WBid.P2","WBid.P3")[114514];
RemUni("UniRemove","WBid.P1","WBid.P2")[];
RemBid("BidRemove","WBid.P1","WBid.P3")[];
```

## 许可证

本项目采用 MIT License 开源。