// Hypergraph independent set ZDD construction (simple version)
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <tdzdd/DdSpec.hpp>
#include <tdzdd/DdStructure.hpp>
#include <tdzdd/eval/ToZBDD.hpp>
#include <vector>

#include "dual.hpp"

// SAPPOROBDD headers (adjust include path/name to your local installation).
#include "ZBDD.h"
#include "SBDD_helper.h"

using namespace std;

// 頂点数
int N = 0;
// edges_by_min[i] は「最小要素が i のハイパー辺」一覧
vector<vector<vector<int> > > edges_by_min;

// a と b はソート済み（昇順 or 降順どちらでも可）である前提で，
// a が b の部分集合かを判定する。
static bool is_subset_sorted(const vector<int>& a, const vector<int>& b) {
  size_t i = 0, j = 0;
  while (i < a.size() && j < b.size()) {
    if (a[i] == b[j]) {
      ++i;
      ++j;
    } else {
      // 並び順は関係ないので，b 側を進めるだけ
      ++j;
    }
  }
  return i == a.size();
}

// 各 vector がソート済みであることを前提に，
// 他の vector に包含される vector を削除する。
static void remove_contained_vectors(vector<vector<int> >& sets) {
  if (sets.empty()) return;

  // まずサイズ昇順で見ると，既に確定した小さい集合で包含判定できる
  vector<size_t> idx(sets.size());
  for (size_t i = 0; i < sets.size(); ++i) idx[i] = i;

  sort(idx.begin(), idx.end(), [&](size_t a, size_t b) {
    if (sets[a].size() != sets[b].size()) return sets[a].size() < sets[b].size();
    return sets[a] < sets[b];
  });

  vector<vector<int> > minimal;
  minimal.reserve(sets.size());

  for (size_t id : idx) {
    const auto& s = sets[id];
    bool covered = false;
    for (const auto& m : minimal) {
      if (m.size() > s.size()) break;
      if (is_subset_sorted(m, s)) {
        covered = true;
        break;
      }
    }
    if (!covered) minimal.push_back(s);
  }

  sets.swap(minimal);
}

class FastInput {
 public:
  FastInput() : idx_(0), size_(0), last_was_cr_(false) {}

  bool ReadHyperedges(vector<vector<int> >& edges, int& max_elem,
                      bool& has_zero) {
    vector<int> current;
    int num = 0;
    bool in_num = false;
    int c = 0;
    while ((c = Read()) != EOF) {
      if (c >= '0' && c <= '9') {
        num = num * 10 + (c - '0');
        in_num = true;
        continue;
      }
      if (in_num) {
        current.push_back(num);
        if (num == 0) has_zero = true;
        if (num > max_elem) max_elem = num;
        num = 0;
        in_num = false;
      }
      if (c == '\r') {
        if (!current.empty()) {
          edges.push_back(std::move(current));
          current.clear();
        }
        last_was_cr_ = true;
        continue;
      }
      if (c == '\n') {
        if (last_was_cr_) {
          last_was_cr_ = false;
          continue;
        }
        if (!current.empty()) {
          edges.push_back(std::move(current));
          current.clear();
        }
        continue;
      }
      last_was_cr_ = false;
    }
    if (in_num) {
      current.push_back(num);
      if (num == 0) has_zero = true;
      if (num > max_elem) max_elem = num;
    }
    if (!current.empty()) edges.push_back(std::move(current));
    return !edges.empty();
  }

 private:
  int Read() {
    if (idx_ >= size_) {
      size_ = std::fread(buf_, 1, sizeof(buf_), stdin);
      idx_ = 0;
      if (size_ == 0) return EOF;
    }
    return static_cast<unsigned char>(buf_[idx_++]);
  }

  char buf_[1 << 16];
  size_t idx_;
  size_t size_;
  bool last_was_cr_;
};

