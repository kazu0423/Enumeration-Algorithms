#include<iostream>
#include<vector>
#include<memory>
#include<algorithm>
#include"Enum.hpp"

using bigint = long long int;

BronKerbosch::BronKerbosch(std::vector<std::vector<int> > H){
  n = H.size();
  G = H;
  std::vector<int> tmp(n);
  for (int i = 0; i < n; i++) tmp[i] = i;
  solution.init(tmp);
  forbidden.init(tmp);
  cand.init(tmp);
}

bigint BronKerbosch::Enumerate(){
  std::cout << "dep:" << std::endl;
  if(forbidden.empty() and cand.empty()) {
    for (int i = solution.begin(); i != solution.end(); i = solution.GetNext(i)) {
      std::cout << solution[i] << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    return 1; 
  }
  bigint res = 0, c_size = cand.size(), f_size = forbidden.size();
  for (int i = cand.begin(); i != cand.end(); i = cand.GetNext(i)) {
    std::cout << "I:" << i << std::endl;
    int c_cnt = 0, f_cnt = 0;
    solution.add(i);
    cand.remove(i);
    for (int j = 0; j < G[i].size(); j++) {
      c_cnt += cand.member(G[i][j]);
      cand.remove(G[i][j]);
      f_cnt += forbidden.member(G[i][j]);
      forbidden.remove(G[i][j]);
    }
    res += Enumerate();
    solution.undo();
    for (int j = 0; j < c_cnt; j++) cand.undo();
    forbidden.add(i);
    i = cand.remove(i);
  }
  for (int i = 0; i < c_size; i++) cand.undo();
  while (f_size != forbidden.size()) forbidden.undo();
  return res;
}
