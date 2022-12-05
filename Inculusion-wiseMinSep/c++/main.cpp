///////////////////////////////////////////////////////
/*                                                   */
/* Enumeration of inclusion-wise minimal separators  */
/* ??? delay                                         */
/* kurita                                            */
/*                                                   */
///////////////////////////////////////////////////////

#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>

#include"Enum.hpp"
using bigint = unsigned long long int;

int main(int argc, char *argv[]){
  if(argc != 2){
    std::cerr << "Error : The number of input file is not 2" <<std::endl;
    return 0;
  }
  
  std::ifstream ist(argv[1], std::ios::in);
  if(!ist){
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  int n, m, cnt = 0;
  std::string tmp;
  getline(ist, tmp);
  tmp = tmp.substr(6);
  sscanf(tmp.data(), "%d %d", &n, &m);
  std::vector<std::vector<int> > G(n);
  while(getline(ist, tmp) and m > 0){
    m--;
    int u, v;
    sscanf(tmp.data(), "%d %d", &u, &v);
    u--, v--;
    G[u].push_back(v);
    G[v].push_back(u);    
  }
  std::cout << n << " " << m << std::endl;
  EIMS eims = EIMS(G);
  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  eims.Enumerate();
  auto diff = end - start;
  std::cout << "elapsed time = "
            << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
            << " msec."
            << std::endl; 
  eims.print();
  // std::cout << "time:"
  //           << std::chrono::duration_cast<std::chrono::milliseconds>(eds.time).count()
  //           << std::endl;
  return 0;
}





