#include "enum.hpp"

#include <boost/dynamic_bitset.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using matching = boost::dynamic_bitset<>;
using constraint = boost::dynamic_bitset<>;
using instance = std::pair<constraint, constraint>;


auto compare = [](matching a, matching b) { return a.count() < b.count(); };


bool checkSolution(BipartiteGraph &G, boost::dynamic_bitset<> &m) {
  int n = G.size();
  std::vector<int> matched(n);

  for (int i = 0; i < G.size(); i++) matched[i] = -1;
  for (int i = 0; i < G.edgeSize(); i++) {
    if (m.test(i) == true) {
      int u = G.getEdge(i).first, v = G.getEdge(i).second;
      if(matched[u] != -1 or matched[v] != -1){
        std::cout << "not matching. " << std::endl;
        std::cout << u << " " << v << std::endl;
        std::pair<int, int> e;
        if(matched[u] != -1) e = G.getEdge(matched[u]);
        if(matched[v] != -1) e = G.getEdge(matched[v]);
        std::cout << e.first << " " << e.second << std::endl;
        std::cout << m << std::endl;
        exit(1);
      }
      matched[u] = matched[v] = i;
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < G[i].size(); j++) {
      if (matched[i] == -1 and matched[G[i][j]] == -1) {
        std::cout << G.edgeSize() << std::endl;
        for (int k = 0; k < G.edgeSize(); k++) {
          if (m.test(k)) {
            std::cout << "{" << G.getEdge(k).first << " " << G.getEdge(k).second
                      << "}" << std::endl;
          }
        }
        std::cout << "Edges:" << std::endl;
        for (int k = 0; k < G.edgeSize(); k++) {
          std::cout << "{" << G.getEdge(k).first << " " << G.getEdge(k).second
                    << "}" << std::endl;
        }
        std::cout << i << " " << G[i][j] << std::endl;
        std::cout << m << std::endl;
        return false;
      }
    }
  }
  return true;
}


