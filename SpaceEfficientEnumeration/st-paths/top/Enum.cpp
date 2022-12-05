#include<iostream>
#include<vector>
#include<memory>
#include<queue>
#include<algorithm>
#include<iomanip>

#include"Enum.hpp"

using bigint = long long int;
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

void EST::Enumerate(int s, int t, int k){
  std::priority_queue<instance, std::vector<instance>, std::greater<instance> > que;
  instance ins(s, t, m, 0);
  que.push(ins);
  std::vector<int> P(n);
  while(k > ans.size() and not que.empty()){
    max_que_size = std::max(max_que_size, (int)que.size());
    ins = que.top();
    s = ins.s, t = ins.t;
    que.pop();
    int val = 0, len = 0;
    for(int i = 0; i < m; i++) val += elist[i].cost*ins.I[i];
    FindShortestPath(ins, P, len);
    for(int i = 0; i < len; i++) {
      edge e = elist[P[i]];
      ins.O[e.id] = true;
      int tmp = ComputeShortestPathLength(ins.I, ins.O, s, t);
      if(tmp < 1e8){
        instance next(s, t, m, val + tmp);
        next.I = ins.I, next.O = ins.O;
        que.push(next);
      }

      ins.I[e.id] = true, ins.O[e.id] = false;
      if(s != e.to) s = e.to;
      else s = e.from;
      val += e.cost;
    }
    ans.push_back(ins.I);
  }
}

void EST::print(){
  std::cout << "ans size:" << ans.size() << " max queue size:" << max_que_size << std::endl;
  // for(int i = 0; i < ans.size(); i++) {
  //   for(int j = 0; j < ans[i].size(); j++) {
  //     std::cout << ans[i][j];
  //   }
    //  std::cout << std::endl;
    //  int len = 0;
    //  for(int j = 0; j < ans[i].size(); j++) {
    //    if(ans[i][j]){
    //      std::cout << "(" << elist[j].from << ", " << elist[j].to << ") ";
    //      len += elist[j].cost;
    //    }
    //  }
    //  std::cout << std::endl;
    //  std::cout << "len:" << len << std::endl;
    //  std::cout << std::endl;
  // }
  // for(int i = 0; i < n; i++) {
  //   for(int j = 0; j < G[i].size(); j++) {
  //     std::cout << G[i][j].to <<":"<< G[i][j].cost << " ";
  //   }
  //   std::cout << std::endl;
  // }
}
