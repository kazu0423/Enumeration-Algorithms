// tsukiyama_mis_mpi.cpp
// Parallel Tsukiyama MIS enumeration (Fig.4) via "reverse-search style" subtree
// distribution. rank0 generates tasks at cutoff depth (i == cutoff) and
// distributes to workers dynamically.
//
// Build: mpicxx -O3 -std=c++17 -march=native tsukiyama_mis_mpi.cpp -o
// tsuki_mis_mpi Run  : mpirun -np 8 ./tsuki_mis_mpi graph.txt --count-only
// --cutoff 40
//
// Input (default 0-based):
//   n m
//   u v (m lines, undirected)
// Options:
//   --one-based     : input vertices are 1..n
//   --count-only    : don't print MIS, just count (recommended)
//   --cutoff K      : task cutoff depth (i index in BACKTRACK), default 40
//   --print-limit L : print at most L MIS per rank (for debugging; ignored in
//   count-only)

#include <mpi.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Graph {
  int n = 0;  // number of vertices, internal indexing 1..n
  std::vector<std::vector<int>> adj;
};

static Graph G;

static bool count_only = true;
static uint64_t print_limit = 0;

static inline void output_current_mis(const std::vector<int>& IS, int rank,
                                      uint64_t& local_printed,
                                      uint64_t& local_count) {
  local_count++;
  if (count_only) return;
  if (print_limit > 0 && local_printed >= print_limit) return;

  std::vector<int> mis;
  mis.reserve(G.n);
  for (int v = 1; v <= G.n; v++)
    if (IS[v] == 0) mis.push_back(v);

  std::cout << "[rank " << rank << "] MIS(size=" << mis.size() << "):";
  for (int v : mis) std::cout << " " << v;
  std::cout << ", local count: " << local_count << "\n";
  local_printed++;
}

// Direct translation of Fig.4 BACKTRACK(i), but purely on passed-in IS (no
// globals).
static void BACKTRACK_seq(std::vector<int>& IS,
                          std::vector<std::vector<int>>& Bucket, int i,
                          int rank, uint64_t& local_printed,
                          uint64_t& local_count) {
  if (i >= G.n) {
    // std::cout << "[rank " << rank << "] local_count: " << local_count << "\n";
    if ((local_count % 1000000) == 0) output_current_mis(IS, rank, local_printed, local_count);
    else local_count++;
    return;
  }

  int x = i + 1;
  int c = 0;

  // C1
  for (int y : G.adj[x]) {
    if (y <= i && IS[y] == 0) c++;
  }

  if (c == 0) {
    // L1/B1/L2
    for (int y : G.adj[x])
      if (y <= i) IS[y] += 1;
    BACKTRACK_seq(IS, Bucket, x, rank, local_printed, local_count);
    for (int y : G.adj[x])
      if (y <= i) IS[y] -= 1;
  } else {
    // B2
    IS[x] = c;
    BACKTRACK_seq(IS, Bucket, x, rank, local_printed, local_count);
    IS[x] = 0;

    // C2/C3
    bool f = true;
    Bucket[x].clear();

    for (int y : G.adj[x]) {
      if (y < 1 || y > i) continue;

      if (IS[y] == 0) {
        Bucket[x].push_back(y);
        for (int z : G.adj[y]) {
          if (z <= i) {
            IS[z] -= 1;
            if (IS[z] == 0) f = false;
          }
        }
      }
      IS[y] += 1;
    }

    // B3
    if (f) BACKTRACK_seq(IS, Bucket, x, rank, local_printed, local_count);

    // L3
    for (int y : G.adj[x])
      if (y <= i) IS[y] -= 1;

    // L4/L5
    for (int y : Bucket[x]) {
      for (int z : G.adj[y])
        if (z <= i) IS[z] += 1;
    }
    Bucket[x].clear();
  }
}

// A task is an entry state of BACKTRACK(i): (i, IS[1..n]) with all Buckets
// empty.
struct Task {
  int i;
  std::vector<int> IS;
};

