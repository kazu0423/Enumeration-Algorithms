#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <queue>

#include "enum.hpp"

std::vector<int> GreedySol(VertexWeightedGraph &G, std::vector<int> partial_sol = std::vector<int>()){
  int n = G.size();
  std::vector<bool> is_added(n, true);
  for(int i = 0; i < partial_sol.size(); i++) {
    int v = partial_sol[i];
    is_added[v] = false;
    for(int j = 0; j < G[v].size(); j++) is_added[G[v][j]] = false;
  }
  for(int i = 0; i < n; i++) {
    if(is_added[i]){
      partial_sol.push_back(i);
      for(int j = 0; j < G[i].size(); j++) is_added[G[i][j]] = false;
    }
  }
  std::sort(partial_sol.begin(), partial_sol.end());
  return partial_sol;
}

int computeWeight(VertexWeightedGraph &G, std::vector<int> &IS){
  int res = 0;
  for(int i = 0; i < IS.size(); i++) res += G.getWeight(IS[i]);
  return res;
}

void Enumerate(VertexWeightedGraph &G, int k){
  std::priority_queue<std::pair<int, std::vector<int> > >  que;
  std::set<std::vector<int> > output;
  std::vector<std::vector<int> > output_sorted;
  std::vector<int> IS = GreedySol(G);
  int weight = computeWeight(G, IS);
  que.push(std::pair<int, std::vector<int> >(weight, IS));
  output.insert(IS);
  output_sorted.push_back(IS);

  while(not que.empty() and output_sorted.size() < k){
    if(output.size() % 1000000 == 0){
      std::cout << output.size() << std::endl;
    }
    IS = que.top().second;
    que.pop();
    output_sorted.push_back(IS);
    int pos = 0;
    for(int i = 0; i < G.size(); i++) {
      if(IS[pos] == i) continue;
      while(pos < IS.size() and IS[pos] < i and i < IS[pos + 1]) pos++;
      std::vector<bool> partial_sol(G.size(), false);
      for(int j = 0; j < pos+1; j++) partial_sol[IS[j]] = true;
      partial_sol[i] = true;
      for(int j = 0; j < G[i].size(); j++) partial_sol[G[i][j]] = false;
      std::vector<int> new_IS;
      for(int j = 0; j < G.size(); j++) if(partial_sol[j]) new_IS.push_back(j);
      new_IS = GreedySol(G, new_IS);
      sort(new_IS.begin(), new_IS.end());
      int new_weight = computeWeight(G, new_IS);
      if(output.find(new_IS) != output.end()) continue;
      que.push(std::pair<int, std::vector<int> >(new_weight, new_IS));
      output.insert(new_IS);
    }
  }
  // std::cout << "solution set." << std::endl;
  for(auto IS: output_sorted){
    std::cout << computeWeight(G, IS) << std::endl;
    // std::cout << "weight:" << computeWeight(G, IS) << " ";
    // for(int i = 0; i < IS.size(); i++) {
      // std::cout << IS[i] + 1 << " ";
    // }
    // std::cout << std::endl;
  }
  return;
}