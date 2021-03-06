#ifndef __ENUM__
#define __ENUM__
#include<memory>
#include<queue>
#include<stack>
#include<chrono>

#include"List.hpp"
#include"basicDataStructure.hpp"
using bigint = long long int;
using SmallestLastOrdering = std::vector<int>;

class EIS{
public:
  EIS(std::vector<std::vector<int> > H);
  ~EIS(){};
  inline int size(){return n;}
  void Enumerate();
  void print();
private:
  void RecEnum();
  void ComputeSLO();
  std::vector<std::vector<int> > G;
  SmallestLastOrdering SLO; 
  List<int> cand;
  FixedStack<int> solution;
  // std::vector<bigint> ans;
  bigint ans = 0;
  std::vector<int> deg;
  int n, tail = 0, v, maxi, mini;
};
#endif // __ENUM__