// rank0: generate tasks by simulating Tsukiyama recursion but cutting off at
// i==cutoff
static void GEN_tasks(std::vector<int>& IS,
                      std::vector<std::vector<int>>& Bucket, int i, int cutoff,
                      std::vector<Task>& out_tasks) {
  if (i >= G.n) {
    // leaf would be a task too, but we can just keep it as a task at i==n
    out_tasks.push_back(Task{i, IS});
    return;
  }
  if (i >= cutoff) {
    out_tasks.push_back(Task{i, IS});
    return;
  }

  int x = i + 1;
  int c = 0;
  for (int y : G.adj[x]) {
    if (y <= i && IS[y] == 0) c++;
  }

  if (c == 0) {
    for (int y : G.adj[x])
      if (y <= i) IS[y] += 1;
    GEN_tasks(IS, Bucket, x, cutoff, out_tasks);
    for (int y : G.adj[x])
      if (y <= i) IS[y] -= 1;
  } else {
    IS[x] = c;
    GEN_tasks(IS, Bucket, x, cutoff, out_tasks);
    IS[x] = 0;

    bool f = true;
    Bucket[x].clear();

    for (int y : G.adj[x]) {
      if (y < 1 || y > i) continue;

      if (IS[y] == 0) {
        Bucket[x].push_back(y);
        for (int z : G.adj[y]) {
          if (z <= i) {
            IS[z] -= 1;
            if (IS[z] == 0) f = false;
          }
        }
      }
      IS[y] += 1;
    }

    if (f) GEN_tasks(IS, Bucket, x, cutoff, out_tasks);

    for (int y : G.adj[x])
      if (y <= i) IS[y] -= 1;

    for (int y : Bucket[x]) {
      for (int z : G.adj[y])
        if (z <= i) IS[z] += 1;
    }
    Bucket[x].clear();
  }
}

static void read_graph_1based(const std::string& path, bool one_based_input) {
  std::ifstream in(path);
  if (!in) {
    std::cerr << "Cannot open: " << path << "\n";
    std::exit(1);
  }
  int n, m;
  in >> n >> m;
  G.n = n;
  G.adj.assign(n + 1, {});
  for (int i = 0; i < m; i++) {
    int u, v;
    in >> u >> v;
    if (!one_based_input) {
      u++;
      v++;
    }  // 0-based -> 1-based
    if (u == v) continue;
    if (u < 1 || u > n || v < 1 || v > n) {
      std::cerr << "Edge out of range: " << u << " " << v << "\n";
      std::exit(1);
    }
    G.adj[u].push_back(v);
    G.adj[v].push_back(u);
  }
  for (int v = 1; v <= n; v++) {
    auto& a = G.adj[v];
    std::sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());
  }
}

