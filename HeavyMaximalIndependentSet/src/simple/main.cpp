/////////////////////////////////////////////////////////////////////////
/*                                                                     */
/* Heavy maximal independent set enumeration in (k+1)-claw free graphs */
/* poly. delay and k-approximaition order                              */
/* kurita                                                              */
/*                                                                     */
////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "enum.hpp"
#include "graph.hpp"

std::vector<int> read_weight(std::string input){
  std::vector<int> res;
  int n = input.length();
  int prev = 0;
  // std::cout << input << std::endl;
  for(int i = 0; i < n; i++) {
    if(input[i] == ' ') {
      res.push_back(std::atoi(input.substr(prev, i-prev).c_str()));
      prev = ++i;
    }
  }
  res.push_back(std::atoi(input.substr(prev, n-prev).c_str()));
  return res;
}


int main(int argc, char *argv[]) {
  if (argc != 2 and argc != 3) {
    std::cerr << "Error : The number of input file is " << argc << std::endl;
    return 0;
  }

  std::ifstream ist(argv[1], std::ios::in);
  if (!ist) {
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  int n, m = 0, id = 0, k = 1000;
  if(argc == 3) k = std::atoi(argv[2]);
  std::cerr << "k:" << k << std::endl;
  std::string tmp;

  getline(ist, tmp);
    sscanf(tmp.data(), "%d %d", &n, &m);
  getline(ist, tmp);
  std::vector<std::vector<int> > H(n);
  std::vector<int> w = read_weight(tmp);
  std::vector<std::pair<int, int> > sort(n);
  for(int i = 0; i < n; i++) sort[i] = std::pair<int, int>(w[i], i);
  std::sort(sort.begin(), sort.end());
  std::reverse(sort.begin(), sort.end());
  // for(int i = 0; i < n; i++) {
  //   std::cout << sort[i].first << " " << sort[i].second << std::endl;
  // }
  std::vector<int> new_id(n);
  for(int i = 0; i < n; i++) new_id[sort[i].second] = i;
  while (getline(ist, tmp)) {
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    u--, v--;
    H[new_id[u]].push_back(new_id[v]);
    H[new_id[v]].push_back(new_id[u]);
  }
  for(int i = 0; i < n; i++) w[i] = sort[i].first;
  VertexWeightedGraph G(H, w);

  auto start = std::chrono::system_clock::now();
  Enumerate(G, k);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  // printf("%lld\n", std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
  // std::vector<int> distribution(n / 2 + 1);
  // for (auto s : LMM) {
  //   if (not checkMaximality(G, s)) {
  //     std::cerr << "Error: Non-maximal solution. " << std::endl;
  //     return 0;
  //   }
  //   distribution[s.count()]++;
  //   // std::cout << s << std::endl;
  // }
  // for (int i = 0; i < distribution.size(); i++) {
  //   std::cout << std::setw(2) << i << ": " << distribution[i] << std::endl;
  // }
  return 0;
}
