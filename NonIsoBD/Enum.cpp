#include<iostream>
#include<vector>
#include<queue>
#include<stack>
#include<utility>

#include"Enum.hpp"
#include<boost/graph/isomorphism.hpp>
#include<boost/graph/adjacency_list.hpp>

void NonIsoGraphs::init(int _n, int _c){
  n = _n, c = _c, h = _n - _c;
  R.resize(c);
  for(int i = 0; i < c; i++) {
    R[i].resize(c,0);
  }
}

void NonIsoGraphs::Enumerate(){
  // root計算
  int m = (4*n + c)/2;
  for(int i = 0; i < c; i++) {
    if(i < c - h/2) R[i][(i+1)%c] = R[(i + 1)%c][i] = 2;
    else R[i][(i + 1)%c] = R[(i + 1)%c][i] = 1;
  }
  EnumRec(R, 0);
}

void NonIsoGraphs::EnumRec(Graph G, int depth = 0){
  std::cout << "start EnumRec " << depth << std::endl;
  print();
  std::vector<Graph> children = EnumChildren(G);
  ans += children.size();
  for(int i = 0; i < children.size(); i++) {
    EnumRec(children[i], depth + 1);
  }
  std::cout << "end EnumRec " << depth << std::endl;
}

bool NonIsoGraphs::connectivity (Graph &G){
  // std::cout << "start connectivity." << std::endl;  
  std::vector<bool> visited(c, false);
  std::queue<int> que;
  visited[0] = true;
  que.push(0);
  while(not que.empty()){
    int v = que.front();
    que.pop();
    for(int i = 0; i < c; i++) {
      if(G[v][i] == 0)continue;
      if(visited[i]) continue;
      visited[i] = true;
      que.push(i);
    }
  }
  bool f = true;
  for(int i = 0; i < c; i++) f &= visited[i];
  // std::cout << "end connectivity." << std::endl;  
  return f;
}

std::vector<Graph> NonIsoGraphs::EnumChildren(Graph G){
  Graph H = G, tmp;
  std::vector<Graph> children;
  // std::cout << "start children" << std::endl;
  for(int i = 0; i < c; i++) {
    for(int j = i+1; j < c; j++) {
      H[i][j]++;
      H[j][i]++;
      for (int x = 0; x < c; x++) {
        for(int y = x+1; y < c; y++) {
          if(H[x][y] == 0) continue;
          if((i == x) and (j == y)) continue;
          H[x][y]--;
          H[y][x]--;
          if(not connectivity(H)) continue;
          if(GraphIso(R, H)) continue;
          tmp = Parent(H);
          if(GraphIso(G, tmp)) children.push_back(H);
          H[x][y]++;
          H[y][x]++;
        }
      }
      H[i][j]--;
      H[j][i]--;
    }
  }
  // std::cout << children.size() << " num: end" << std::endl;
  return children;
}

bool NonIsoGraphs::GraphIso (Graph G, Graph H){
  // std::cout<< "start GraphIso." << std::endl;
  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> graph_boost;
  std::vector<std::pair<int, int> > edges;

  for(int i = 0; i < c; i++) {
    for(int j = i+1; j < c; j++) {
      for(int k = 0; k < G[i][j]; k++) {
        edges.push_back(std::pair<int, int>(i, j));
      }
    }
  }
  const graph_boost g1 = graph_boost(edges.begin(), edges.end(), c);
  edges = std::vector<std::pair<int, int> >();

  for(int i = 0; i < c; i++) {
    for(int j = i+1; j < c; j++) {
      for(int k = 0; k < H[i][j]; k++) {
        edges.push_back(std::pair<int, int>(i, j));
      }
    }
  }
  const graph_boost g2 = graph_boost(edges.begin(), edges.end(), c);
  // std::cout<< "end GraphIso." << std::endl;
  return boost::isomorphism(g1, g2);
}

// 全部の頂点の隣接頂点数を2にすれば良い．
Graph NonIsoGraphs::Parent(Graph G){
  if(GraphIso(R, G)) return G;
  // std::cout << "start Parent" << std::endl;
  for(int i = 0; i < c; i++) {
    std::vector<int> neighbor;
    for(int j = 0; j < c; j++) if(G[i][j] > 0) neighbor.push_back(j);
    if(neighbor.size() > 2){
      for(int j = 0; j < neighbor.size(); j++) {
        for(int k = 0; k < neighbor.size(); k++) {
          if(j == k) continue;
          //辺 {i, w} を {i, u} に書き換える
          int u = neighbor[j], w = neighbor[k];
          int tmp1 = G[i][u], tmp2 = G[i][w];
          G[i][u]++, G[u][i]++;
          G[i][w]--, G[w][i]--;
          if(not connectivity(G)){
            G[i][w]++, G[w][i]++;
            G[i][u]--, G[u][i]--;
          }else{
            i = j = k = n + 1;
          }
        }
      }
    }
    if(i == n + 1) {
      // std::cout << "non cycle case" << std::endl;
      return G;
    }
  }
  // std::cout << "cycle. " << std::endl;
  // ここに来るということはグラフは多重辺を無視すれば閉路になる．
  // DFSで頂点列を計算．
  std::stack<std::pair<int, int> > st;
  std::vector<bool> visited(c, false);
  std::vector<int> ordering;
  st.push(std::pair<int, int>(0, 0));
  ordering.push_back(0);
  visited[0] = true;
  while(not st.empty()){
    int v = st.top().first, iter = st.top().second;
    st.pop();
    for(int i = iter; i < c; i++) {
      if(G[v][i] > 0 and visited[i] == false){
        st.push(std::pair<int, int>(v, i + 1));
        st.push(std::pair<int, int>(i, 0));
        visited[i] = true;
        ordering.push_back(i);
        break;
      }
    }
  }
  // std::cout << "end dfs." << std::endl;
  // std::cout << "ordering size and n" << ordering.size() << " " << n << std::endl;

  int start, end = ordering[c-1];//多重度が2の列で一番長い列の始点と終点を計算  
  int longest = 0;
  for(int i = 0; i < ordering.size(); i++) {
    int u = ordering[i], w = ordering[(i+1)%c];
    if(G[u][w] == 2){
      longest++;
      if(longest == 1) start = u;
    }else{
      end = u;
    }
  }
  G[end][ordering[(end+1)%c]]++;
  G[ordering[(end+1)%c]][end]++;
  // std::cout << "ordering size:" << ordering.size() << std::endl;
  // std::cout << "cycldddde. " << std::endl;
  // std::cout << "cycldddde. " << std::endl;
  for(int i = 0; i < n; i++) {
    int u = ordering[(i+end+1)%c], w = ordering[(i+end+2)%c];
    // std::cout << "u: " << u << ", w:" << w << std::endl;
    // std::cout << "end: " << (i + end + 1) % c << " " << (i + end + 2) % c << std::endl;
    if (G[u][w] > 1) {
      G[u][w]--, G[w][u]--;
      break;
    }
  }
  // std::cout << "end Parent" << std::endl;
  return G;
}
void NonIsoGraphs::print(){
  std::cout << ans << std::endl;
}
