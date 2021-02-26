/////////////////////////////////////////
/*                                     */
/* Enumeration Large Maximal Matchings */
/* O(nm) delay                         */
/* kurita                              */
/*                                     */
/////////////////////////////////////////

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "enum.hpp"
#include "graph.hpp"

typedef boost::adjacency_list<
    boost::listS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_name_t, std::string> >
    GraphB;  // boost用のグラフ

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
  sscanf(tmp.data(), "%d %d", &n, &m);

  GraphB GB(n);
  std::vector<std::vector<int> > H(n);
  std::vector<std::vector<int> > e_to_id(n);
  std::vector<std::pair<int, int> > id_to_e;
  while (getline(ist, tmp)) {
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    u--, v--;
    boost::add_edge(u, v, TG);
    H[u].push_back(v);
    H[v].push_back(u);
    e_to_id[u][v] = e_to_id[v][u] = id++;
    id_to_e.push_back(std::pair<int, int>(u, v));
  }

  Graph G(H, e_to_id, id_to_e);
  boost::dynamic_bitset<> matching(m);
  std::vector<boost::graph_traits<GraphB>::vertex_descriptor> mate(n);
  bool success = checked_edmonds_maximum_cardinality_matching(GB, &mate[0]);
  assert(success);
  boost::graph_traits<GraphB>::vertex_iterator vi, vi_end;
  for (boost::tie(vi, vi_end) = vertices(GB); vi != vi_end; ++vi) {
    if (mate[*vi] != boost::graph_traits<GraphB>::null_vertex() &&
        *vi < mate[*vi]) {
      std::cout << "{" << *vi << ", " << mate[*vi] << "}" << std::endl;
      // std::cout <<  e_to_id[*vi][mate[*vi]] << std::endl;
      matching.set(e_to_id[*vi][mate[*vi]]);
    }
  }

  std::cout << "MM size:" << matching.count() << ", " << matching << std::endl;
  auto start = std::chrono::system_clock::now();
  std::set<boost::dynamic_bitset<> > LMM;
  Enumerate(G, matching, k, LMM);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  printf("%lld\n",
         std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
  std::cout << "the number of large matchings:" << LMM.size() << std::endl;
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
