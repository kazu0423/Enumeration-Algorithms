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
  int n = 0, m, s, t, len = 0, cnt = 0;
  char c;
  std::string tmp, tt;
  std::vector<std::pair<int, int> > edges;
  while(getline(std::cin, tmp)){
    int u, v;
    // std::cout << tmp << std::endl;
    if(tmp[0] == 'c'){
      continue;
    }else if(tmp[0] == 'e'){
        sscanf(tmp.data(), "%c %d %d", &c, &u, &v);
        edges.push_back(std::pair<int, int>(u-1, v-1));
        n = std::max(std::max(n, u), v);
    }else if(tmp[0] == 'l'){
        sscanf(tmp.data(), "%c %d", &c, &len);
    }else if(tmp[0] == 't'){
        sscanf(tmp.data(), "%c %d %d", &c, &s, &t);
        s--, t--;
    }
  }
  m = edges.size();
  std::vector<std::vector<edge> > _G(n);
  for(int i = 0; i < m; i++) {
    int u = edges[i].first, v = edges[i].second;
    _G[u].push_back(edge(u, v, cnt));
    _G[v].push_back(edge(v, u, cnt++));
  }
  ESTP estp(_G);
  // auto start = std::chrono::system_clock::now();
  estp.Enumerate(s, t, len);
  // auto end = std::chrono::system_clock::now();
  // auto diff = end - start;
  // std::cout << "elapsed time = "
  //           << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count()
  //           << " msec."
  //           << std::endl;  
  estp.print();
  return 0;
}
