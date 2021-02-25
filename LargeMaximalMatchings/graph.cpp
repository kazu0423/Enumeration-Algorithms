#include <vector>
#include "graph.hpp"

void Graph::init(std::vector<std::vector<int> > H){
  n = H.size(), m = 0;
  for(int i = 0; i < n; i++) m += H[i].size();
  m /= 2;
  G.resize(n);
  // edge_to_id.resize(n);
  // for(int i = 0; i < n; i++) edge_to_id[i].resize(n);
    int id = 0;
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < H[i].size(); j++) {
      int to = H[i][j];
      if(i < to)continue;
      G[i].push_back(to);
      G[to].push_back(i);
      // edge_to_id[i][to] = edge_to_id[to][i] = id++;
      // id_to_edge.push_back(std::pair<int, int>(i, to));
    }
  }

  // for(int i = 0; i < n; i++) {
  //   for(int j = 0; j < G[i].size(); j++) {
  //     std::cout << G[i][j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
}