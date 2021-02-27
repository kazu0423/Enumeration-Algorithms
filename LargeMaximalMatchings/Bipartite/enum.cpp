#include "enum.hpp"

#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <functional>
using matching = boost::dynamic_bitset<>;
using constraint = boost::dynamic_bitset<>;

auto compare = [](matching a, matching b){return a.count() > b.count();};

void computeMatched(BipartiteGraph &G, matching &m,
                    std::vector<int> &matched) {
  for (int i = 0; i < G.size(); i++) matched[i] = -1;
  for (int i = 0; i < G.edgeSize(); i++) {
    if (m.test(i) == true) {
      int u = G.getEdge(i).first, v = G.getEdge(i).second;
      matched[u] = matched[v] = i;
    }
  }
}

int DFS(std::vector<std::vector<std::vector<int> > > &B, int v, int t, int f,
        std::vector<bool> &used) {
  if (v == t) return f;
  used[v] = true;
  for (int i = 0; i < B[v].size(); i++) {
    int to = B[v][i][0], &cap = B[v][i][1], rev = B[v][i][2];
    if (not used[to] and cap > 0) {
      int d = DFS(B, to, t, std::min(f, cap), used);
      if (d > 0) {
        cap -= d;
        if(rev != -1) B[to][rev][1] += d;
        return d;
      }
    }
  }
  return 0;
}

int computeMaximumFlow(BipartiteGraph G, matching &mat,
                       constraint &In,
                       constraint &Out) {
  std::vector<std::vector<std::vector<int> > > B(G.size() + 2);  // to, cap, rev;
  std::vector<bool> matched(G.size(), false);
  int m = G.edgeSize();
  for (int i = 0; i < m; i++) {
    if (In.test(i) == true) {
      std::pair<int, int> e = G.getEdge(i);
      matched[e.first] = matched[e.second] = true;
    }
  }
  for (int i = 0; i < G.leftSize(); i++) {
    if (matched[i]) continue;
    for (int j = 0; j < G[i].size(); j++) {
      int to = G[i][j];
      int id = G.getId(std::pair<int, int>(i, to));
      if (Out.test(id) == false and matched[to] == false) {
        B[i].push_back(std::vector<int>{to, 1, (int)B[to].size()});
        B[to].push_back(std::vector<int>{i, 0, (int)B[i].size() - 1});
      }
    }
  }
  for (int i = 0; i < G.leftSize(); i++) B[G.size()].push_back(std::vector<int>{i, 1, -1});
  for (int i = 0; i < G.rightSize(); i++) B[i + G.leftSize()].push_back(std::vector<int>{G.size() + 1, 1, -1});

  while (1) {
    std::vector<bool> used(B.size(), false);
    int f = DFS(B, B.size() - 2, B.size() - 1, 1e9, used);
    if (f == 0) break;
  }
  mat = In;
  for (int i = 0; i < G.leftSize(); i++) {
    for (int j = 0; j < B[i].size(); j++) {
      // if (B[i][j][0] >= G.size()) continue;
      if (B[i][j][1] == 0) {
        int id = G.getId(std::pair<int, int>(i, B[i][j][0]));
        mat.set(id);
      }
    }
  }
  return mat.count();
}

void EnumerateMaximumMatchings(
    BipartiteGraph &G, int k,
    std::set<matching> &maxmat) {
  std::cout << "enum max mat." << std::endl;
  int m = G.edgeSize();
  std::cout << "m:" << m << std::endl;
  matching mat(m);
  constraint In(m), Out(m);
  int f = computeMaximumFlow(G, mat, In, Out);
  std::queue<std::pair<constraint, constraint> > que;
  que.push(std::pair<constraint, constraint>(In, Out));
  while (not que.empty()) {
    std::cout << "que size: "  << que.size() << std::endl;
    In = que.front().first, Out = que.front().second;
    que.pop();
    int tf = computeMaximumFlow(G, mat, In, Out);
    std::cout << "in : " << In << std::endl;
    std::cout << "out: " << Out << std::endl;
    std::cout << "mat: " << mat << std::endl;    
    if (f > tf) continue;
    if (maxmat.size() >= k) break;
    maxmat.insert(mat);
    for (int i = 0; i < m; i++) {
      if (mat.test(i) == true and In.test(i) == false) {
        Out.set(i);
        que.push(std::pair<constraint, constraint>{In, Out});
        In.set(i);
        Out.reset(i);
      }
    }
  }
}

void KBest(BipartiteGraph &G, int k,
           std::set<matching> &output) {
  std::priority_queue<matching, std::vector<matching>, decltype(compare)> que(compare);
  EnumerateMaximumMatchings(G, k, output);
  std::cout << "output size:" << output.size() << std::endl;
  for(auto MM: output) {
    que.push(MM);
    if(que.size() >= k) return;
  }
  matching m, tmp;
  std::vector<int> matched(G.size());
  int cnt = 0;
  while (not que.empty() and output.size() < k) {
    cnt++;
    if (cnt == 1000000) {
      std::cout << output.size() << std::endl;
      cnt = 0;
    }
    m = que.top();
    que.pop();
    for (int i = 0; i < G.edgeSize(); i++) {
      if (m.test(i) == false) {
        computeMatched(G, m, matched);
        tmp = m;
        int u = G.getEdge(i).first, v = G.getEdge(i).second;
        m.set(i);
        int a[] = {u, v};
        for (int j = 0; j < 2; j++) {
          int w = a[j];
          if (matched[w] != -1) {
            std::pair<int, int> e = G.getEdge(matched[w]);
            m.reset(G.getId(e));
            int z = e.first;
            if (z == w) z = e.second;
            matched[z] = -1;
            for (int l = 0; l < G[z].size(); l++) {
              int to = G[z][l];
              if (matched[to] == -1 and to != u and to != v) {
                int id = G.getId(std::pair<int, int>(z, to));
                matched[to] = matched[z] = id;
                m.set(id);
                break;
              }
            }
          }
        }
        matched[u] = matched[v] = i;
        if (output.find(m) == output.end()) {
          que.push(m);
          output.insert(m);
          if(output.size() >= k) break;
        }
        m = tmp;
      }
    }
  }
  return;
}