#ifndef __ENUM__
#define __ENUM__
#include<tuple>
#include<stack>
#include<queue>

#include"Element.hpp"
#include"List.hpp"
#include"Graph.hpp"
#include"AddibleList.hpp"
#include"basicDataStructure.hpp"

using log = std::tuple<int, int, int, int>;
using bigint = long long int;

class ESTP{
public:
  ESTP(std::vector<std::vector<edge>> _G){init(_G);};
  ESTP(){};
  void Enumerate(int s, int t);
  void init(std::vector<std::vector<edge>> _G);
  void print();
private:
  void RecEnum(int s, int t);
  bool Reachable(int s, int t);
  int nextStartPoint(int v);
  int dfs(int v, int t);
  
  Graph G;
  int n, m, rec = 0;
  AddibleList<int> solution;
  std::vector<bigint> ans;
  std::queue<int> que;
  std::vector<bool> used;
};
#endif // __ENUM__
