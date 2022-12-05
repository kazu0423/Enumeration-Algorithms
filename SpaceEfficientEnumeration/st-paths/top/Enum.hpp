#ifndef __ENUM__
#define __ENUM__
#include<memory>
#include<queue>
#include<stack>
#include<chrono>

typedef class instance{
public:
  instance(int size){
    I.resize(size, 0), O.resize(size, 0);
  };
  instance(int a, int b, int size, int v){
    s = a, t = b;
    I.resize(size, 0), O.resize(size, 0);
    val = v;
  };
  bool operator<(const instance &I) const {
    return val < I.val;
  };
  bool operator>(const instance &I) const {
    return val > I.val;
  };
  int s, t;
  std::vector<bool> I, O;
  int val;
}instance;

using bigint = long long int;

typedef class edge{
public:
  edge(int f, int t, int c, int i):from(f), to(t), cost(c), id(i){};
  edge(){};
  int from, to, cost, id;
}edge;

class EST{
public:
  EST(std::vector<std::vector<edge> > H);
  ~EST(){};
  inline int size(){return G.size();}
  void Enumerate(int s, int t, int k);
  void print();
private:
  void FindShortestPath(instance &ins, std::vector<int> &res, int &len);
  int ComputeShortestPathLength(std::vector<bool> &I, std::vector<bool> &O, int s, int t);
  std::vector<std::vector<edge> > G;
  std::vector<edge> elist;
  int n, m, max_que_size = 0;
  std::vector<std::vector<bool> > ans;
};
#endif // __ENUM__
