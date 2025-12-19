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
  deg.resize(c, 0);
  for(int i = 0; i < c; i++) {
    R[i].resize(c,0);
  }
}

void NonIsoGraphs::Enumerate(){
  // root計算．パスにした方が良さそう
  // パスだと水素の数が少ない時にまずい．サイクルだと全部2にするといつでも答えが作れるので閉路の方が良い．
  //　水素が4つ以上ある場合を考えるか．．．
  for(int i = 0; i < c; i++) {
    if(i < c - h/2) R[i][(i+1)%c] = R[(i + 1)%c][i] = 2;
    else R[i][(i + 1)%c] = R[(i + 1)%c][i] = 1;
  }
  for(int i = 0; i < c; i++) {
    for(int j = 0; j < c; j++) deg[i] += R[i][j];
  }
  EnumRec(R, 0);
}

void NonIsoGraphs::EnumRec(Graph G, int depth = 0){
  std::cout << "start EnumRec " << depth << std::endl;
  print();
  std::vector<Graph> children = EnumChildren(G);
  ans += children.size();
  for (int i = 0; i < children.size(); i++) {
    EnumRec(children[i], depth + 1);
  }
  std::cout << "end EnumRec " << depth << std::endl;
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
          bool degree = true;
          for(int a = 0; a < c; a++) {
            int sum = 0;
            for(int b = 0; b < c; b++) sum += H[a][b];
            degree &= (sum <= 4);
          }
          if (connectivity(H) and degree) {
            tmp = Parent(H);
            if (GraphIso(G, tmp)) children.push_back(H);
          }
          H[x][y]++;
          H[y][x]++;
        }
      }
      H[i][j]--;
      H[j][i]--;
    }
  }
  std::vector<Graph> result;
  for(int i = 0; i < children.size(); i++) {
    bool is_child = true;
    for(int j = 0; j < children.size() and is_child; j++) {
      if(i == j) continue;
      is_child &= (not GraphIso(children[i], children[j]));
    }
    if(is_child) result.push_back(children[i]);
  }
  // std::cout << children.size() << " num: end" << std::endl;
  return result;
}

