///////////////////////////////////////////////////////
/*                                                   */
/* Appximate Enumeration of minimal vecrtex covers   */
/* app. ratio: 3, delay: O(nm) delay                 */
/* writer: kazuhiro kurita                           */
/*                                                   */
///////////////////////////////////////////////////////

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Enum.hpp"
#include "Graph.hpp"
// #define DEBUG

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Error : The number of input file is not 2" << std::endl;
    return 0;
  }
  std::ifstream ist(argv[1], std::ios::in);
  if (!ist) {
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  int n, m, cnt = 0;
  std::string tmp;
  getline(ist, tmp);
  tmp = tmp.substr(5);
  std::cout << tmp << std::endl;
  sscanf(tmp.data(), "%d %d", &n, &m);
  Graph H(n);
  while (getline(ist, tmp)) {
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    u--, v--;
    H[u].push_back(v);
    H[v].push_back(u);
  }
  int k = n / 4;
  std::cout << n << " " << m << " " << k << std::endl;
  AppEnumVC solver(H);
  auto start = std::chrono::system_clock::now();
  solver.AppEnumerate(k);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  std::cout
      << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
      << std::endl;
  solver.printSol();
  return 0;
}