#include<iostream>
#include<algorithm>
#include"Enum.hpp"

#define DELETED -2

void ESTP::init(std::vector<std::vector<edge>> _G){
  n = _G.size(), m = 0;
  G.init(_G);
  for (int i = 0; i < _G.size(); i++) m += _G[i].size();
  m /= 2;
  solution.init(m);
  for (int i = 0; i < m; i++) solution.set(i, i);
  used.resize(n);
  ans.resize(n + 1, 0);
}

void ESTP::Enumerate(int s, int t){
  RecEnum(s, t);
}

void ESTP::RecEnum(int s, int t){
  rec++;
  if(s == t){
    solution.add(s);
    // for (int i = solution.begin(); i != solution.end(); i = solution.GetNext(i)) {
    //   printf("%d ", i);
    // }
    //   printf("\n");
    ans[solution.size()]++;
    solution.undo();
    return;    
  }
  G.RemoveVertex(s);
  for (int i = G[s].begin(); i != G[s].end(); i = G[s].GetNext(i)) {
    int v = G[s][i].to;
    if(Reachable(v, t)) {
      int tmp = nextStartPoint(v);
      solution.add(s);
      RecEnum(v, t);
      solution.undo();
    } 
  }
  G.undo();
}

int ESTP::nextStartPoint(int v){
  for (int i = 0; i < n; i++) used[i] = false;
  used[v] = true;
  return v;
}

bool ESTP::Reachable(int s, int t){
  if(s == t)return true;
  for (int i = 0; i < n; i++) used[i] = false;
  que.push(s);
  used[s] = true;
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = G[v].begin(); i != G[v].end(); i = G[v].GetNext(i)) {
      int u = G[v][i].to;
      if(used[u] == true)continue;
      if(u == t)return true;
      used[u] = true;
      que.push(u);
    }
  }
  return false;
}

void ESTP::print(){
  int sum = 0, maxi = 0;
  std::cout << " rec:" << rec << std::endl;
  for (int i = 0; i < ans.size(); i++) {
    sum += ans[i];
    if(ans[i] > 0) maxi = i;
  }
  std::cout << "size:" << sum << std::endl;
  for (int i = 0; i <= maxi; i++) {
    std::cout << i << ":" << ans[i] << std::endl;
  }
}
 
