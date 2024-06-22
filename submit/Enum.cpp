#include<iostream>
#include<algorithm>
#include"Enum.hpp"

#define DELETED -2

void ESTP::init(std::vector<std::vector<edge>> _G){
  n = _G.size();
  G.init(_G);
  m = 0;
  for (int i = 0; i < _G.size(); i++) m += _G[i].size();
  m /= 2;
  solution.init(m);
  for (int i = 0; i < m; i++) solution.set(i, i);
  used.resize(n);
  ans.resize(n + 10, 0);
  isLeaf.resize(n,-1);
  dist.resize(n, 1e9);
  dist_t.resize(n, 1e9);
  shortest.resize(n, 1e9);
  par.resize(n, -1);
}

// OK
void ESTP::Enumerate(int s, int t, int len){
  if(len != -1) l = len;
  // std::cout << "Start Enumerate." << std::endl;
  EnumShortestPaths(s, t, 0);
  for (int i = 0; i < n; i++) dist[i] = 1e9;
  dist[s] = 0;
  que.push(s);
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = G[v].begin(); i != G[v].end(); i = G[v].GetNext(i)) {
      int u = G[v][i].to;
      if(dist[u] != 1e9)continue;
      if(u == t) {
        std::queue<int>().swap(que);
        break;
      }
      dist[u] = std::min(dist[u], dist[v] + 1);
      que.push(u);
    }
  }

  // std::cout << "First recursion." << std::endl;
  if(isNonShortest(s, t)) RecEnum(s, t, dist[t]);
}

void ESTP::RecEnum(int s, int t, int d){
  rec++;
  // std::cout << "Rec s:" << s << std::endl;  
  // print();
  // std::cout << std::endl;  
  solution.add(s);
  G.RemoveVertex(s);
  for (int i = G[s].begin(); i != G[s].end(); i = G[s].GetNext(i)) {
    int v = G[s][i].to;
    for (int j = 0; j < n; j++) dist[j] = 1e9;
    que.push(v);
    dist[v] = 0;
    while(not que.empty()){
      int u = que.front();
      que.pop();
      for (int j = G[u].begin(); j != G[u].end(); j = G[u].GetNext(j)) {
        int w = G[u][j].to;
        if(dist[w] != 1e9)continue;
        dist[w] = std::min(dist[w], dist[u] + 1);
        if(w == t) {
          std::queue<int>().swap(que);
          break;
        }
        que.push(w);
      }
    }
    // std::cout << "dist:" <<  dist[t] << std::endl;    
    if(dist[t] == 1e9) continue;
    if(dist[t] + solution.size() + 1 > l) continue;
    if(dist[t] > d-1) {
      // std::cout << "NonShortest" << std::endl;
      EnumShortestPaths(v, t, solution.size());
      RecEnum(v, t, dist[t]);
    }else if(isNonShortest(v, t)){
      // std::cout << "Shortest" << std::endl;
      RecEnum(v, t, d-1);
    }
  }
  G.undo();
  solution.undo();
}

// OK
int ESTP::isNonShortest (int s, int t){
  // std::cout << "Start NonShortest." << std::endl;
  for (int i = G.begin(); i != G.end(); i = G.GetNext(i)) dist[i] = 1e9, isLeaf[i] = -1, par[i] = -1, dist_t[i] = 1e9;
  dist[s] = 0;
  que.push(s);
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = G[v].begin(); i != G[v].end(); i = G[v].GetNext(i)) {
      int u = G[v][i].to;
      if(dist[u] != 1e9) continue;
      dist[u] = std::min(dist[u], dist[v] + 1);
      par[u] = v;
      isLeaf[v] = u;
      que.push(u);
    }
  }

  que.push(t);
  dist_t[t] = 0;
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = G[v].begin(); i != G[v].end(); i = G[v].GetNext(i)) {
      int u = G[v][i].to;
      if(dist_t[u] != 1e9) continue;
      dist_t[u] = std::min(dist_t[u], dist_t[v] + 1);
      que.push(u);
    }
  }
  
  for(int i = G.begin(); i != G.end(); i = G.GetNext(i)) {
    for(int j = G[i].begin(); j != G[i].end(); j = G[i].GetNext(j)) {
      int u = G[i][j].to;
      if(par[u] == i or par[i] == u) continue;
      if(dist[i] + dist_t[u] + 1 > dist[t] and dist_t[u] != 1e9){
          // std::cout << "End NonShortest." << std::endl;
          return 1;          
      } 
    }
  }
  // std::cout << "End NonShortest." << std::endl;
  return 0;
}

// OK
void ESTP::EnumShortestPaths(int s, int t, int buf){
  // std::cout << "Start ESP." << std::endl;
  for (int i = G.begin(); i != G.end(); i = G.GetNext(i)) dist[i] = 1e9, shortest[i] = 0;
  dist[s] = 0;
  que.push(s);
  shortest[s] = 1;

  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = G[v].begin(); i != G[v].end(); i = G[v].GetNext(i)) {
      int u = G[v][i].to;
      if(dist[u] == dist[v] + 1 or dist[u] == 1e9){
        shortest[u] += shortest[v];
      }
      if(dist[u] == 1e9) que.push(u);
      dist[u] = std::min(dist[u], dist[v] + 1);
    }
  }
  // std::cout << dist[t] << " buf:" << buf << std::endl;
  // std::cout << shortest[t] << std::endl;
  ans[dist[t] + buf + 1] += shortest[t];
  // std::cout << "End ESP." << std::endl;
}

void ESTP::print(){
  int sum = 0, maxi = 0;
  // std::cout << " rec:" << rec << std::endl;
  for (int i = 0; i < ans.size(); i++) {
    sum += ans[i];
    if(ans[i] > 0) maxi = i;
  }
  // std::cout << "size:" << sum << std::endl;
  // for (int i = 0; i <= maxi; i++) {
  //   std::cout << i << ":" << ans[i] << std::endl;
  // }
  std::cout << sum << std::endl;
}
 
