////////////////////////////////////////
/*                                    */
/* Top-k st-paths                     */
/* poly. time and poly. space         */
/* kurita                             */
/*                                    */
////////////////////////////////////////

#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>
#include<random>

#include"Enum.hpp"
// #define DEBUG


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
  int n, m, k, cnt = 0;
  double eps;
  std::mt19937 mt(11); 
  std::string tmp;
  getline(ist, tmp);
  sscanf(tmp.data(), "%d %d %d %lf", &n, &m, &k, &eps);  
  std::vector<std::vector<edge> > G(n);
  while(getline(ist, tmp)){
    int u, v, c;
    // sscanf(tmp.data(), "%d %d %d", &u, &v, &c);
    sscanf(tmp.data(), "%d %d", &u, &v);
    c = (mt()%20)+1;
    G[u].push_back(edge(u, v, c, cnt));
    G[v].push_back(edge(v, u, c, cnt++));
  }
  std::cout << n << " " << m << std::endl;
  EST est(G);
  
  auto start = std::chrono::system_clock::now();
  est.Enumerate(0, n-1, k);
  auto end = std::chrono::system_clock::now();
  auto diff = end - start;
  std::cout << "elapsed time = "
            << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
            << " msec."
            << std::endl;  
  est.print();
  return 0;
}