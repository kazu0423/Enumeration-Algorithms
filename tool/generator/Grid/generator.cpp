#include<random>
#include<vector>
#include<string>
#include<set>
#include<iostream>
#include<algorithm>
#include<queue>
#include<fstream>
#include<cstring>
#include<ctime>

#include"unionFind.hpp"
typedef std::pair<int, int> pii;

std::mt19937 mt(14);


bool checkCon(std::vector<std::vector<int> > &G) {
  int n = G.size();
  UnionFind uf(n);
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < G[i].size(); j++) {
      uf.unite(i, G[i][j]);
    }
  }
  return uf.size() == 1;
}

void GridGraph(std::ofstream &output_file, int n, int m, int W, double prob = -1){
  int dx[] = {1, 0, -1, 0}, dy[] = {0, 1, 0, -1};
  std::vector<std::vector<int> > G(n*m);
  int num_edge = 0;
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < m; j++) {
      int v = i*m + j;
      for(int k = 0; k < 4; k++) {
        if(i + dy[k] >= n)continue;
        if(i + dy[k] < 0)continue;
        if(j + dx[k] >= m)continue;
        if(j + dx[k] < 0)continue;
        int u = (i+dy[k])*m + (j+dx[k]);
        if(mt()%7 <= 3){
          G[v].push_back(u);
          G[u].push_back(v);
          num_edge++;
        }
      }
    }
  }
  if(not checkCon(G)){
    std::cout << "non connected." << std::endl;
    exit(1);
  }
  output_file << n*m << " " << num_edge << std::endl;
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < m; j++) {
      for(int k = 0; k < G[i*m + j].size(); k++) {
        if(i*m+j < G[i*m+j][k])continue;
        output_file << i*m+j << " " << G[i*m+j][k] << " " << (mt()%W)+1 << std::endl;
      }
    }
  }
}

int main(int argc, char *argv[]){
  double prob;
  int n, m, W;
  if(argc == 5){
    n = atoi(argv[1]), m = atoi(argv[2]), W = atoi(argv[3]), prob = atof(argv[3]); 
  }else{
    std::cerr << "Enter the number of vertices and density." << std::endl;
    exit(1);
  }
  std::string name;
  std::cout << "Generate grid graphs." << std::endl;
  for (int i = 1; i <= 1; i++) {
    char tmp[5];
    sprintf(tmp, "%02d", i);
    std::string id = tmp;
    name = "GridGraph_" + std::to_string(n).substr(0, 3) + "_" + std::to_string(m).substr(0, 3) + "_" + std::to_string(W).substr(0, 3) + "_" + std::to_string(prob).substr(0, 4) + ".in";
    std::cout << name << std::endl;
    std::ofstream output_file(name.c_str());
    GridGraph(output_file, n, m, prob);
    output_file.close();
  }
  return 0;
}