// 0 ~ c-1 のハミルトン閉路を作りに行く．
//最長パスを伸ばす辺交換を行う．
Graph NonIsoGraphs::Parent(Graph G){
  std::cout << "original" << std::endl;
  for(int i = 0; i < c; i++) {
    for(int j = 0; j < c; j++) {
      std::cout << G[i][j];
    }
    std::cout << std::endl;
  }

  Graph empty(c), H = G;
  for (int i = 0; i < c; i++) empty[i].resize(c, -1);
  if (GraphIso(R, G)) return empty;
  int dp[1<<c][c][c];
  for(int i = 0; i < (1<<c); i++) 
    for(int j = 0; j < c; j++) 
      for(int k = 0; k < c; k++) 
        dp[i][j][k] = 0;

  int maxi = -1;
  std::tuple<int, int, int> goal;
  std::queue<std::tuple<int, int, int> > que;
  for(int i = 0; i < c; i++) que.push(std::tuple<int, int, int>((1<<i), i, i));
  while(not que.empty()){
    int status = std::get<0>(que.front()), v = std::get<1>(que.front()), u = std::get<2>(que.front());
    que.pop();
    for(int i = 0; i < c; i++) {
      if(G[u][i] == 0) continue;
      if((status & (1<<i)) > 0) continue;
      int next = status + (1 << i);
      dp[next][v][i] = std::max(dp[next][v][i], dp[status][v][u] + 1);
      que.push(std::tuple<int, int, int>(next, v, i));
      if(maxi < dp[next][v][i]) {
        maxi = dp[next][v][i], goal = std::tuple<int, int, int>(next, v, i);
      }
    }
  }
  // std::cout << "end longest path." << std::endl;
  bool hamiltonian = false;
  for(int i = 0; i < c; i++) {
    for(int j = 0; j < c; j++) {
      if(G[i][j] > 0){
        hamiltonian |= (dp[(1<<c)-1][i][j] > 0);
      }
    }
  }
  // std::cout << "end hamiltonicity: maxi:" << maxi << std::endl;

  std::vector<int> ordering;
  std::vector<bool> used(c, false);
  ordering.push_back(std::get<2>(goal));
  used[std::get<2>(goal)] = true;
  while(maxi > 0){
    int status = std::get<0>(goal), v = std::get<1>(goal), u = std::get<2>(goal);
    // std::cout << "status:" << status << " maxi:" << maxi << std::endl;
    for(int i = 0; i < c; i++) {
      if(G[u][i] == 0) continue;
      if((status & (1<<u)) == 0) continue;
      if(dp[status - (1<<u)][v][i] == maxi - 1){
        maxi--;
        goal = std::tuple<int, int, int>(status - (1<<u), v, i);
        used[i] = true;
        ordering.push_back(i);
        break;
      }
    }
  }
  // std::cout << "ordering size:" << ordering.size() << std::endl;
  for(int i = 0; i < c; i++) {
    if(used[i] == false){
      ordering.push_back(i);
    }
  }
  Graph tmp = G;
  for(int i = 0; i < c; i++) {
    for(int j = 0; j < c; j++) {
      G[i][j] = tmp[ordering[i]][ordering[j]];
    }
  }
  // std::cout << "ordering size:" << ordering.size() << std::endl;
  // for(int i = 0; i < ordering.size(); i++) std::cout << ordering[i] << " ";
  // std::cout << std::endl;
  // std::cout << "reordering" << std::endl;
  // for(int i = 0; i < c; i++) {
  //   for(int j = 0; j < c; j++) {
  //     std::cout << G[i][j];
  //   }
  //   std::cout << std::endl;
  // }
  //   std::cout << std::endl;
  //   std::cout << std::endl;
  //   std::cout << std::endl;

  for(int i = 0; i < c; i++) {
    for(int j = i+2; j < c; j++) {
      if((i == 0) and (j == c-1)) continue;
      if(G[i][j] > 0){
        for(int k = 0; k < c; k++) {
          if(G[k][(k+1)%c] < 1 + hamiltonian){
            G[k][(k+1)%c]++, G[(k+1)%c][k]++;
            G[i][j]--, G[j][i]--;
            if(connectivity(G)) return G;
            G[i][j]++, G[j][i]++;
            G[k][(k+1)%c]--, G[(k+1)%c][k]--;
          }
        }
      }
    }
  }
  //ここまで来るとグラフは閉路で，対角要素の隣が非ゼロ要素になるような頂点番号づけになっている．
  // 一番長い2以上の区間を伸ばすようにする．
  int start = 0, end = 0, length = 0, max_start, max_end;
  maxi = 0;
  for(int i = 0; i < 2*c; i++) {
    int x = i%c, y = (i+1)%c;
    if(G[x][y] >= 2){
      end = i+1;
      length++;
      if(maxi < length){
        maxi = length, max_start = start, max_end = end;
      }
      maxi = std::max(maxi, length);
    }else{
      length = 0;
      start = i+1;
    }
  }
  for(int i = max_end; i < 3*c; i++) {
    int x = i%c, y = (i+1)%c;
    int tx = max_end %c, ty = (max_end + 1) % c;
    if(G[x][y] > 1){
      G[tx][ty]++, G[ty][tx]++;
      G[x][y]--, G[y][x]--;
      return G;
    }
  }

  std::cout << "erorr parent." << std::endl;
  // for (int i = 0; i < c; i++) {
  //   for (int j = 0; j < c; j++) {
  //     std::cout << G[i][j];
  //   }
  //   std::cout << std::endl;
  // }
  return empty;
}
void NonIsoGraphs::print(){
  std::cout << "Number of solutions:" << ans << std::endl;
}

bool NonIsoGraphs::GraphIso(Graph G, Graph H) {
  // std::cout<< "start GraphIso." << std::endl;
  typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>
      graph_boost;
  std::vector<std::pair<int, int> > edges;

  for (int i = 0; i < c; i++) {
    for (int j = i + 1; j < c; j++) {
      for (int k = 0; k < G[i][j]; k++) {
        edges.push_back(std::pair<int, int>(i, j));
      }
    }
  }
  const graph_boost g1 = graph_boost(edges.begin(), edges.end(), c);
  edges = std::vector<std::pair<int, int> >();

  for (int i = 0; i < c; i++) {
    for (int j = i + 1; j < c; j++) {
      for (int k = 0; k < H[i][j]; k++) {
        edges.push_back(std::pair<int, int>(i, j));
      }
    }
  }
  const graph_boost g2 = graph_boost(edges.begin(), edges.end(), c);
  // std::cout<< "end GraphIso." << std::endl;
  return boost::isomorphism(g1, g2);
}

bool NonIsoGraphs::connectivity(Graph &G) {
  // std::cout << "start connectivity." << std::endl;
  std::vector<bool> visited(c, false);
  std::queue<int> que;
  visited[0] = true;
  que.push(0);
  while (not que.empty()) {
    int v = que.front();
    que.pop();
    for (int i = 0; i < c; i++) {
      if (G[v][i] == 0) continue;
      if (visited[i]) continue;
      visited[i] = true;
      que.push(i);
    }
  }
  bool f = true;
  for (int i = 0; i < c; i++) f &= visited[i];
  // std::cout << "end connectivity." << std::endl;
  return f;
}
