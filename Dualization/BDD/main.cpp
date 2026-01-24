#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

// SAPPOROBDD headers (adjust include path/name to your local installation).
#include "BDD.h"
#include "SOP.h"

// This implementation uses the SAPPOROBDD C++ wrapper API in BDD.h.

namespace {

using std::cin;
using std::cerr;
using std::cout;
using std::vector;

class FastInput {
 public:
  FastInput() : idx_(0), size_(0), last_was_cr_(false) {}

  bool ReadSets(vector<vector<int>>& sets, int& max_elem, bool& has_zero) {
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
        if (num == 0) {
          has_zero = true;
        }
        if (num > max_elem) {
          max_elem = num;
        }
        num = 0;
        in_num = false;
      }
      if (c == '\r') {
        if (!current.empty()) {
          sets.push_back(std::move(current));
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
          sets.push_back(std::move(current));
          current.clear();
        }
        continue;
      }
      last_was_cr_ = false;
    }
    if (in_num) {
      current.push_back(num);
      if (num == 0) {
        has_zero = true;
      }
      if (num > max_elem) {
        max_elem = num;
      }
    }
    if (!current.empty()) {
      sets.push_back(std::move(current));
    }
    return !sets.empty();
  }

 private:
  int Read() {
    if (idx_ >= size_) {
      size_ = std::fread(buf_, 1, sizeof(buf_), stdin);
      idx_ = 0;
      if (size_ == 0) {
        return EOF;
      }
    }
    return static_cast<unsigned char>(buf_[idx_++]);
  }

  char buf_[1 << 16];
  size_t idx_;
  size_t size_;
  bool last_was_cr_;
};

// Build dual BDD: f^d(x) = ~f(~x).
class DualBuilder {
 public:
  BDD Build(const BDD& f) {
    bddword id = f.GetID();
    auto it = memo_.find(id);
    if (it != memo_.end()) {
      return it->second;
    }
    int var = f.Top();
    if (var == 0) {
      BDD res = (id == bddtrue) ? BDD(0) : BDD(1);
      memo_[id] = res;
      return res;
    }
    BDD lo = f.At0(var);
    BDD hi = f.At1(var);
    BDD dual_lo = Build(hi);
    BDD dual_hi = Build(lo);
    BDD var_bdd = BDDvar(var);
    BDD result = ((~var_bdd) & dual_lo) | (var_bdd & dual_hi);
    memo_[id] = result;
    return result;
  }

 private:
  std::unordered_map<bddword, BDD> memo_;
};


BDD minimalize_monotone_increasing(const BDD& f, int n) {
  BDD result = f;
  for (int i = 1; i <= n; ++i) {
    BDD fi0 = f.At0(i);
    BDD cond = (~BDDvar(i)) | ~fi0;
    result &= cond;
  }
  return result;
}


void apply_variable_order(std::vector<std::vector<int>>& sets, int n,
                          const std::vector<int>& order) {
  std::vector<int> map(n + 1, 0);
  for (int i = 0; i < n; ++i) {
    map[order[i]] = i + 1;
  }
  for (auto& elems : sets) {
    for (int& x : elems) {
      if (x >= 1 && x <= n) {
        x = map[x];
      }
    }
  }
}

std::vector<int> compute_vertex_separation_order(const vector<vector<int>>& sets,
                                                 int n) {
  // Greedy ordering to keep the frontier (vertex separation) small.
  std::vector<std::vector<int>> incident(n + 1);
  for (size_t ei = 0; ei < sets.size(); ++ei) {
    for (int v : sets[ei]) {
      if (v >= 1 && v <= n) {
        incident[v].push_back(static_cast<int>(ei));
      }
    }
  }

  std::vector<int> in_prefix_count(sets.size(), 0);
  std::vector<int> outside_count(sets.size(), 0);
  for (size_t ei = 0; ei < sets.size(); ++ei) {
    outside_count[ei] = static_cast<int>(sets[ei].size());
  }

  std::vector<int> frontier_count(n + 1, 0);
  std::vector<int> order;
  order.reserve(n);
  std::vector<char> in_prefix(n + 1, 0);
  int current_frontier = 0;

  std::vector<int> dec(n + 1, 0);
  std::vector<int> touched;
  touched.reserve(n);

  for (int step = 0; step < n; ++step) {
    int best_v = -1;
    int best_frontier = std::numeric_limits<int>::max();

    for (int v = 1; v <= n; ++v) {
      if (in_prefix[v]) {
        continue;
      }
      int new_frontier = current_frontier;
      int frontier_v = 0;

      for (int ei : incident[v]) {
        if (outside_count[ei] > 1) {
          ++frontier_v;
        } else if (outside_count[ei] == 1) {
          for (int u : sets[ei]) {
            if (!in_prefix[u]) {
              continue;
            }
            if (dec[u] == 0) {
              touched.push_back(u);
            }
            ++dec[u];
          }
        }
      }
      if (frontier_v > 0) {
        ++new_frontier;
      }
      for (int u : touched) {
        if (frontier_count[u] > 0 && frontier_count[u] - dec[u] == 0) {
          --new_frontier;
        }
        dec[u] = 0;
      }
      touched.clear();

      if (new_frontier < best_frontier) {
        best_frontier = new_frontier;
        best_v = v;
      }
    }

    if (best_v == -1) {
      break;
    }

    int v = best_v;
    in_prefix[v] = 1;
    order.push_back(v);

    int frontier_v = 0;
    for (int ei : incident[v]) {
      if (outside_count[ei] == 0) {
        continue;
      }
      if (outside_count[ei] == 1) {
        for (int u : sets[ei]) {
          if (in_prefix[u] && frontier_count[u] > 0) {
            --frontier_count[u];
            if (frontier_count[u] == 0) {
              --current_frontier;
            }
          }
        }
        --outside_count[ei];
        ++in_prefix_count[ei];
      } else {
        --outside_count[ei];
        ++in_prefix_count[ei];
        ++frontier_v;
      }
    }
    if (frontier_v > 0) {
      frontier_count[v] = frontier_v;
      ++current_frontier;
    }
  }

  for (int v = 1; v <= n; ++v) {
    if (!in_prefix[v]) {
      order.push_back(v);
    }
  }
  return order;
}

