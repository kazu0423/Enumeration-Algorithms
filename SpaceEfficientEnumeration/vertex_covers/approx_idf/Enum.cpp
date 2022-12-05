#include<iostream>
#include<vector>
#include<memory>
#include<queue>
#include<algorithm>
#include<iomanip>

#include"Enum.hpp"

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
int EVC::FindAppMinVC(instance ins, std::vector<bool> &res, bool flag){
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


void EVC::Enumerate(int k, double eps){
  instance ins(n, 0);
  std::vector<bool> VC(n, false);

  double val = FindAppMinVC(ins, VC)/2. - 1;
  while(k > ans.size()){
    std::cout << "val:" << val << ", " << (1+eps)*val << std::endl;
    if(DFS(ins, k, val, (1+eps)*val) >= 1e8)break;
    val *= (1+eps);
  }
}

int EVC::DFS(instance ins, int k, double mini, double maxi, bool output) {  
  if(ans.size() >= k)return 1e9;

  std::vector<bool> VC(n);
  int val = FindAppMinVC(ins, VC, 1);
  // std::cout << "d:" << d << " n:" << n << " val:" << val<< std::endl;
  // std::cout << "VC:";
  // for(int i = 0; i < n; i++) {
  //   std::cout << VC[i];
  // }
  // std::cout << std::endl;
  // std::cout << " I:";
  // for(int j = 0; j < n; j++) {
  //   std::cout << ins.I[j];
  // }
  // std::cout << std::endl << " O:";
  // for(int j = 0; j < n; j++) {
  //   std::cout << ins.O[j];
  // }
  // std::cout << std::endl;
 
  if(maxi <= val){
    // if(val < 1e9) std::cout << "return for val:" << val << std::endl;
    return val;    
  } 
  // std::cout << std::endl;

  int res = 1e9;
  if((mini <= val and val < maxi) or output) {
    // for(int i = 0; i < n; i++) {
    //   if(ins.O[i] and VC[i]){
    //     std::cout << "case 3: duplication." << std::endl;
    //   }
    // }
    ans.push_back(VC);
    output = true;
  }
  for(int i = 0; i < n; i++) {
    if(VC[i] == false or ins.I[i])continue;
    ins.O[i] = true;
    res = std::min(res, DFS(ins, k, mini, maxi, output));
    ins.I[i] = true, ins.O[i] = false;
    }
  res = std::min(res, DFS(ins, k, mini, maxi, output));
  return res;
}

void EVC::print(){
  std::cout << "ans size:" << ans.size() << std::endl;
  // std::sort(ans.begin(), ans.end());  
  // for(int i = 0; i < ans.size(); i++) {
  //   if(checkVC(ans[i]) == false){
  //     std::cerr << "not VC. " << std::endl;
  //     exit(1);
  //   }
  //   int c = 0;
  //   for(int j = 0; j < ans[i].size(); j++) {
  //     std::cout << ans[i][j];
  //     c += cost[j]*ans[i][j];
  //   }
  //   std::cout << ":" << c << std::endl;
  //   // std::cout << std::endl;
  // }
  // for(int i = 0; i < n; i++) {
  //   for(int j = 0; j < G[i].size(); j++) {
  //     std::cout << G[i][j].to << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // std::sort(ans.begin(), ans.end());
  // for(int i = 0; i < ans.size()-1; i++) {
  //   if(ans[i] == ans[i+1]) {
  //     std::cout << "duplication." << std::endl;
  //     exit(1);
  //   }
  // }
}



bool EVC::checkVC(std::vector<bool> VC){
    for(int i = 0; i < G.size(); i++) {
        if(VC[i])continue;
        for(int j = 0; j < G[i].size(); j++) {
            if(VC[G[i][j].to] == false)return false;
        }
    }
    return true;
}