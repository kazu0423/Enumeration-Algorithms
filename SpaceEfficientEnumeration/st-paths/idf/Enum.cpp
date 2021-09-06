#include<iostream>
#include<vector>
#include<memory>
#include<queue>
#include<algorithm>
#include<iomanip>

#include"Enum.hpp"

using pii = std::pair<int, int>;

EST::EST(std::vector<std::vector<edge> > H){
  G = H;
  n = G.size(), m = 0;
  for(int i = 0; i < n; i++) m += G[i].size();
  elist.resize(m);
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      edge e = G[i][j];
      elist[e.id] = e;
    }
  }
}


int EST::ComputeShortestPathLength (std::vector<bool> &I, std::vector<bool> &O ,int s, int t){
  std::priority_queue<pii, std::vector<pii>, std::greater<pii> > que;
  std::vector<int> dp(n, 1e9);
  for(int i = 0; i < n; i++) {
    if(I[i]){
      edge e = elist[i];
      dp[e.from] = dp[e.to] = 0;
    }
  }
  dp[s] = 0;
  que.push(pii(0, s));
  while(not que.empty()){
    int c = que.top().first;
    int v = que.top().second; 
    que.pop();
    for(int i = 0; i < G[v].size(); i++) {
      edge e = G[v][i];
      if(O[e.id] or I[e.id])continue;
      if(dp[e.to] > c + e.cost){
        dp[e.to] = c + e.cost;
        que.push(pii(dp[e.to], e.to));
      }
    }
  }
  return dp[t];
}


void EST::FindShortestPath(instance &ins, std::vector<int> &res, int &len){
  int s = ins.s, t = ins.t;
  std::vector<bool> I = ins.I, O = ins.O;
  std::priority_queue<pii, std::vector<pii>, std::greater<pii> > que;
  std::vector<int> dp(n, 1e9), restore(2*n, -1);
  for(int i = 0; i < n; i++) {
    if(I[i]){
      edge e = elist[i];
      dp[e.from] = dp[e.to] = 0;
    }
  }
  dp[s] = 0;
  que.push(pii(0, s));
  while(not que.empty()){
    int c = que.top().first;
    int v = que.top().second;
    que.pop();
    for(int i = 0; i < G[v].size(); i++) {
      edge e = G[v][i];
      if(O[e.id] or I[e.id])continue;
      if(dp[e.to] > c + e.cost){
        dp[e.to] = c + e.cost;
        que.push(pii(dp[e.to], e.to));
        restore[2*e.to]   = v;
        restore[2*e.to+1] = e.id;
      }
    }
  }
  std::vector<int> id_list;
  int tmp = t;
  while(tmp != s){
    id_list.push_back(restore[2*tmp + 1]);
    tmp = restore[2*tmp];
  }
  reverse(id_list.begin(), id_list.end());
  for(int i = 0; i < id_list.size(); i++) {
    res[len++] = id_list[i];
  }
}

void EST::Enumerate(int s, int t, double eps, int k){
  instance ins(s, t, m, 0);
  double len = ComputeShortestPathLength(ins.I, ins.O, s, t);
  int total_weight = 0;
  double next_mini = len + 0.000001;
  double prev = len;
  for(int i = 0; i < m; i++) total_weight += elist[i].cost;
  while(len < total_weight and ans.size() < k){
    prev = DFS(ins, k, prev, next_mini);
    std::swap(prev, next_mini);
  }
}

int EST::DFS(instance &ins, int k, double mini, double maxi){
  instance tmp = ins;
  int weight = 0;
  for(int i = 0; i < m; i++) weight += elist[i].cost*ins.I[i];
  int val = ComputeShortestPathLength(ins.I, ins.O, ins.s, ins.t) + weight, len = 0;
  if(val >= maxi) return val + (val == maxi);
  int res = 1e9;
  std::vector<int> P(n);
  FindShortestPath(ins, P, len);
  std::vector<bool> sol = ins.I;
  for(int i = 0; i < len; i++) sol[elist[P[i]].id] = true;
  if(mini <= val and val < maxi and ans.size() < k) ans.push_back(sol);

  for(int i = 0; i < len; i++) {
    edge e = elist[P[i]];
    ins.O[e.id] = true;
    res = std::min(res, DFS(ins, k, mini, maxi));
    ins.I[e.id] = true, ins.O[e.id] = false;
    if(ins.s != e.to) ins.s = e.to;
    else ins.s = e.from;
    val += e.cost;
  }
  ins = tmp;
  return res;
}

void EST::print(){
  std::cout << "ans size:" << ans.size() << std::endl;
  for(int i = 0; i < ans.size(); i++) {
    for(int j = 0; j < ans[i].size(); j++) {
      std::cout << ans[i][j];
    }
     std::cout << std::endl;
     int len = 0;
     for(int j = 0; j < ans[i].size(); j++) {
       if(ans[i][j]){
         std::cout << "(" << elist[j].from << ", " << elist[j].to << ") ";
         len += elist[j].cost;
       }
     }
     std::cout << std::endl;
     std::cout << "len:" << len << std::endl;
     std::cout << std::endl;
  }
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      std::cout << G[i][j].to <<":"<< G[i][j].cost << " ";
    }
    std::cout << std::endl;
  }
}
