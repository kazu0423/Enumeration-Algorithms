//////////////////////////////////////////////////////////////////////////////
/*                                                                           */
/* Reverse-search LCM for closed frequent itemsets with MPI parallelization. */
/*                                                                           */
///////////////////////////////////////////////////////////////////////////////

#include <mpi.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct Bitset {
  std::vector<uint64_t> words;
  explicit Bitset(size_t n = 0) : words(n, 0) {}
};

int popcount(const Bitset& bs) {
  int total = 0;
  for (uint64_t w : bs.words) {
    total += __builtin_popcountll(w);
  }
  return total;
}

Bitset make_full_bitset(int n_bits) {
  const size_t words = static_cast<size_t>((n_bits + 63) / 64);
  Bitset bs(words);
  for (size_t i = 0; i < words; ++i) {
    bs.words[i] = ~0ULL;
  }
  if (n_bits % 64 != 0) {
    const uint64_t mask = (1ULL << (n_bits % 64)) - 1ULL;
    bs.words.back() = mask;
  }
  return bs;
}

Bitset and_bitsets(const Bitset& a, const Bitset& b) {
  Bitset out(a.words.size());
  for (size_t i = 0; i < a.words.size(); ++i) {
    out.words[i] = a.words[i] & b.words[i];
  }
  return out;
}

bool is_subset_of(const Bitset& a, const Bitset& b) {
  for (size_t i = 0; i < a.words.size(); ++i) {
    if ((a.words[i] & ~b.words[i]) != 0ULL) {
      return false;
    }
  }
  return true;
}

std::vector<int> compute_closure_bitset(const std::vector<Bitset>& item_tids,
                                        const Bitset& tids) {
  std::vector<int> closure;
  closure.reserve(item_tids.size());
  for (size_t item = 0; item < item_tids.size(); ++item) {
    if (is_subset_of(tids, item_tids[item])) {
      closure.push_back(static_cast<int>(item));
    }
  }
  return closure;
}

std::vector<int> intersect_lists(const std::vector<int>& a,
                                 const std::vector<int>& b) {
  std::vector<int> out;
  out.reserve(std::min(a.size(), b.size()));
  size_t i = 0;
  size_t j = 0;
  while (i < a.size() && j < b.size()) {
    if (a[i] == b[j]) {
      out.push_back(a[i]);
      ++i;
      ++j;
    } else if (a[i] < b[j]) {
      ++i;
    } else {
      ++j;
    }
  }
  return out;
}

bool is_subset_list(const std::vector<int>& small, const std::vector<int>& big) {
  size_t i = 0;
  size_t j = 0;
  while (i < small.size() && j < big.size()) {
    if (small[i] == big[j]) {
      ++i;
      ++j;
    } else if (small[i] > big[j]) {
      ++j;
    } else {
      return false;
    }
  }
  return i == small.size();
}

std::vector<int> compute_closure_list(
    const std::vector<std::vector<int>>& item_tids,
    const std::vector<int>& tids) {
  std::vector<int> closure;
  closure.reserve(item_tids.size());
  for (size_t item = 0; item < item_tids.size(); ++item) {
    if (is_subset_list(tids, item_tids[item])) {
      closure.push_back(static_cast<int>(item));
    }
  }
  return closure;
}

std::string format_itemset(const std::vector<int>& itemset, int support) {
  std::ostringstream oss;
  for (size_t i = 0; i < itemset.size(); ++i) {
    if (i != 0) {
      oss << ' ';
    }
    oss << itemset[i];
  }
  oss << " (" << support << ")";
  return oss.str();
}

void emit_itemset(const std::string& line,
                  bool stream,
                  std::vector<std::string>& output) {
  if (stream) {
    std::cout << line << '\n';
    std::cout.flush();
  } else {
    output.push_back(line);
  }
}

void enumerate_lcm(const std::vector<Bitset>& item_tids,
                   const std::vector<int>& prefix,
                   const Bitset& tids,
                   int prev_item,
                   int n_items,
                   int minsup,
                   bool distribute,
                   int rank,
                   int world_size,
                   bool stream,
                   std::vector<std::string>& output,
                   int& local_count) {
  std::vector<char> in_prefix(n_items, 0);
  for (int item : prefix) {
    in_prefix[item] = 1;
  }

  for (int item = prev_item + 1; item < n_items; ++item) {
    if (distribute && (item % world_size != rank)) {
      continue;
    }
    Bitset new_tids = and_bitsets(tids, item_tids[item]);
    const int support = popcount(new_tids);
    if (support < minsup) {
      continue;
    }
    std::vector<int> closure = compute_closure_bitset(item_tids, new_tids);

    int min_new = 0;
    for (int t_item : closure) {
      if (!in_prefix[t_item]) {
        min_new = t_item;
        break;
      }
    }
    if (min_new != item) {
      continue;
    }

    std::vector<int> next_prefix = prefix;
    for (int t_item : closure) {
      if (!in_prefix[t_item]) {
        next_prefix.push_back(t_item);
      }
    }

    emit_itemset(format_itemset(next_prefix, support), stream, output);
    local_count++;
    enumerate_lcm(item_tids, next_prefix, new_tids, next_prefix.back(), n_items,
                  minsup, false, rank, world_size, stream, output, local_count);
  }
}

