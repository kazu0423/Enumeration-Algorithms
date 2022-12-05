///////////////////////////////////////////////////////
/*                                                   */
/* Enumeration of inclusion-wise minimal separators  */
/* ??? delay                                         */
/* kurita                                            */
/*                                                   */
///////////////////////////////////////////////////////

#include<iostream>
#include<vector>
#include<queue>
#include<fstream>
#include<chrono>

#include"Enum.hpp"
using bigint = unsigned long long int;

bool disconnected(std::vector<std::vector<int> > G, int n, std::vector<int> &separator) {
    std::queue<int> que;
    std::vector<bool> used(n, false);
    for(int v: separator) used[v] = true;
    for(int i = 0; i < n; i++) {
        if(not used[i]) {
            que.push(i);
            // std::cout << "i:" << i << std::endl;
            used[i] = true;
            break;
        }
    }
    if(que.size() == 0) return true;
    // std::cout << "hoge" << std::endl;
    int count = 1;
    while(not que.empty()){
        int v = que.front();
        que.pop();
        for(int i = 0; i < G[v].size(); i++) {
            int u = G[v][i];
            // std::cout << "u:" << u << std::endl;
            if(not used[u]){
                // std::cout << "add u:" << u << std::endl;
                used[u] = true;
                que.push(u);
                count++;
            }
        }
    }
    // std::cout << separator.size() << " " << count << std::endl;
    if(separator.size() + count != n) return true;
    // std::cout << "fuga" << std::endl;
    count = 0;
    for(int i = 0; i < n; i++) used[i] = false;
    for(int i = 0; i < separator.size(); i++) used[separator[i]] = true;
    for(int v: separator) {
        for(int i = 0; i < G[v].size(); i++) {
            if(not used[G[v][i]]) break;
            if(i == G[v].size() - 1) return true;
        }
    }
    return false;
}

void minimization(std::vector<std::vector<int> > G, int n, std::vector<int> &separator){    
  std::vector<int> loop = separator;
  for(int v: loop){
    std::vector<int> tmp;
    for(int u: separator) if(u != v) tmp.push_back(u);
    if(disconnected(G, n, tmp)) separator = tmp;
  }
}


std::vector<int> split(std::string s){
  int pos = 0, prev = 0;
  std::vector<int> ans;
  while(pos < s.length()) {
    if(s[pos] != ' ') {
      pos++;
    }else{
      pos++;
      std::cout << "a;" << s.substr(prev, pos - prev) << std::endl;
      std::cout << "b:" << stoi(s.substr(prev, pos - prev)) << std::endl;
      ans.push_back(std::stoi(s.substr(prev, pos - prev)) - 1);
      prev = pos;
    }
  }
  std::cout << "a;" << s.substr(prev, pos - prev) << std::endl;
  std::cout << "b:" << stoi(s.substr(prev, pos - prev)) << std::endl;
  ans.push_back(std::stoi(s.substr(prev, pos - prev)) - 1);
  std::cout << std::endl;
  return ans;
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
  std::cout << "bruteforce." << std::endl;
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
  std::vector<bool> bit(n, false);
  std::queue<std::pair<std::vector<bool>, int> > que;
  que.push(std::pair<std::vector<bool>, int>(bit, 0));
  while(not que.empty()){
    // std::cout << "que:" << que.size() << std::endl;
    bit = que.front().first;
    int s = que.front().second;
    que.pop();
    for(int i = s; i < n; i++) {
      if(bit[i])continue;
      std::vector<int> sep;
      bit[i] = true;
      for(int j = 0; j < n; j++) if(bit[j]) sep.push_back(j);
      if(disconnected(G, n, sep)) {
        int size = sep.size();
        minimization(G, n, sep);
        if(size == sep.size())cnt++;
      }else{
        que.push(std::pair<std::vector<bool>, int>(bit, i+1));
      }
      bit[i] = false;
    }

  }
  std::cout << "cnt:" << cnt << std::endl;

  return 0;
}
