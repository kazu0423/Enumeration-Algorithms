#ifndef __GRAPH__
#define __GRAPH__
#include<vector>

#include"List.hpp"
#include"Element.hpp"

using pii = std::pair<int, int>;

class Graph{
public:
  Graph(){};
  Graph(std::vector<std::vector<edge> > _G){init(_G);};
  void init(std::vector<std::vector<edge> > _G);
  inline int size(){return n;}
  inline int edgeSize(){return current_edge_size;}
  inline List<edge>& operator[](const int id){return G[id];}
  int RemoveEdge(int id, int x = -1);
  int RemoveVertex(int id);
  void undo();
  inline int begin(){return vlist.GetNext(n);}
  inline int end(){return n;}
  inline int GetNext(int id){return vlist.GetNext(id);};
  inline int GetPrev(int id){return vlist.GetPrev(id);};
  inline int GetDeg(int id){return G[id].size();};
  inline edge GetEdge(int id){return elist[id];};
  void print();
private:
  int n, m, current_edge_size = 0, head;
  std::vector<List<edge> > G;
  List<int> vlist;
  List<edge> elist;
  std::vector<pii> edge2vertex;//u: first, v:second
  std::vector<int> next;
};

#endif // __GRAPH__
