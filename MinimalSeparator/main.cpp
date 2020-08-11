/****************************************************************************************/
/*                                                                                      */
/*  Title: Generating All the Minimal Separators of a Graph                             */
/*  Minimal Separator Enumeration: Anne Berry, Jean-Paul Bordat, and Olivier Cogis      */
/*  Time complexity: O(n^3) time per solution                                           */
/****************************************************************************************/
#include<iostream>
#include<queue>
#include<vector>
#include<set>
#include<fstream>
#include<sys/time.h>
using namespace std;
using Graph = vector<vector<int> >;
//this function is used for time measurement
double gettimeofday_sec(){
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

vector<int> GetFullComponent(Graph &G, vector<int> &used, int s){
  vector<int> ret;
  queue<int> que;
  que.push(s);
  used[s] = 1;
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for (int i = 0; i < G[v].size(); i++) {
      if(used[G[v][i]] != 0) {
        if(used[G[v][i]] == -1)ret.push_back(G[v][i]);
        continue; 
      }
      used[G[v][i]] = 1;
      que.push(G[v][i]);
    }
  }
  return ret;
}


void EnumMinimalSperator(Graph &G, set<vector<int> > &ans){
  int n = G.size();
  vector<int> used(n, 0);
  queue<vector<int> > que;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) used[j] = 0;
    for (int j = 0; j < G[i].size(); j++) used[G[i][j]] = -1;
    used[i] = -1;
    if(used[i] == 0) {
      vector<int> tmp = GetFullComponent(G, used, i);
      ans.insert(tmp);
      que.push(tmp);
    }
  }
  while(not que.empty()){
    vector<int> S = que.front();
    que.pop();
    for (auto v: S) {
      for (int i = 0; i < n; i++) used[i] = 0;
      for (int i = 0; i < S.size(); i++) used[S[i]] = -1;
      for (int i = 0; i < G[i].size(); i++) used[G[v][i]] = -1;
      for (int i = 0; i < n; i++) {
        if(used[i] == 0) {
          vector<int> tmp = GetFullComponent(G, used, i);
          if(ans.find(tmp) == ans.end()){
            ans.insert(tmp);
            que.push(tmp);
          }
        }
      }
    }
  }
}

int main(int argc, char *argv[]){
  const int num_file = 3;
  if(argc != num_file){
    std::cerr << "Error : The number of input file is not " <<
      num_file << std::endl;
    return 0;
  }
  //opening input file
  std::ifstream ist(argv[1], std::ios::in);
  if(!ist){
    std::cerr << "can't open input file: " << argv[1] << std::endl;
    return 0;
  }
  //opening output files
  FILE *output;
  if((output = fopen(argv[2], "w")) == NULL){
    std::cerr << "can't write file open" << std::endl;
  }
 
  int n;
  double t1, t2;//for time measurement
    

  //initialize Graph
  std::string tmp;
  getline(ist, tmp);  
  sscanf(tmp.c_str(), "%d\n", &n);
  Graph G(n);
  while(getline(ist, tmp)){
    int u, v;
    sscanf(tmp.c_str(), "%d %d", &u, &v);
    G[u].push_back(v);
    G[v].push_back(u);
  }
  
  // //O(1) enumeration 
  t1 = gettimeofday_sec();
  set<vector<int> > ans;
  EnumMinimalSperator(G, ans);
  t2 = gettimeofday_sec();
  std::cout << "time:" << t2 - t1 << std::endl;
  std::cout << std::endl;
  return 0;
}