static void mpi_send_task(int dest, const Task& t) {
  int n = (int)t.IS.size();
  MPI_Send(&t.i, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
  MPI_Send(&n, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
  MPI_Send(t.IS.data(), n, MPI_INT, dest, 1, MPI_COMM_WORLD);
}

static bool mpi_recv_task(int src, Task& t) {
  MPI_Status st;
  int i;
  MPI_Recv(&i, 1, MPI_INT, src, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
  if (i < 0) return false;  // termination
  int n;
  MPI_Recv(&n, 1, MPI_INT, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  t.i = i;
  t.IS.assign(n, 0);
  MPI_Recv(t.IS.data(), n, MPI_INT, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  return true;
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int rank = 0, size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 2) {
    if (rank == 0) {
      std::cerr << "Usage: " << argv[0]
                << " graph.txt [--one-based] [--count-only] [--cutoff K] "
                   "[--print-limit L]\n";
    }
    MPI_Finalize();
    return 0;
  }

  bool one_based_input = false;
  int cutoff =
      40;  // default: makes enough tasks on many graphs, but safe for n~300

  // defaults: count-only for speed
  count_only = false;
  print_limit = 0;

  for (int i = 2; i < argc; i++) {
    std::string a = argv[i];
    if (a == "--one-based")
      one_based_input = true;
    else if (a == "--count-only")
      count_only = true;
    else if (a == "--cutoff" && i + 1 < argc)
      cutoff = std::stoi(argv[++i]);
    else if (a == "--print-limit" && i + 1 < argc) {
      print_limit = (uint64_t)std::stoull(argv[++i]);
      count_only = false;
    } else if (a == "--print") {
      count_only = false;
    }  // optional
  }

  read_graph_1based(argv[1], one_based_input);

  // worker loop
  if (rank != 0) {
    uint64_t local_count = 0, local_printed = 0;
    std::vector<std::vector<int>> Bucket(G.n + 1);

    // tell master we're ready
    int ready = 1;
    MPI_Send(&ready, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

    while (true) {
      Task t;
      if (!mpi_recv_task(0, t)) break;

      // reset buckets
      for (auto& b : Bucket) b.clear();

      // run subtree
      std::vector<int> IS = t.IS;
      BACKTRACK_seq(IS, Bucket, t.i, rank, local_printed, local_count);

      // report completion + ask for more
      uint64_t msg[2] = {local_count, 1};
      MPI_Send(msg, 2, MPI_UINT64_T, 0, 3, MPI_COMM_WORLD);
    }

    // send final count
    uint64_t done_msg[2] = {local_count, 0};
    MPI_Send(done_msg, 2, MPI_UINT64_T, 0, 4, MPI_COMM_WORLD);
    std::cout << "Finish:[" << rank << "] MIS count = " << local_count << "\n";

    MPI_Finalize();
    return 0;
  }

  // -------- rank0 master --------
  if (rank == 0) {
    // generate tasks
    std::vector<int> IS(G.n + 1, 0);
    std::vector<std::vector<int>> Bucket(G.n + 1);

    std::vector<Task> tasks;
    tasks.reserve(100000);

    // initial call in our sequential code was BACKTRACK(1); keep that
    // convention: Task nodes represent entry to BACKTRACK(i).
    GEN_tasks(IS, Bucket, 1, cutoff, tasks);

    std::cerr << "Generated tasks = " << tasks.size()
              << " at cutoff i=" << cutoff << " with MPI ranks=" << size
              << "\n";

    // dynamic dispatch
    int next_task = 0;
    int active_workers = size - 1;

    uint64_t global_count = 0;

    // wait for workers to be ready, then send initial tasks
    for (int w = 1; w < size; w++) {
      int ready = 0;
      MPI_Recv(&ready, 1, MPI_INT, w, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      if (next_task < (int)tasks.size()) {
        mpi_send_task(w, tasks[next_task++]);
      } else {
        int term = -1;
        MPI_Send(&term, 1, MPI_INT, w, 1, MPI_COMM_WORLD);
      }
    }

    // loop: receive progress and keep feeding tasks
    while (active_workers > 0) {
      MPI_Status st;
      uint64_t msg[2];
      MPI_Recv(msg, 2, MPI_UINT64_T, MPI_ANY_SOURCE, MPI_ANY_TAG,
               MPI_COMM_WORLD, &st);
      int src = st.MPI_SOURCE;

      // msg[0] is sender's local_count so far (cumulative), msg[1] is
      // "wants_more" flag We do NOT add msg[0] each time (it’s cumulative);
      // instead we just remember last value per worker.
      static std::vector<uint64_t> last_count;
      if (last_count.empty()) last_count.assign(size, 0);

      uint64_t delta = msg[0] - last_count[src];
      last_count[src] = msg[0];
      global_count += delta;

      if (st.MPI_TAG == 4) {
        // worker done
        active_workers--;
        continue;
      }

      // feed next task or terminate
      if (next_task < (int)tasks.size()) {
        mpi_send_task(src, tasks[next_task++]);
      } else {
        int term = -1;
        MPI_Send(&term, 1, MPI_INT, src, 1, MPI_COMM_WORLD);
      }
    }

    // master also needs to count its own tasks? In this design, master does not
    // compute subtrees. If you want master to also work, we can easily add
    // that, but keeping master pure simplifies scheduling.

    std::cout << "MIS count = " << global_count << "\n";
  }

  MPI_Finalize();
  return 0;
}
