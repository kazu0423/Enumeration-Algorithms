#ifndef __BPARTITE__
#define __BPARTITE__

#include <vector>
#include <iostream>
#include <utility>

class BipartiteGraph{
public:
  BipartiteGraph(){};
  BipartiteGraph(std::vector<std::vector<int> > H, int l, int r, std::vector<std::vector<int> > ei, std::vector<std::pair<int, int> > ie){
      edge_to_id = ei;
      id_to_edge = ie;
      n = l + r;
      nl = l, nr = r;
      init(H);
    };
  void init(std::vector<std::vector<int> > H);
  inline int edgeSize(){return m;};
  inline int size(){return nr+nl;};
  inline int leftSize(){return nl;};
  inline int rightSize(){return nr;};  
  inline std::vector<int>& operator[](const int id){return G[id];}
  std::pair<int, int> getEdge(int id){return id_to_edge[id];};
  int getId(std::pair<int, int> e){return edge_to_id[e.first][e.second];};
private:
  int n, nl, nr, m;
  std::vector<std::vector<int> > G;
  std::vector<std::vector<int> > edge_to_id;
  std::vector<std::pair<int, int> > id_to_edge;
};
#endif  // __BIPARTITE__