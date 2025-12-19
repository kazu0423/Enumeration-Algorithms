// mis_reverse_search_mpi.cpp
// Reverse search enumeration of Maximal Independent Sets (MIS)
// based on Eppstein (2004) "All Maximal Independent Sets and Dynamic Dominance
// for Sparse Graphs" MPI parallelization: distribute the root's children by
// (child_index % world_size).
//
// Build: mpicxx -O3 -std=c++17 -march=native mis_reverse_search_mpi.cpp -o
// mis_rs Run : mpirun -np 4 ./mis_rs graph.txt --one-based  (if input vertices
// are 1..n) Input format (default 0-based):
//   n m
//   u v   (m lines, undirected, no self-loops)
//
// Output: each rank prints MIS as sorted vertex list on stdout with a rank
// prefix.

#include <mpi.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Bitset {
  int n = 0;
  int w = 0;
  std::vector<uint64_t> a;

  Bitset() = default;
  explicit Bitset(int n_) { init(n_); }

  void init(int n_) {
    n = n_;
    w = (n + 63) / 64;
    a.assign(w, 0ULL);
  }

  void set(int i) { a[i >> 6] |= (1ULL << (i & 63)); }
  void reset(int i) { a[i >> 6] &= ~(1ULL << (i & 63)); }
  bool test(int i) const { return (a[i >> 6] >> (i & 63)) & 1ULL; }

  void clear() { std::fill(a.begin(), a.end(), 0ULL); }

  bool equals(const Bitset& o) const { return a == o.a; }

  bool empty() const {
    for (uint64_t x : a)
      if (x) return false;
    return true;
  }

  int popcount() const {
    int s = 0;
    for (uint64_t x : a) s += __builtin_popcountll(x);
    return s;
  }

  Bitset operator|(const Bitset& o) const {
    Bitset r(n);
    for (int i = 0; i < w; i++) r.a[i] = a[i] | o.a[i];
    return r;
  }
  Bitset operator&(const Bitset& o) const {
    Bitset r(n);
    for (int i = 0; i < w; i++) r.a[i] = a[i] & o.a[i];
    return r;
  }
  Bitset operator~() const {
    Bitset r(n);
    for (int i = 0; i < w; i++) r.a[i] = ~a[i];
    // mask off extra bits
    int extra = w * 64 - n;
    if (extra > 0) {
      uint64_t mask = (extra == 64) ? 0ULL : (~0ULL >> extra);
      r.a[w - 1] &= mask;
    }
    return r;
  }

  Bitset& operator|=(const Bitset& o) {
    for (int i = 0; i < w; i++) a[i] |= o.a[i];
    return *this;
  }
  Bitset& operator&=(const Bitset& o) {
    for (int i = 0; i < w; i++) a[i] &= o.a[i];
    return *this;
  }

  std::vector<int> to_list_sorted() const {
    std::vector<int> res;
    res.reserve(popcount());
    for (int i = 0; i < n; i++)
      if (test(i)) res.push_back(i);
    return res;
  }
};

struct Graph {
  int n = 0;
  std::vector<Bitset> nb;  // open neighborhood bitset
  Bitset all;              // all-ones mask (size n)

  explicit Graph(int n_ = 0) : n(n_), nb(n_), all(n_) {
    for (int i = 0; i < n; i++) {
      nb[i].init(n);
      all.set(i);
    }
  }
};

static Graph G;

// Greedy lexicographically-first maximal independent superset of 'base'.
// Order is 0,1,2,...,n-1.
static Bitset greedy_complete(Bitset base) {
  for (int v = 0; v < G.n; v++) {
    if (base.test(v)) continue;
    // check v is independent of base: no neighbor in base
    Bitset inter = base & G.nb[v];
    if (inter.empty()) base.set(v);
  }
  return base;  // maximal by construction
}

static bool is_maximal_independent(const Bitset& S) {
  // independence is not re-checked here (assumed by construction), but we could
  // add it if needed.
  Bitset dominated = S;  // vertices in S are "covered" for maximality test
  for (int v = 0; v < G.n; v++)
    if (S.test(v)) dominated |= G.nb[v];
  return dominated.equals(G.all);
}

