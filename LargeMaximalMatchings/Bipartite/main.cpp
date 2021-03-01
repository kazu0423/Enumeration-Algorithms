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
#include "bipartite.hpp"

int main(int argc, char *argv[]) {
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
  H.resize(n);
  e_to_id.resize(n, std::vector<int>(n, -1));
  for (int i = 0; i < X.size(); i++) {
    int u = X[i], v = Y[i] + Xmax + 1;
    if(e_to_id[u][v] != -1)continue;
    e_to_id[u][v] = e_to_id[v][u] = id++;
    H[u].push_back(v);
    H[v].push_back(u);
    id_to_e.push_back(std::pair<int, int>(u, v));
  }
  m = id_to_e.size();
  std::cout << n << " " << m << " " << Xmax << std::endl;
  BipartiteGraph G(H, Xmax + 1, Ymax + 1, e_to_id, id_to_e);  
  auto start = std::chrono::system_clock::now();
  std::set<boost::dynamic_bitset<> > LMM;
  if(std::atoi(argv[3]) == 0){
    std::cout << "Solve " << k << "-best solutions. " << std::endl;
    KBest(G, k, LMM);
  }else{
    std::cout << "Enumerate maximal matchings with cardinality at least " << k << "." << std::endl;    
    EnumerateLMM(G, k, LMM);
  }
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  printf("time: %lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
         
  std::vector<int> distribution(n/2+1);
  for (auto s : LMM) {
    if (not checkSolution(G, s)) {
      std::cerr << "Error: Non-maximal solution. " << std::endl;
      return 0;
    }
    distribution[s.count()]++;
    // std::cout << s.count() << ":" << s << std::endl;
  }
  for (int i = 0; i < distribution.size(); i++) {
    std::cout << std::setw(2) << i << ": " << distribution[i] << std::endl;
  }
  std::cout << "total MM:" << LMM.size() << std::endl;
  return 0;
}