// ハイパーグラフの independent set:
// どのハイパー辺も「全ての頂点を選ぶ」ことがない集合
class HyperIndependentSetSpec
    : public tdzdd::DdSpec<HyperIndependentSetSpec, vector<vector<int> >, 2> {
 public:
  explicit HyperIndependentSetSpec(const vector<vector<int> >& edges)
      : edges_(edges) {}

  size_t hash_code(void const* p, int) const {
    auto const& s = *static_cast<vector<vector<int> > const*>(p);
    size_t h = 0;
    for (const auto& edge : s) {
      h = h * 131 + edge.size();
      for (int v : edge) h = h * 131 + v;
    }
    return h;
  }

  bool equal_to(void const* p, void const* q, int) const {
    auto const& a = *static_cast<vector<vector<int> > const*>(p);
    auto const& b = *static_cast<vector<vector<int> > const*>(q);
    return a == b;
  }

  int getRoot(vector<vector<int> >& state) const {
    // 空のハイパー辺があると解なし
    for (const auto& e : edges_) {
      if (e.empty()) return 0;
    }
    state = edges_;
    return N;
  }

  // 変数順序は頂点番号順 (N, N-1, ..., 1)
  int getChild(vector<vector<int> >& state, int level, int take) const {
    const int current_var = N - level;
    vector<vector<int> > next_state = state;
    // cout << "level: " << level << ", current_var: " << current_var << ", take: " << take << ", state size: " << state.size() << endl;

    for(auto e: edges_by_min[current_var]) {
      next_state.push_back(e);
    }
    if (take) {  // 頂点を選ぶ
      for(auto &edge: next_state) {
         edge.erase(remove(edge.begin(), edge.end(), current_var), edge.end());
         if(edge.empty()){
            cout << "No solution due to empty hyperedge." << endl;
            return 0; // 空のハイパー辺があると解なし  
         }
      }
      remove_contained_vectors(next_state);
      state = next_state;
      for(auto &e: state) {
        sort(e.begin(), e.end());
      }
      sort(state.begin(), state.end());
      if(level == 1) return -1; // 真の終端
      else return level - 1;
    } else {  // 頂点を選ばない
      for(auto &edge: next_state) {
        if(find(edge.begin(), edge.end(), current_var) != edge.end()) {
          // current_var を含む辺は「選ばない頂点が確定」したので削除
          next_state.erase(remove(next_state.begin(), next_state.end(), edge), next_state.end());
        }
      }
      remove_contained_vectors(next_state);
      state = next_state;
      for(auto &e: state) {
        sort(e.begin(), e.end());
      }
      sort(state.begin(), state.end());
      if(level == 1) return -1; // 真の終端
      else return level - 1;
    }
  }

 private:
  const vector<vector<int> >& edges_;
};

int main() {
  vector<vector<int> > edges;
  int max_elem = 0;
  bool has_zero = false;
  FastInput in;
  if (!in.ReadHyperedges(edges, max_elem, has_zero)) return 0;

  int offset = has_zero ? 1 : 0;
  if (offset == 1) {
    for (auto& edge : edges) {
      for (int& v : edge) v += 1;
    }
  }
  N = max_elem + offset;

  // 各ハイパー辺を降順・重複除去
  for (auto& edge : edges) {
    sort(edge.begin(), edge.end(), greater<int>());
    edge.erase(unique(edge.begin(), edge.end()), edge.end());
  }

  // 最小要素ごとにハイパー辺を分類
  edges_by_min.assign(N + 1, vector<vector<int> >());
  for (const auto& edge : edges) {
    if (edge.empty()) continue;
    int min_elem = edge.back();  // 降順なので最後が最小
    if (min_elem >= 1 && min_elem <= N) {
      edges_by_min[min_elem].push_back(edge);
    }
  }
  sort(edges.begin(), edges.end());
  vector<vector<int> > empty;
  HyperIndependentSetSpec spec(empty);
  cout << "constructing ZDD" << endl;
  tdzdd::DdStructure<2> dd(spec);
 cout << "Size of Quasi-Reduced ZDD: " << dd.size() << endl;
  dd.zddReduce();
  cout << "Size of Reduced ZDD: " << dd.size() << endl;
  ZBDD IS = dd.evaluate(tdzdd::ToZBDD());

  cerr << "Building dual family..." << '\n';
  ZBDD dual = dual_family(IS, N);
  dual.Print();

  cerr << "Minimalization..." << '\n';
  ZBDD minimal = minimalize_upward_closed(dual, N);
  
  cout << "Number of solutions: " << minimal.Card() << endl;

  return 0;
}
