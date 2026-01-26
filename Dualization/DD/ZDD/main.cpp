#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

// SAPPOROBDD headers (adjust include path/name to your local installation).
#include "ZBDD.h"
#include "SBDD_helper.h"

// This implementation uses the SAPPOROBDD C++ wrapper API in ZBDD.h.

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

ZBDD build_superset_family_union(const vector<vector<int>>& sets, int n) {
  ZBDD family(0);
  for (const auto& edge : sets) {
    std::vector<bddvar> targets;
    targets.reserve(edge.size());
    for (int v : edge) {
      if (v >= 1 && v <= n) {
        targets.push_back(v);
      }
    }
    std::sort(targets.begin(), targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());    
    ZBDD b = sbddh::getPowerSetIncluding(n, targets);
    // family = family + sbddh::getPowerSetIncluding(n, targets);
    family.Print();
  }
  return family;
}

ZBDD complement_family(const ZBDD& f, int n) {
  ZBDD comp = f;
  for (int i = 1; i <= n; ++i) {
    comp = comp.Change(i);
  }
  return comp;
}

ZBDD dual_family(const ZBDD& f, int n) {
  ZBDD all = sbddh::getPowerSet(n);
  ZBDD comp = complement_family(f, n);
  return all - comp;
}

ZBDD minimalize_upward_closed(const ZBDD& f, int n) {
  // Remove any set that has a one-element deletion still in f.
  ZBDD result = f;
  for (int i = 1; i <= n; ++i) {
    ZBDD removed = result.OnSet0(i);  // Remove i from sets that contain i.
    ZBDD redundant_base = removed & result;
    ZBDD redundant_sets = redundant_base.Change(i);
    result = result - redundant_sets;
  }
  return result;
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

  // Initialize SAPPOROBDD (tune node/cache sizes as needed).
  cerr << "Initializing ZDD package..." << '\n';
  BDD_Init(100000000, 100000000);
  for (int i = 0; i < n; ++i) {
    BDD_NewVar();
  }

  cerr << "Building superset family (OR of supersets)..." << '\n';
  ZBDD superset_union = build_superset_family_union(sets, n);
  superset_union.Print();

  cerr << "Building dual family..." << '\n';
  ZBDD dual = dual_family(superset_union, n);
  dual.Print();
  cerr << "Minimalization..." << '\n';
  ZBDD minimal = minimalize_upward_closed(dual, n);

  cerr << "Printing ZDD info..." << '\n';
  minimal.Print();

  long double count = minimal.Card();
  cout << "Solution count (approx): " << std::setprecision(10)
       << std::scientific << count << '\n';
  return 0;
}
