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
  instance(int size, int v){
    I.resize(size, 0), O.resize(size, 0);
    val = v;
  };
  bool operator<(const instance &I) const {
    return val < I.val;
  };
  bool operator>(const instance &I) const {
    return val > I.val;
  };
  std::vector<bool> I, O;
  int val;
}instance;


typedef class edge{
public:
  edge(int f, int t, int i):from(f), to(t), id(i){};
  edge(){};
  int from, to, id;
}edge;

class EVC{
public:
  EVC(std::vector<std::vector<edge> > H, std::vector<int>  vcost);
  ~EVC(){};
  inline int size(){return G.size();}
  void Enumerate(int k, double eps);
  void print();
private:
  int FindAppMinVC(instance ins, std::vector<bool> &res, bool flag = false);
  int DFS(instance ins, int k, double mini, double maxi, bool output = false);
  bool checkVC(std::vector<bool> VC);
  std::vector<std::vector<edge> > G;
  std::vector<edge> elist;
  std::vector<int> cost;
  int n, m, test = 0;;
  std::vector<std::vector<bool> > ans;
};
#endif // __ENUM__