std::vector<int> compute_min_fill_order(const vector<vector<int>>& sets, int n) {
  // Min-Fill heuristic on the primal graph of the hypergraph.
  std::vector<std::vector<int>> neighbors(n + 1);
  std::vector<std::vector<char>> adj(n + 1, std::vector<char>(n + 1, 0));
  for (const auto& edge : sets) {
    for (size_t i = 0; i < edge.size(); ++i) {
      int u = edge[i];
      if (u < 1 || u > n) {
        continue;
      }
      for (size_t j = i + 1; j < edge.size(); ++j) {
        int v = edge[j];
        if (v < 1 || v > n) {
          continue;
        }
        if (!adj[u][v]) {
          adj[u][v] = 1;
          adj[v][u] = 1;
          neighbors[u].push_back(v);
          neighbors[v].push_back(u);
        }
      }
    }
  }

  std::vector<char> active(n + 1, 1);
  std::vector<int> order;
  order.reserve(n);
  std::vector<int> nbrs;
  nbrs.reserve(n);

  auto collect_neighbors = [&](int v) {
    nbrs.clear();
    for (int u : neighbors[v]) {
      if (active[u]) {
        nbrs.push_back(u);
      }
    }
  };

  for (int step = 0; step < n; ++step) {
    int best_v = -1;
    long long best_fill = std::numeric_limits<long long>::max();
    int best_deg = std::numeric_limits<int>::max();

    for (int v = 1; v <= n; ++v) {
      if (!active[v]) {
        continue;
      }
      collect_neighbors(v);
      int deg = static_cast<int>(nbrs.size());
      long long fill = 0;
      if (deg > 1) {
        long long existing = 0;
        for (size_t i = 0; i < nbrs.size(); ++i) {
          int u = nbrs[i];
          for (size_t j = i + 1; j < nbrs.size(); ++j) {
            int w = nbrs[j];
            if (adj[u][w]) {
              ++existing;
            }
          }
        }
        long long total_pairs =
            static_cast<long long>(deg) * (deg - 1) / 2;
        fill = total_pairs - existing;
      }

      if (fill < best_fill ||
          (fill == best_fill && (deg < best_deg ||
                                 (deg == best_deg && v < best_v)))) {
        best_fill = fill;
        best_deg = deg;
        best_v = v;
      }
    }

    if (best_v == -1) {
      break;
    }

    collect_neighbors(best_v);
    for (size_t i = 0; i < nbrs.size(); ++i) {
      int u = nbrs[i];
      for (size_t j = i + 1; j < nbrs.size(); ++j) {
        int w = nbrs[j];
        if (!adj[u][w]) {
          adj[u][w] = 1;
          adj[w][u] = 1;
          neighbors[u].push_back(w);
          neighbors[w].push_back(u);
        }
      }
    }
    active[best_v] = 0;
    order.push_back(best_v);
  }

  for (int v = 1; v <= n; ++v) {
    if (active[v]) {
      order.push_back(v);
    }
  }
  return order;
}

void print_variable_order(const std::vector<int>& order) {
  for (size_t i = 0; i < order.size(); ++i) {
    if (i != 0) {
      cout << ' ';
    }
    cout << order[i];
  }
  cout << '\n';
}


void print_solution(const vector<int>& assign, int n, int offset) {
  bool first = true;
  for (int i = 1; i <= n; ++i) {
    if (assign[i] == 1) {
      if (!first) {
        cout << ' ';
      }
      cout << (i - offset);
      first = false;
    }
  }
  cout << '\n';
}

