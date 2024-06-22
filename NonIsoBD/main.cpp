///////////////////////////////////////////////////////////////////////////
/*                                                                       */
/* Enumerating all non-isomorphic graphs with degree either one or four  */
/* Poly-delay (in theory)                                                */
/* kurita                                                                */
/*                                                                       */
///////////////////////////////////////////////////////////////////////////

#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>
// #define DEBUG

#include"Enum.hpp"
using bigint = long long int;

int main(int argc, char *argv[]){
  int n, c, cnt = 0;
  std::cout << "hoge" << std::endl;
  std::cin >> n >> c;
  std::cout << n << " " << c << std::endl;
  if((n-c)&1){
    std::cerr << "Error : There is no such graphs" <<std::endl;
    return 0;
  }
  NonIsoGraphs NIG;
  NIG.init(n, c);
  NIG.Enumerate();
  NIG.print();
  // auto start = std::chrono::system_clock::now();
  // eds.Enumerate();
  // auto end = std::chrono::system_clock::now();
  // auto diff = end - start;
  // std::cout << "elapsed time = "
  //           << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
  //           << " msec."
  //           << std::endl;  
  // eds.print();
  return 0;
}







