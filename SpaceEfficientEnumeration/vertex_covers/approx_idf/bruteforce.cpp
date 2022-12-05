////////////////////////////////////////
/*                                    */
/* 2-approximate top-k vertex covers  */
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

bool checkVC(std::vector<std::vector<int> > &G, std::vector<bool> &VC){
    for(int i = 0; i < G.size(); i++) {
        if(VC[i])continue;
        for(int j = 0; j < G[i].size(); j++) {
            if(VC[G[i][j]] == false)return false;
        }
    }
    return true;
}

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
  std::vector<std::vector<int> > G(n);
  while(getline(ist, tmp)){
    int u, v, c;
    // sscanf(tmp.data(), "%d %d %d", &u, &v, &c);
    sscanf(tmp.data(), "%d %d", &u, &v);
    c = (mt()%20)+1;
    G[u].push_back(v);
    G[v].push_back(u);
  }
  std::vector<int> cost(n);
  for(int i = 0; i < n; i++) cost[i] = (mt()%20)+1;
  std::cout << n << " " << m << std::endl;
  std::vector<bool> VC(n);
  int res = 0, maxi = 0;
  std::vector<std::vector<bool> > ans;
  for(int i = 0; i < (1<<n); i++) {
      for(int j = 0; j < n; j++) {
          if((i&(1<<j))) VC[j] = true;
          else VC[j] = false;
      }
      if(checkVC(G, VC)){
        res++;
        int tmp = 0;
        for(int j = 0; j < n; j++) tmp += cost[j]*VC[j];
        ans.push_back(VC);
        maxi = std::max(maxi, tmp);
      }
  }
  sort(ans.begin(), ans.end());
  for(int i = 0; i < ans.size(); i++) {
    for(int j = 0; j < n; j++) {
      std::cout << ans[i][j];
    }
    std::cout << std::endl;
  }
  std::cout << res << " " << maxi << std::endl;
  return 0;
}