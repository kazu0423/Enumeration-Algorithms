#include <vector>
#include "bipartite.hpp"

void BipartiteGraph::init(std::vector<std::vector<int> > H){
  m = 0;
  for(int i = 0; i < n; i++) m += H[i].size();
  m /= 2;
  G.resize(n);
  // std::cout << "n:" << n << std::endl;
  for(int i = 0; i < nl; i++) {
    for(int j = 0; j < H[i].size(); j++) {
      int to = H[i][j];
      // std::cout  << i << " " << to << std::endl;
      G[i].push_back(to);
      G[to].push_back(i);
    }
  }
}