void Matched(BipartiteGraph &G, matching &m, std::vector<int> &matched) {
  for (int i = 0; i < G.size(); i++) matched[i] = -1;
  for (int i = 0; i < G.edgeSize(); i++) {
    if (m.test(i) == true) {
      int u = G.getEdge(i).first, v = G.getEdge(i).second;
      if(matched[u] != -1 or matched[v] != -1){
        std::cout << "not matching. " << std::endl;
        std::cout << u << " " << v << std::endl;
        std::pair<int, int> e;
        if(matched[u] != -1) e = G.getEdge(matched[u]);
        if(matched[v] != -1) e = G.getEdge(matched[v]);
        std::cout << e.first << " " << e.second << std::endl;
        std::cout << m << std::endl;
        exit(1);
      }
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

int MaximumFlow(BipartiteGraph G, matching &mat, constraint &In, constraint &Out) {
  std::vector<std::vector<std::vector<int> > > B(G.size() + 2);  // to, cap, rev;
  std::vector<bool> matched(G.size(), false);
  int m = G.edgeSize();
  // std::cout << "Max. Flow" << std::endl;
  // std::cout << "In     :" << In  << std::endl;
  // std::cout << "Out    :" << Out << std::endl;
  // std::cout << "Graph G:" << std::endl;
  // for(int i = 0; i < G.leftSize(); i++) {
  //   std::cout << i << ": ";
  //   for(int j = 0; j < G[i].size(); j++) {
  //     std::cout << G[i][j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
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

  // std::cout << "Graph B:" << std::endl;
  // for(int i = 0; i < G.leftSize(); i++) {
  //   std::cout << i << ": ";
  //   for(int j = 0; j < B[i].size(); j++) {
  //     std::cout << B[i][j][0] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  for (int i = 0; i < G.leftSize(); i++){
    if(matched[i]) continue;
    B[G.size()].push_back(std::vector<int>{i, 1, -1});
  }

  for (int i = G.leftSize(); i < G.size(); i++){
    if(matched[i]) continue;
    B[i].push_back(std::vector<int>{G.size() + 1, 1, -1});
  }
  while (1) {
    std::vector<bool> used(B.size(), false);
    int f = DFS(B, B.size() - 2, B.size() - 1, 1e9, used);
    if (f == 0) break;
  }
  mat = In;
  for (int i = 0; i < G.leftSize(); i++) {
    for (int j = 0; j < B[i].size(); j++) {
      if (B[i][j][1] == 0) mat.set(G.getId(std::pair<int, int>(i, B[i][j][0])));
    }
  }
  return mat.count();
}

void EnumerateMaximumMatchings(BipartiteGraph &G, int k,
                               std::set<matching> &maxmat) {
  int m = G.edgeSize();
  matching mat(m);
  constraint In(m), Out(m);
  // std::cout << "in : " << In << std::endl;
  // std::cout << "out: " << Out << std::endl;
  // std::cout << "k  : " << k << std::endl;
  int f = MaximumFlow(G, mat, In, Out);
  std::queue<instance> que;
  que.push(instance(In, Out));
  while (not que.empty()) {
    In = que.front().first, Out = que.front().second;
    que.pop();
    MaximumFlow(G, mat, In, Out);
    if (f > mat.count()) continue;
    maxmat.insert(mat);
    if (maxmat.size() >= k) break;
    for (int i = 0; i < m; i++) {
      if (mat.test(i) == true and In.test(i) == false and Out.test(i) == false) {
        Out.set(i);
        // std::cout << "in : " << In << std::endl;
        // std::cout << "out: " << Out << std::endl;
        // std::cout << "mat: " << mat << std::endl;
        // std::cout << "que size:" << que.size()  << std::endl << std::endl; 
        que.push(instance(In, Out));
        Out.reset(i);
        In.set(i);
      }
    }
  }
}

void KBest(BipartiteGraph &G, int k, std::set<matching> &output) {
  std::priority_queue<matching, std::vector<matching>, decltype(compare)> que(compare);
  EnumerateMaximumMatchings(G, k, output);
  // std::cout << "output size:" << output.size() << std::endl;
  matching m, tmp;
  std::vector<int> matched(G.size());
  m = *output.begin();
  std::cout << "A seed maximum matching, maching size: " << m.count() << ", " << m << std::endl;  
  for (auto MM : output) {
    que.push(MM);
    m = MM;
    Matched(G, m, matched);
    if(not checkSolution(G, m)) {
      std::cout << "error." << std::endl;
      exit(1);
    }
    if (que.size() >= k) return;
  }
  int cnt = 0;
  std::cout << "max. mat size:" << que.size() << std::endl;
  while (not que.empty() and output.size() < k) {
    cnt++;
    if (cnt == 1000000) {
      std::cout << output.size() << std::endl;
      cnt = 0;
    }
    m = que.top();
    // std::cout << m.count() << std::endl;
    que.pop();
    for (int i = 0; i < G.edgeSize(); i++) {
      if (m.test(i) == false) {
        Matched(G, m, matched);
        tmp = m;
        int u = G.getEdge(i).first, v = G.getEdge(i).second;
        m.set(i);
        std::vector<int> a = std::vector<int>{u, v};
        for (int w: a) { 
          if(matched[w] == -1) continue;
          std::pair<int, int> e = G.getEdge(matched[w]);
          // std::cout << "mat: " << matched[w] << std::endl;
          m.reset(matched[w]);
          int z = e.first;
          if (z == w) z = e.second;
          matched[z] = -1;
          for (int j = 0; j < G[z].size(); j++) {
            int to = G[z][j];
            if (matched[to] == -1 and to != u and to != v) {
              int id = G.getId(std::pair<int, int>(z, to));
              matched[to] = matched[z] = id;
              m.set(id);
              break;
            }
          }
        }
        matched[u] = matched[v] = i;
        if (output.find(m) == output.end()) {
          que.push(m);
          output.insert(m);
          if(not checkSolution(G, m)) {
            std::cout << "error." << std::endl;
            exit(1);
          }
          if (output.size() >= k) break;
        }
        m = tmp;
      }
    }
  }
  return;
}

void EnumerateLMM(BipartiteGraph &G, int k,
                  std::set<boost::dynamic_bitset<> > &output){
  matching mat(G.edgeSize());
  constraint In(G.edgeSize()), Out(G.edgeSize());
  int f = MaximumFlow(G, mat, In, Out);
  matching m, tmp;
  std::vector<int> matched(G.size());
  int cnt = 0;
  std::queue<matching> que;
  que.push(mat);
  output.insert(mat);
  std::cout << "A seed maximum matching: " << mat << std::endl;
  while (not que.empty()) {
    cnt++;
    if (cnt == 1000000) {
      std::cout << output.size() << std::endl;
      cnt = 0;
    }
    m = que.front();
    que.pop();
    for (int i = 0; i < G.edgeSize(); i++) {
      if (m.test(i) == false) {
        Matched(G, m, matched);
        tmp = m;
        int u = G.getEdge(i).first, v = G.getEdge(i).second;
        // std::cout << "add: " << u << " " << v << std::endl;
        m.set(i);
        std::vector<int> a = std::vector<int>{u, v};
        for (int w: a) {
          // std::cout << "w: " << w << std::endl;
          if (matched[w] == -1) continue;
          std::pair<int, int> e = G.getEdge(matched[w]);
          m.reset(matched[w]);
          int z = e.first;
          if (z == w) z = e.second;
          matched[z] = -1;
          for (int j = 0; j < G[z].size(); j++) {
            int to = G[z][j];
            if (matched[to] == -1 and to != u and to != v) {
              int id = G.getId(std::pair<int, int>(z, to));
              matched[to] = matched[z] = id;
              m.set(id);
              break;
            }
          }
        }
        matched[u] = matched[v] = i;
        if (output.find(m) == output.end() and m.count() >= k) {
          que.push(m);
          output.insert(m);
        }
        m = tmp;
      }
    }
  }
  return;
}