static Bitset LFMIS;  // lexicographically first MIS
static std::vector<std::vector<int>>
    later;  // later[v] lists vertices assigned to v in LFMIS

static Bitset parent_of(const Bitset& S) {
  // If S == LFMIS, parent is itself (root).
  // Find earliest v in LFMIS \ S.
  int v = -1;
  for (int i = 0; i < G.n; i++) {
    if (LFMIS.test(i) && !S.test(i)) {
      v = i;
      break;
    }
  }
  if (v == -1) return S;  // root

  // N = neighbors(v) ∩ S
  Bitset N = S & G.nb[v];

  // X = (S ∪ {v}) \ N
  Bitset X = S;
  X.set(v);
  X = X & (~N);

  // parent(S) = greedy_complete(X)
  return greedy_complete(X);
}

struct MPIContext {
  int rank = 0;
  int size = 1;
};

// Enumerate all independent (vertex) subsets of 'cand' (given as list) and call
// cb(subset_bitset). This is exponential in |cand|; intended for sparse / small
// later(v).
template <class Callback>
static void enumerate_independent_subsets(
    const std::vector<int>& cand, int idx, Bitset& chosen,
    Bitset& forbidden,  // vertices that cannot be chosen due to adjacency with
                        // chosen
    Callback&& cb) {
  if (idx == (int)cand.size()) {
    if (!chosen.empty()) cb(chosen);
    return;
  }

  int u = cand[idx];

  // Option 1: skip u
  enumerate_independent_subsets(cand, idx + 1, chosen, forbidden, cb);

  // Option 2: take u if allowed (not forbidden)
  if (!forbidden.test(u)) {
    // update
    chosen.set(u);

    Bitset old_forbidden = forbidden;
    forbidden.set(u);
    forbidden |= G.nb[u];

    enumerate_independent_subsets(cand, idx + 1, chosen, forbidden, cb);

    // rollback
    forbidden = std::move(old_forbidden);
    chosen.reset(u);
  }
}

static void output_set(const MPIContext& mpi, const Bitset& S) {
  auto lst = S.to_list_sorted();
  std::cout << "[rank " << mpi.rank << "] MIS(size=" << lst.size() << "):";
  for (int v : lst) std::cout << " " << v;
  std::cout << "\n";
}

// DFS reverse search from node S (serial within a rank)
static void search_dfs(const MPIContext& mpi, const Bitset& S) {
  output_set(mpi, S);

  // For each v in order:
  // if v not in LFMIS continue
  // if v not in S break
  for (int v = 0; v < G.n; v++) {
    if (!LFMIS.test(v)) continue;
    if (!S.test(v)) break;

    const auto& Lv = later[v];
    if (Lv.empty()) continue;

    Bitset chosen(G.n), forbidden(G.n);
    chosen.clear();
    forbidden.clear();

    enumerate_independent_subsets(Lv, 0, chosen, forbidden,
                                  [&](const Bitset& Nset) {
                                    // T = (S ∪ N) \ (neighbors of N)
                                    Bitset neigh_union(G.n);
                                    neigh_union.clear();
                                    for (int u : Nset.to_list_sorted()) {
                                      neigh_union |= G.nb[u];
                                    }
                                    Bitset T = (S | Nset) & (~neigh_union);

                                    if (!is_maximal_independent(T)) return;
                                    Bitset p = parent_of(T);
                                    if (!p.equals(S)) return;

                                    search_dfs(mpi, T);
                                  });
  }
}

