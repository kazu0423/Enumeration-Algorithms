#include<iostream>
#include<vector>
#include<memory>
#include<queue>
#include<algorithm>
#include<iomanip>

#include"Enum.hpp"

using bigint = long long int;
using pii = std::pair<int, int>;

EVC::EVC(std::vector<std::vector<edge> > H, std::vector<int> vcost){
  G = H;
  n = G.size(), m = 0;
  cost.resize(n);
  for(int i = 0; i < n; i++) m += G[i].size(), cost[i] = vcost[i];
  elist.resize(m);
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      edge e = G[i][j];
      elist[e.id] = e;
    }
  }
}


//local ratio
int EVC::FindAppMinVC(instance &ins, std::vector<int> &res, bool flag){
  std::vector<int> local = cost;
  for(int i = 0; i < n; i++) local[i] = local[i]*(1-ins.I[i]);

  for(int i = 0; i < m; i++) {
    int u = elist[i].from, v = elist[i].to, c;
    if(local[v] < local[u])std::swap(u, v);
    local[v] -= local[u];
    local[u] = 0;
  }
  for(int i = 0; i < n; i++) {
    if(local[i] > 0) continue;
    bool minimize = true;
    for(int j = 0; j < G[i].size(); j++) {
      minimize &= (local[G[i][j].to] == 0);
    }
    if(minimize) local[i] = 1;
  }
  int val = 0;
  for(int i = 0; i < n; i++) {
    val += cost[i]*(local[i] == 0);
  }
  if(flag){
    for(int i = 0; i < n; i++) res[i] = (local[i] == 0);  
  }
  return val;
}


void EVC::Enumerate(int k){
  std::priority_queue<instance, std::vector<instance>, std::greater<instance> > que;
  instance ins(n, 0);
  que.push(ins);
  std::vector<int> VC(n);
  std::vector<bool> I(n), O(n);
  while(k > ans.size() and not que.empty()){
    ins = que.top();
    que.pop();
    int val = FindAppMinVC(ins, VC, 1);
    for(int i = 0; i < n; i++) {
      if(VC[i] == false or ins.I[i])continue;
      I = ins.I, O = ins.O;
      O[i] = true;

      bool addible = (val < 1e8);
      for(int j = 0; j < G[i].size(); j++) {
        I[G[i][j].to] = true;
        addible &= (O[G[i][j].to] == false);
      }
      if(addible){
        instance next(n, val);
        next.I = I, next.O = O;
        que.push(next);
      }
      ins.I[i] = true;
    }
    ans.push_back(ins.I);
  }
}

void EVC::print(){
  std::cout << "ans size:" << ans.size() << std::endl;
  for(int i = 0; i < ans.size(); i++) {
    int c = 0;
    for(int j = 0; j < ans[i].size(); j++) {
      std::cout << ans[i][j];
      c += cost[j]*ans[i][j];
    }
    std::cout << ":" << c << std::endl;
  }
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      std::cout << G[i][j].to << " ";
    }
    std::cout << std::endl;
  }
}
