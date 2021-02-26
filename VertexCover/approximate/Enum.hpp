#ifndef __ENUM__
#define __ENUM__
#include<vector>
#include<iostream>
#include<set>

#include"Graph.hpp"

class AppEnumVC{
private:
  void AppVC();
  void minimize();
  bool checkVC();
  void printCurrentSol();
public:  
  AppEnumVC (Graph H):G(H),n(H.size()){};
  void AppEnumerate(int k);
  void printSol();
  int n;
  Graph G;
  std::vector<int> current_vc, tmp;
  std::set<std::vector<int> > Outputs;
  int vc_size, ans_size;
};

#endif // __ENUM__
