#ifndef __ENUM__
#define __ENUM__
#include<memory>
#include<queue>
#include<stack>
#include<chrono>
#include"basicDataStructure.hpp"

using bigint = long long int;

class BronKerbosch{
public:
  BronKerbosch(std::vector<std::vector<int> > H);
  bigint Enumerate();
  void print(){};
private:
  int n, m;
  std::vector<std::vector<int> > G;
  List<int> cand; 
  AddibleList<int> forbidden, solution;
};
#endif // __ENUM__