void enumerate_lcm_list(const std::vector<std::vector<int>>& item_tids,
                        const std::vector<int>& prefix,
                        const std::vector<int>& tids,
                        int prev_item,
                        int n_items,
                        int minsup,
                        bool distribute,
                        int rank,
                        int world_size,
                        bool stream,
                        std::vector<std::string>& output,
                        int& local_count) {
  std::vector<char> in_prefix(n_items, 0);
  for (int item : prefix) {
    in_prefix[item] = 1;
  }

  for (int item = prev_item + 1; item < n_items; ++item) {
    if (distribute && (item % world_size != rank)) {
      continue;
    }
    std::vector<int> new_tids = intersect_lists(tids, item_tids[item]);
    const int support = static_cast<int>(new_tids.size());
    if (support < minsup) {
      continue;
    }
    std::vector<int> closure = compute_closure_list(item_tids, new_tids);

    int min_new = 0;
    for (int t_item : closure) {
      if (!in_prefix[t_item]) {
        min_new = t_item;
        break;
      }
    }
    if (min_new != item) {
      continue;
    }

    std::vector<int> next_prefix = prefix;
    for (int t_item : closure) {
      if (!in_prefix[t_item]) {
        next_prefix.push_back(t_item);
      }
    }

    emit_itemset(format_itemset(next_prefix, support), stream, output);
    local_count++;
    enumerate_lcm_list(item_tids, next_prefix, new_tids, next_prefix.back(),
                       n_items, minsup, false, rank, world_size, stream, output,
                       local_count);
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0;
  int world_size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  bool stream = false;
  std::string mode = "auto";
  for (int i = 2; i < argc; ++i) {
    std::string opt = argv[i];
    if (opt == "--stream" || opt == "-s") {
      stream = true;
    } else if (opt == "--mode" && i + 1 < argc) {
      mode = argv[++i];
    } else if (opt.rfind("--mode=", 0) == 0) {
      mode = opt.substr(7);
    } else {
      if (rank == 0) {
        std::cerr << "Error: unknown option: " << opt << "\n";
      }
      MPI_Finalize();
      return 1;
    }
  }
  if (argc < 2) {
    if (rank == 0) {
      std::cerr << "Error: usage: " << argv[0]
                << " <input-file> [--stream|-s] [--mode auto|sparse|dense]\n";
    }
    MPI_Finalize();
    return 1;
  }
  if (mode != "auto" && mode != "sparse" && mode != "dense") {
    if (rank == 0) {
      std::cerr << "Error: --mode must be auto, sparse, or dense.\n";
    }
    MPI_Finalize();
    return 1;
  }

  int n_items = 0;
  int n_transactions = 0;
  int minsup = 0;
  long long total_len = 0;
  std::vector<int> tx_sizes;
  std::vector<int> flat_items;

  if (rank == 0) {
    std::ifstream ist(argv[1]);
    if (!ist) {
      std::cerr << "can't open input file: " << argv[1] << "\n";
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::string line;
    int max_item = -1;
    while (std::getline(ist, line)) {
      std::istringstream iss(line);
      std::vector<int> items;
      int item = 0;
      while (iss >> item) {
        if (item >= 0) {
          items.push_back(item);
          if (item > max_item) {
            max_item = item;
          }
        }
      }
      if (items.empty()) {
        continue;
      }
      std::sort(items.begin(), items.end());
      items.erase(std::unique(items.begin(), items.end()), items.end());
      tx_sizes.push_back(static_cast<int>(items.size()));
      total_len += static_cast<long long>(items.size());
      flat_items.insert(flat_items.end(), items.begin(), items.end());
    }
    n_transactions = static_cast<int>(tx_sizes.size());
    n_items = max_item + 1;

    std::cerr << "minsup? " << std::endl;
    std::cin >> minsup;
  }

  MPI_Bcast(&n_items, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_transactions, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&minsup, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&total_len, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

  int flat_size = static_cast<int>(flat_items.size());
  MPI_Bcast(&flat_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank != 0) {
    tx_sizes.resize(n_transactions);
    flat_items.resize(flat_size);
  }
  MPI_Bcast(tx_sizes.data(), n_transactions, MPI_INT, 0, MPI_COMM_WORLD);
  if (flat_size > 0) {
    MPI_Bcast(flat_items.data(), flat_size, MPI_INT, 0, MPI_COMM_WORLD);
  }

  const int word_count = (n_transactions + 63) / 64;
  std::vector<Bitset> item_tids_bits(n_items, Bitset(word_count));
  std::vector<std::vector<int>> item_tids_list(n_items);
  int offset = 0;
  for (int tid = 0; tid < n_transactions; ++tid) {
    const int len = tx_sizes[tid];
    const uint64_t mask = 1ULL << (tid % 64);
    const int word = tid / 64;
    for (int j = 0; j < len; ++j) {
      const int item = flat_items[offset + j];
      if (item >= 0 && item < n_items) {
        item_tids_bits[item].words[word] |= mask;
        item_tids_list[item].push_back(tid);
      }
    }
    offset += len;
  }

  const auto start = std::chrono::high_resolution_clock::now();
  Bitset root_tids_bits = make_full_bitset(n_transactions);
  std::vector<int> root_tids_list(n_transactions);
  std::iota(root_tids_list.begin(), root_tids_list.end(), 0);
  const double avg_len =
      n_transactions > 0 ? static_cast<double>(total_len) / n_transactions
                         : 0.0;
  const double density = n_items > 0 ? avg_len / n_items : 0.0;
  std::string resolved_mode = mode;
  if (mode == "auto") {
    resolved_mode = density >= 0.1 ? "dense" : "sparse";
  }

  std::vector<int> root_closure =
      resolved_mode == "dense" ? compute_closure_bitset(item_tids_bits,
                                                       root_tids_bits)
                               : compute_closure_list(item_tids_list,
                                                      root_tids_list);
  std::vector<std::string> local_output;
  int local_count = 0;

  if (minsup <= n_transactions) {
    if (rank == 0) {
      emit_itemset(format_itemset(root_closure, n_transactions), stream,
                   local_output);
      local_count++;
    }
    int prev_item = root_closure.empty() ? -1 : root_closure.back();
    if (resolved_mode == "dense") {
      enumerate_lcm(item_tids_bits, root_closure, root_tids_bits, prev_item,
                    n_items, minsup, true, rank, world_size, stream,
                    local_output, local_count);
    } else {
      enumerate_lcm_list(item_tids_list, root_closure, root_tids_list, prev_item,
                         n_items, minsup, true, rank, world_size, stream,
                         local_output, local_count);
    }
  }

  int total_count = 0;
  MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (stream) {
    if (rank == 0) {
      std::cerr << "total itemsets = " << total_count << "\n";
      const auto end = std::chrono::high_resolution_clock::now();
      const auto diff =
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      std::cerr << "elapsed time = " << diff.count() << " msec.\n";
    }
    MPI_Finalize();
    return 0;
  }

  std::string local_blob;
  if (!local_output.empty()) {
    for (size_t i = 0; i < local_output.size(); ++i) {
      local_blob.append(local_output[i]);
      local_blob.push_back('\n');
    }
  }

  int local_len = static_cast<int>(local_blob.size());
  std::vector<int> recv_counts;
  std::vector<int> displs;
  if (rank == 0) {
    recv_counts.resize(world_size, 0);
  }
  MPI_Gather(&local_len, 1, MPI_INT,
             rank == 0 ? recv_counts.data() : nullptr, 1, MPI_INT,
             0, MPI_COMM_WORLD);

  std::vector<char> recvbuf;
  // int total_len = 0;
  if (rank == 0) {
    displs.resize(world_size, 0);
    for (int i = 0; i < world_size; ++i) {
      displs[i] = total_len;
      total_len += recv_counts[i];
    }
    recvbuf.resize(total_len);
  }

  MPI_Gatherv(local_len ? local_blob.data() : nullptr, local_len, MPI_CHAR,
              rank == 0 ? recvbuf.data() : nullptr,
              rank == 0 ? recv_counts.data() : nullptr,
              rank == 0 ? displs.data() : nullptr,
              MPI_CHAR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    if (!recvbuf.empty()) {
      std::cout.write(recvbuf.data(), recvbuf.size());
    }
    std::cerr << "total itemsets = " << total_count << "\n";
    const auto end = std::chrono::high_resolution_clock::now();
    const auto diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cerr << "elapsed time = " << diff.count() << " msec.\n";
  }

  MPI_Finalize();
  return 0;
}
