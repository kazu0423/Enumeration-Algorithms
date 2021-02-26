#include <vector>
#include "bipartite.hpp"

void BipartiteGraph::init(std::vector<std::vector<int> > H){
  n = H.size(), m = 0;
  for(int i = 0; i < n; i++) m += H[i].size();
  m /= 2;
  G.resize(n);
    int id = 0;
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < H[i].size(); j++) {
      int to = H[i][j];
      if(i < to)continue;
      G[i].push_back(to);
      G[to].push_back(i);
    }
  }
}