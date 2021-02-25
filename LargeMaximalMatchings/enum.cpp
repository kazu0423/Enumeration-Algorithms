#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <queue>
#include "enum.hpp"

#include <boost/dynamic_bitset.hpp>

void computeMatched(Graph &G, boost::dynamic_bitset<> &m, std::vector<int> &matched){
  for(int i = 0; i < G.size(); i++) matched[i] = -1;
  for(int i = 0; i < G.edge_size(); i++) {
    if(m.test(i) == true){
      int u = G.getEdge(i).first, v = G.getEdge(i).second;
      matched[u] = matched[v] = i;
    }
  }
}

void Enumerate(Graph &G, boost::dynamic_bitset<> &matching, int k, std::set<boost::dynamic_bitset<> > &output){
  std::queue<boost::dynamic_bitset<> >  que;
  que.push(matching);
  output.insert(matching);
  boost::dynamic_bitset<> m = matching, tmp;
  std::vector<int> matched(G.size());
  while(not que.empty()){
    if(output.size() % 1000 == 0){
      std::cout << output.size() << std::endl;
    }
    m = que.front();
    que.pop();
    for(int i = 0; i < G.edge_size(); i++) {
      if (m.test(i) == false) {
        computeMatched(G, m, matched);
        tmp = m;
        int u = G.getEdge(i).first, v = G.getEdge(i).second;
        // std::cout << "test:" << matched[u] << ", " << matched[v] << std::endl;
        // std::cout << m << std::endl << std::endl;
        m.set(i);
        // std::cout << "hoge:" << matched[v] << std::endl;
        // std::cout << u << ", " << v << std::endl;
        int a[] = {u, v};
        for (int j = 0; j < 2; j++) {
          int w = a[j];
          if (matched[w] != -1) {
            std::pair<int, int> e = G.getEdge(matched[w]);
            // std::cout << "edge:" <<  e.first << " " << e.second << " " << G.getId(e) << std::endl;
            m.reset(G.getId(e));
            int z = e.first;
            if (z == w) z = e.second;
            matched[z] = -1;
            for (int k = 0; k < G[z].size(); k++) {
              int to = G[z][k];
              // std::cout << "to:" << to << std::endl;
              // for(int l = 0; l < G.size(); l++) std::cout << matched[l] << " ";
              // std::cout << std::endl;
              if (matched[to] == -1 and to != u and to != v) {
                int id = G.getId(std::pair<int, int>(z, to));
                // std::cout << id << ": " << G.getEdge(id).first << " " << G.getEdge(id).second << std::endl;
                matched[to] = matched[z] = id;
                m.set(id);
                break;
              }
            }
          }
        }
        matched[u] = matched[v] = i;
        // std::cout << "matching size:" << m.count() << std::endl;
        // std::cout << tmp << std::endl;
        // std::cout << m << std::endl << std::endl;
        if(m.count() >= k and output.find(m) == output.end()) {
          que.push(m);
          output.insert(m);
        }
        m = tmp;
      }
    }
  }
  return;
}