////////////////////////////////////////
/*                                    */
/* Enumeration of independnet set     */
/* O(1) time per solution             */
/* constraint: \omega(G) is constant  */
/* kurita                             */
/*                                    */
////////////////////////////////////////

#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>

#include"Enum.hpp"
// #define DEBUG

using bigint = long long int;

int main(int argc, char *argv[]){
  if(argc != 3){
    std::cerr << "Error : The number of input file is not 2" <<std::endl;
    return 0;
  }
  int k = std::atoi(argv[1]);
  std::ifstream ist(argv[2], std::ios::in);
  if(!ist){
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  int n, m, cnt = 0;
  std::string tmp;
  getline(ist, tmp);
  sscanf(tmp.data(), "%d %d", &n, &m);
  std::vector<std::vector<int> > G(n, std::vector<int>(n, 0));
  while(getline(ist, tmp)){
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    G[u][v] = G[v][u] = 1;
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if(i == j)continue;
      G[i][j] = (G[i][j] != 1);
    }
  }
  // PARALLEL_EIS eis(G);
  EIS eis(G);
  
  auto start = std::chrono::system_clock::now();
  eis.Enumerate(k);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  std::cout << "elapsed time = "
            << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
            << " msec."
            << std::endl;  
  eis.print();
  return 0;
}





