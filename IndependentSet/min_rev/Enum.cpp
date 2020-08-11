#include<iostream>
#include<vector>
#include<memory>
#include<algorithm>

#include"Enum.hpp"

#define INF 1e9
using bigint = long long int;

EIS::EIS(std::vector<std::vector<int> > H){
  n = H.size();
  // ans.resize(n, 0);
  G.resize(H.size());
  deg.resize(n, 0);
  solution.resize(n);
  SLO.resize(n);
  for (int i = 0; i < n; i++) G[i].resize(n);
  for (int i = 0; i < n; i++) G[i] = H[i];
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      std::cout << G[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::vector<int> c(n);
  for (int i = 0; i < n; i++) c[i] = i;
  cand.init(c);
}


void EIS::Enumerate(){
  RecEnum();
}


void EIS::ComputeSLO(){
  for (int i = cand.begin(); i != cand.end(); i = cand.GetNext(i)) {
    deg[i] = 0;
    for (int j = cand.begin(); j != cand.end(); j = cand.GetNext(j)) {
      deg[i] += G[i][j];
    }
  }
  for (int i = 0; i < cand.size(); i++) {
    mini =   1e9;
    // maxi =  -1e9;
    for (int j = cand.begin(); j != cand.end(); j = cand.GetNext(j)) {
      if(deg[j] < mini) v = j, mini = deg[j];
      // if(deg[j] > maxi) v = j, maxi = deg[j];
    }
    for (int j = cand.begin(); j != cand.end(); j = cand.GetNext(j)) {
      deg[j] -= G[v][j];
    }
    deg[v] = 1e9;
    SLO[i + tail] = v;
  }
}

void EIS::RecEnum() {
  ans++;
  if(ans > 1e9)return;
  int stack = 1, num_children = cand.size();
  ComputeSLO();
  for (int i = 0; i < num_children; i++) {
    cand.remove(SLO[tail++]);
  }
  for (int i = 0; i < num_children; i++) {
    v = SLO[--tail];
    stack = 1;
    for (int j = cand.begin(); j != cand.end(); j = cand.GetNext(j)){
      // if(G[v][j] == 1) j = cand.remove(j), stack++;
      stack += G[v][j];
      if(G[v][j] == 1) j = cand.remove(j);
      // std::cout << G[v][j] << " " << j << " " << n << " " << cand.end() << std::endl;
    }
    solution.push(v);
    if(not cand.empty()) RecEnum();
    // else ans[solution.size()]++;
    else ans++;
    if(ans > 1e9)return;
    solution.pop();
    for (int j = 0; j < stack; j++) cand.undo();
  }
  return;
}


void EIS::print(){
  // int sum = 0;
  // for (int i = 0; i < ans.size(); i++) sum += ans[i];
  // std::cout << "sum:" << sum << std::endl;
  // for (int i = 0; i < n; i++) {
  //   std::cout << "[" << i << "]:" << ans[i] << std::endl;
  //   if(ans[i] == 0)break;
  // }
}
