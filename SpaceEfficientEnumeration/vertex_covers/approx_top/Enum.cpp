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
  cost = vcost;
  for(int i = 0; i < n; i++) m += G[i].size();
  m /= 2;
  elist.resize(m);
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      edge e = G[i][j];
      elist[e.id] = e;
    }
  }
}


//local ratio
int EVC::FindAppMinVC(instance &ins, std::vector<bool> &res, bool flag){
  bool isVC = true;
  for(int i = 0; i < m; i++) {
    int u = elist[i].from, v = elist[i].to;
    isVC &= (ins.I[u] or ins.I[v]);
  }
  if(isVC){
    // std::cout << "isVC" << std::endl;
    int mini = 1e9, id, val = 0;
    for(int i = 0; i < n; i++) {
      val += cost[i]*ins.I[i];
      if(cost[i] < mini and ins.I[i] == false and ins.O[i] == false){
        mini = cost[i];
        id = i;
      }
    }
    // std::cout << "mini:" << mini << " id:" << id << std::endl;
    if(flag) {
      res = ins.I;
      if(mini != 1e9) res[id] = true;      
    }
    return val + mini;
  }


  std::vector<int> local = cost;
  for(int i = 0; i < n; i++) local[i] *= (1-ins.I[i]);

  for(int i = 0; i < n; i++) {
    if(not ins.O[i])continue;
    for(int j = 0; j < G[i].size(); j++) {
      int w = G[i][j].to;
      local[w] = 0;
      if(ins.O[w]) {
        // std::cout << "case 1: both end points are not contained. " << i << ", " << w << std::endl;
        return 1e9;
      }
    }
  }
  // std::cout << "local:";
  // for(int i = 0; i < n; i++) {
  //   std::cout << local[i];
  // }
  // std::cout << std::endl;
  for(int i = 0; i < m; i++) {
    int u = elist[i].from, v = elist[i].to;
    // std::cout << "prev u:" << u << " v:" << v << std::endl;    
    if(local[v] < local[u])std::swap(u, v);
    // std::cout << "swap u:" << u << " v:" << v << std::endl;    
    local[v] -= local[u];
    local[u] = 0;
  }
  for(int i = 0; i < n; i++) {
    if(local[i] > 0) continue;

    bool minimize = ((not ins.I[i]) and (not ins.O[i]));
    for(int j = 0; j < G[i].size() and minimize; j++) {
      minimize &= (local[G[i][j].to] == 0);
    }
    if(minimize) local[i] = 1;
  }
  int val = 0;
  for(int i = 0; i < n; i++) {
    val += cost[i]*(local[i] == 0);
    if(flag) res[i] = (local[i] == 0);  
  }
  return val;
}

void EVC::Enumerate(int k){
  std::priority_queue<instance, std::vector<instance>, std::greater<instance> > que;
  instance ins(n, 0);
  que.push(ins);
  std::vector<bool> VC(n);
  while(k > ans.size() and not que.empty()){
    max_que_size = std::max(max_que_size, (int)que.size());
    ins = que.top();
    que.pop();
    int val = FindAppMinVC(ins, VC, 1);
    ans.push_back(VC);
    for(int i = 0; i < n; i++) {
      if(VC[i] == false or ins.I[i])continue;
      ins.O[i] = true;
      int tmp = FindAppMinVC(ins, VC);
      if(tmp < 1e9){
        instance next(n, tmp);
        next.I = ins.I, next.O = ins.O;
        que.push(next);
      }
      ins.I[i] = true, ins.O[i] = false;
    }
    if(val >= 1e9){
      std::cout << "shine." << std::endl;
    }
    instance next(n, val);
    next.I = VC, next.O = ins.O;
    if(FindAppMinVC(next, VC) < 1e9) que.push(next);
  }
}

void EVC::print(){
  std::cout << "ans size:" << ans.size() << " max que size:" << max_que_size << std::endl;
  // for(int i = 0; i < ans.size(); i++) {
  //   int c = 0;
  //   for(int j = 0; j < ans[i].size(); j++) {
  //     std::cout << ans[i][j];
  //     c += cost[j]*ans[i][j];
  //   }
  //   std::cout << ":" << c << std::endl;
  // }
  // for(int i = 0; i < n; i++) {
  //   for(int j = 0; j < G[i].size(); j++) {
  //     std::cout << G[i][j].to << " ";
  //   }
  //   std::cout << std::endl;
  // }
  sort(ans.begin(), ans.end());
  for(int i = 0; i < ans.size()-1; i++) {
    if(ans[i] == ans[i+1]){
      std::cout << "dupulication." << std::endl;
      exit(1);
    }
  }
}
