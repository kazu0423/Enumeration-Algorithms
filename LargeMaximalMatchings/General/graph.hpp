#ifndef __GRAPH__
#define __GRAPH__

#include <vector>
#include <iostream>
#include <utility>

class Graph{
public:
  Graph(){};
  Graph(std::vector<std::vector<int> > H, std::vector<std::vector<int> > ei, std::vector<std::pair<int, int> > ie){
      edge_to_id = ei;
      id_to_edge = ie;
      init(H);
    };
  void init(std::vector<std::vector<int> > H);
  inline int edge_size(){return m;};
  inline int size(){return n;};
  inline std::vector<int>& operator[](const int id){return G[id];}
  std::pair<int, int> getEdge(int id){return id_to_edge[id];};
  int getId(std::pair<int, int> e){return edge_to_id[e.first][e.second];};
private:
  int n, m;
  std::vector<std::vector<int> > G;
  std::vector<std::vector<int> > edge_to_id;
  std::vector<std::pair<int, int> > id_to_edge;
};
#endif  // __GRAPH__