// Enumerate root's children in a fixed order, distribute by child_index %
// mpi.size
static void parallel_search_from_root(const MPIContext& mpi) {
  // Root is LFMIS itself
  if (mpi.rank == 0) {
    std::cerr << "Root (LFMIS) size = " << LFMIS.popcount() << "\n";
    std::cerr << "MPI ranks = " << mpi.size << "\n";
  }

  // Output root only from rank 0 (avoid duplicates)
  if (mpi.rank == 0) output_set(mpi, LFMIS);

  long long child_index = 0;

  // generate children of root exactly like in search_dfs, but dispatch each
  // child subtree.
  const Bitset& S = LFMIS;
  for (int v = 0; v < G.n; v++) {
    if (!LFMIS.test(v)) continue;
    // For root, S contains all vertices of LFMIS, so never breaks

    const auto& Lv = later[v];
    if (Lv.empty()) continue;

    Bitset chosen(G.n), forbidden(G.n);
    chosen.clear();
    forbidden.clear();

    enumerate_independent_subsets(Lv, 0, chosen, forbidden,
                                  [&](const Bitset& Nset) {
                                    Bitset neigh_union(G.n);
                                    neigh_union.clear();
                                    for (int u : Nset.to_list_sorted())
                                      neigh_union |= G.nb[u];

                                    Bitset T = (S | Nset) & (~neigh_union);

                                    if (!is_maximal_independent(T)) return;
                                    Bitset p = parent_of(T);
                                    if (!p.equals(S)) return;

                                    long long idx = child_index++;
                                    if ((idx % mpi.size) != mpi.rank) return;

                                    // each rank explores its assigned child
                                    search_dfs(mpi, T);
                                  });
  }

  // A barrier helps flush output more cleanly.
  MPI_Barrier(MPI_COMM_WORLD);
}

static void build_LFMIS_and_later() {
  // LFMIS
  Bitset empty(G.n);
  empty.clear();
  LFMIS = greedy_complete(empty);

  // later(v) partition for v in LFMIS
  later.assign(G.n, {});
  for (int u = 0; u < G.n; u++) {
    if (LFMIS.test(u)) continue;

    int best = -1;
    // earliest neighbor in LFMIS
    for (int v = 0; v < G.n; v++) {
      if (!LFMIS.test(v)) continue;
      if (G.nb[u].test(v)) {
        best = v;
        break;
      }
    }
    if (best != -1) later[best].push_back(u);
    // If best == -1, u has no neighbor in LFMIS; but that contradicts
    // maximality of LFMIS. We'll assert for sanity.
    else {
      std::cerr << "Warning: vertex " << u
                << " has no neighbor in LFMIS (unexpected)\n";
    }
  }

  // sort each later[v] to make enumeration deterministic
  for (auto& vec : later) std::sort(vec.begin(), vec.end());
}

static void read_graph(const std::string& path, bool one_based) {
  std::ifstream in(path);
  if (!in) {
    std::cerr << "Cannot open: " << path << "\n";
    std::exit(1);
  }
  int n, m;
  in >> n >> m;
  G = Graph(n);

  for (int i = 0; i < m; i++) {
    int u, v;
    in >> u >> v;
    if (one_based) {
      --u;
      --v;
    }
    if (u < 0 || v < 0 || u >= n || v >= n) {
      std::cerr << "Edge out of range: " << u << " " << v << "\n";
      std::exit(1);
    }
    if (u == v) continue;
    G.nb[u].set(v);
    G.nb[v].set(u);
  }
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  MPIContext mpi;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi.size);

  if (argc < 2) {
    if (mpi.rank == 0) {
      std::cerr << "Usage: " << argv[0] << " graph.txt [--one-based]\n";
    }
    MPI_Finalize();
    return 0;
  }

  std::string path = argv[1];
  bool one_based = false;
  for (int i = 2; i < argc; i++) {
    std::string a = argv[i];
    if (a == "--one-based") one_based = true;
  }

  read_graph(path, one_based);
  build_LFMIS_and_later();

  // sanity: LFMIS should be maximal
  if (mpi.rank == 0 && !is_maximal_independent(LFMIS)) {
    std::cerr << "Internal error: LFMIS is not maximal.\n";
    MPI_Abort(MPI_COMM_WORLD, 2);
  }

  parallel_search_from_root(mpi);

  MPI_Finalize();
  return 0;
}
