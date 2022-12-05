#ifndef __ENUM__
#define __ENUM__
#include<memory>
#include<queue>
#include<set>
#include<stack>
#include<chrono>
using bigint = unsigned long long int;
using MS = std::vector<bigint>; // 大体500頂点くらいが最大なので，長さは固定長9に設定する

class EIMS{
public:
  EIMS(std::vector<std::vector<int> > H);
  ~EIMS(){};
  inline int size(){return G.size();}
  void Enumerate();
  void print();
private:
  void minimization(std::vector<int> &separator);
  bool is_supersep(std::vector<int> &separator);
  void connected_component(std::vector<int> &separator);
  std::vector<std::vector<int> > G;
  std::set<std::vector<int> > ans;
  std::queue<int> que;
  std::vector<int> cc;
  int n;
};
#endif // __ENUM__