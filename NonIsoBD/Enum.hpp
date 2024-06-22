#ifndef __ENUM__
#define __ENUM__
#include<vector>
using bigint = long long int;
using Matrix = std::vector<std::vector<int> >;
using Graph = std::vector<std::vector<int> >;

class NonIsoGraphs{
public:
  NonIsoGraphs(){}
  void init(int n, int _c);
  void Enumerate();
  int size(){return R.size();};
  void print();
private:
  // void dfs(int size, int depth = 0);
  int n, c, h, ans = 0;
  Graph Parent(Graph G);
  bool GraphIso(Graph G, Graph H);
  std::vector<Graph> EnumChildren(Graph G);
  void EnumRec(Graph G, int depth);
  bool connectivity(Graph &G);
  Graph R;
  // std::vector<bigint> ans;
};

#endif // __ENUM__
