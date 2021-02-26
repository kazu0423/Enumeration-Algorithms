/////////////////////////////////////////
/*                                     */
/* Enumeration Large Maximal Matchings */
/* O(nm) delay                         */
/* kurita                              */
/*                                     */
/////////////////////////////////////////

#include <boost/dynamic_bitset.hpp>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "enum.hpp"
#include "graph.hpp"

void func(Graph &G, boost::dynamic_bitset<> &m, std::vector<int> &matched) {
  for (int i = 0; i < G.size(); i++) matched[i] = -1;
  for (int i = 0; i < G.edge_size(); i++) {
    if (m.test(i) == true) {
      int u = G.getEdge(i).first, v = G.getEdge(i).second;
      matched[u] = matched[v] = i;
    }
  }
}

bool checkMaximality(Graph &G, boost::dynamic_bitset<> &m) {
  int n = G.size();
  std::vector<int> matched(n);
  func(G, m, matched);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < G[i].size(); j++) {
      if (matched[i] == -1 and matched[G[i][j]] == -1) {
        std::cout << G.edge_size() << std::endl;
        for (int k = 0; k < G.edge_size(); k++) {
          if (m.test(k)) {
            std::cout << "{" << G.getEdge(k).first << " " << G.getEdge(k).second
                      << "}" << std::endl;
          }
        }
        std::cout << "Edges:" << std::endl;
        for (int k = 0; k < G.edge_size(); k++) {
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

int main(int argc, char *argv[]) {
  if (argc != 3 and argc != 4) {
    std::cerr << "Error : The number of input file is " << argc << std::endl;
    return 0;
  }

  std::ifstream ist(argv[1], std::ios::in);
  if (!ist) {
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }

  int n, m = 0, id = 0, k = std::atoi(argv[2]);
  std::string tmp;
  getline(ist, tmp);

  std::vector<std::vector<int> > H;
  std::vector<std::vector<int> > e_to_id;
  std::vector<std::pair<int, int> > id_to_e;
  std::vector<int> X, Y;
  int Xmax = 0, Ymax = 0;
  while (getline(ist, tmp)) {
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    X.push_back(--u);
    Y.push_back(--v);
    Xmax = std::max(u, Xmax);
    Ymax = std::max(v, Ymax);
  }
  n = Xmax + Ymax + 2;
  for (int i = 0; i < Y.size(); i++) Y[i] += Xmax + 1;
  H.resize(n);
  e_to_id.resize(n, std::vector<int>(n, -1));
  for (int i = 0; i < X.size(); i++) {
    int u = X[i], v = Y[i];
    H[u].push_back(v);
    H[v].push_back(u);
    e_to_id[u][v] = e_to_id[v][u] = id++;
    id_to_e.push_back(std::pair<int, int>(u, v));
  }
  m = id_to_e.size();

  Graph G(H, e_to_id, id_to_e);
  boost::dynamic_bitset<> matching(m);

  auto start = std::chrono::system_clock::now();
  std::set<boost::dynamic_bitset<> > LMM;
  Enumerate(G, matching, k, LMM);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  printf("%lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
         
  std::vector<int> distribution(n / 2 + 1);
  for (auto s : LMM) {
    if (not checkMaximality(G, s)) {
      std::cerr << "Error: Non-maximal solution. " << std::endl;
      return 0;
    }
    distribution[s.count()]++;
    // std::cout << s << std::endl;
  }
  for (int i = 0; i < distribution.size(); i++) {
    std::cout << std::setw(2) << i << ": " << distribution[i] << std::endl;
  }
  return 0;
}