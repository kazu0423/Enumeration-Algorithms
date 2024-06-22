#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>
#include<sstream>

#include"Graph.hpp"
#include"Enum.hpp"
// #define DEBUG

using bigint = long long int;

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
  int n, m, s, t, len = 0, cnt = 0;
  std::string tmp, st;
  getline(ist, tmp);
  getline(ist, tmp);
  std::istringstream iss(tmp);  
  iss >> st >> st >> n >> m;
  std::vector<std::vector<edge> > _G(n);
  for(int i = 0; i < m; i++) {
    int u, v;
    getline(ist, tmp);
    iss = std::istringstream(tmp);
    iss >> st >> u >> v;
    u--, v--;
    _G[u].push_back(edge(u, v, cnt));
    _G[v].push_back(edge(v, u, cnt++));
  }
  getline(ist, tmp);
  iss = std::istringstream(tmp);  
  iss >> st >> len;
  // std::cout << "vertices:" << n << " edges:" << m << " length:" << len << std::endl;
  getline(ist, tmp);
  iss = std::istringstream(tmp);  
  iss >> st >> s >> t;
  ESTP estp(_G);
  // auto start = std::chrono::system_clock::now();
  estp.Enumerate(s-1, t-1, len);
  // auto end = std::chrono::system_clock::now();
  // auto diff = end - start;
  // std::cout << "elapsed time = "
  //           << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
  //           << " msec."
  //           << std::endl;  
  estp.print();
  return 0;
}