void enumerate_all_free_vars(int idx, int n, vector<int>& assign, int offset) {
  if (idx > n) {
    print_solution(assign, n, offset);
    return;
  }
  assign[idx] = 0;
  enumerate_all_free_vars(idx + 1, n, assign, offset);
  assign[idx] = 1;
  enumerate_all_free_vars(idx + 1, n, assign, offset);
}

void enumerate_all_solutions(const BDD& f, int idx, int n, vector<int>& assign,
                             int offset) {
  int top = f.Top();
  if (top == 0) {
    if (f.GetID() == bddtrue) {
      enumerate_all_free_vars(idx, n, assign, offset);
    }
    return;
  }
  if (idx < top) {
    assign[idx] = 0;
    enumerate_all_solutions(f, idx + 1, n, assign, offset);
    assign[idx] = 1;
    enumerate_all_solutions(f, idx + 1, n, assign, offset);
    return;
  }
  assign[idx] = 0;
  enumerate_all_solutions(f.At0(top), idx + 1, n, assign, offset);
  assign[idx] = 1;
  enumerate_all_solutions(f.At1(top), idx + 1, n, assign, offset);
}

long double pow2_ld(int k) {
  return std::ldexp(1.0L, k);
}

long double count_rec(const BDD& f, int prev_level, int n,
                      std::unordered_map<bddword, long double>& memo) {
  bddword id = f.GetID();
  int var = f.Top();
  if (var == 0) {
    return (id == bddtrue) ? pow2_ld(n - prev_level) : 0.0L;
  }
  int lev = BDD_LevOfVar(var);
  int gap = lev - prev_level - 1;
  auto it = memo.find(id);
  if (it != memo.end()) {
    return pow2_ld(gap) * it->second;
  }
  long double c0 = count_rec(f.At0(var), lev, n, memo);
  long double c1 = count_rec(f.At1(var), lev, n, memo);
  long double no_gap = c0 + c1;
  memo[id] = no_gap;
  return pow2_ld(gap) * no_gap;
}

long double count_solutions(const BDD& f, int n) {
  std::unordered_map<bddword, long double> memo;
  memo.reserve(1024);
  return count_rec(f, 0, n, memo);
}
}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  cin.tie(nullptr);

  // Input format: each line is a subset as space-separated 1-based indices.
  // Example line: "1 3 5" represents {1,3,5}.
  vector<vector<int>> sets;
  int max_elem = 0;
  bool has_zero = false;
  cerr << "input reading..." << '\n';
  FastInput in;
  if (!in.ReadSets(sets, max_elem, has_zero)) {
    return 0;
  }
  int offset = has_zero ? 1 : 0;
  if (offset == 1) {
    for (auto& elems : sets) {
      for (int& x : elems) {
        x += 1;
      }
    }
  }
  int n = max_elem + offset;
  // cerr << "Original variable order..." << '\n';
  std::vector<int> identity_order;
  identity_order.reserve(n);
  for (int i = 1; i <= n; ++i) {
    identity_order.push_back(i);
  }
  // print_variable_order(identity_order);
  // cerr << "Computing variable order (Min-Fill)..." << '\n';
  // std::vector<int> order = compute_min_fill_order(sets, n);
  // cerr << "Reordered variable order..." << '\n';
  // print_variable_order(order);
  // apply_variable_order(sets, n, order);

  // Initialize SAPPOROBDD (tune node/cache sizes as needed).
  cerr << "Initializing BDD package..." << '\n';
  BDD_Init(100000000, 100000000);
  for (int i = 0; i < 2 * n; ++i) {
    BDD_NewVar();
  }
  std::vector<BDD> var_bdds(n + 1);
  for (int i = 1; i <= n; ++i) {
    var_bdds[i] = BDDvar(i);
  }

  cerr << "Building BDD..." << '\n';
  BDD b(0);
  for (const auto& elems : sets) {
    BDD term(1);
    for (int x : elems) {
      term &= var_bdds[x];
    }
    b |= term;
  }

  cerr << "Building dual BDD..." << '\n';
  DualBuilder dual_builder;
  BDD dual = dual_builder.Build(b);

  cerr << "Minimalization..." << '\n';
  BDD minimal = minimalize_monotone_increasing(dual, n);
  cerr << "Printing BDD info..." << '\n';
  minimal.Print();
  long double count = count_solutions(minimal, n);
  cout << "Solution count (approx): " << std::setprecision(10)
       << std::scientific << count << '\n';

  cerr << "Printing ZDD info..." << '\n';  
  // cerr << "Enumerating all solutions..." << '\n';
  // vector<int> assign(n + 1, 0);
  // enumerate_all_solutions(minimal, 1, n, assign, offset);
  return 0;
}
