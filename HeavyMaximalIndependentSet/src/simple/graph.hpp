#ifndef __GRAPH__
#define __GRAPH__

#include <vector>
#include <iostream>
#include <utility>

class VertexWeightedGraph{
public:
  VertexWeightedGraph(){};
  VertexWeightedGraph(std::vector<std::vector<int> > H, std::vector<int> weight){
      init(H);
      w = weight;
    };
  void init(std::vector<std::vector<int> > H);
  inline int edge_size(){return m;};
  inline int size(){return n;};
  inline int getWeight(int pos){return w[pos];}
  inline std::vector<int>& operator[](const int id){return G[id];}
private:
  int n, m;
  std::vector<std::vector<int> > G;
  std::vector<int> w;
};
#endif  // __GRAPH__