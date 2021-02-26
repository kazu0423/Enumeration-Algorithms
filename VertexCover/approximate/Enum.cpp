#include<vector>
#include<tuple>
#include<queue>
#include<iostream>

#include"Enum.hpp"

void AppEnumVC::AppVC(){
  current_vc.resize(n, 0);
  for (int i = 0; i < n; i++){
    for (int j = 0; j < G[i].size(); j++){ 
      if(current_vc[i] == 0 and current_vc[G[i][j]] == 0){
        current_vc[i] = current_vc[G[i][j]] = 1;
        break;
      }
    }
  }
  minimize();
}

void AppEnumVC::minimize(){  
  vc_size = 0;
  for(int i = 0; i < n; i++) {
    if(current_vc[i] == 1){
      current_vc[i] = 0;
      current_vc[i] = not checkVC();
    }
    vc_size += current_vc[i];
  }
}

bool AppEnumVC::checkVC(){
  for(int i = 0; i < n; i++) {
    int v = i;
    if(current_vc[v])continue;
    for(int j = 0; j < G[v].size(); j++) {
      int u = G[v][j];
      if(current_vc[u] == 0)return false;
    }
  }
  return true;
}

void AppEnumVC::AppEnumerate(int k){
  ans_size = 0;
  AppVC();
  std::queue<std::vector<int> > que;
  int initial_size = 0;
  for(int i = 0; i < n; i++) initial_size += current_vc[i];
  if(initial_size > 2*k)return;
  que.push(current_vc);
  printCurrentSol();
  while(not que.empty()){
    ans_size++;
    current_vc = que.front();
    que.pop();
    tmp = current_vc;
    for(int i = 0; i < n; i++) {
       if(current_vc[i] == 0) continue;
       current_vc[i] = 0;
       for(int j = 0; j < G[i].size(); j++) current_vc[G[i][j]] = 1;
       minimize();
       //std::cout << vc_size  << " " << que.size() << std::endl;
       if(Outputs.find(current_vc) != Outputs.end()) continue;
       if(vc_size > initial_size + k) continue;
       que.push(current_vc);
       printCurrentSol();
       current_vc = tmp;
    }
  }
}

void AppEnumVC::printCurrentSol(){
  Outputs.insert(current_vc);
  for(int i = 0; i < n; i++) {
    std::cout << current_vc[i] << " ";
  }
  std::cout << std::endl;
}

void AppEnumVC::printSol(){
  std::cout << "end." << std::endl;
}