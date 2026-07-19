# Parallel branch and bound: maximum-weight connected bipartite subgraph

Semester project for the NI-PDP course (Parallel and Distributed Programming) at FIT ČVUT, 2023. The task was set by the course. Given an undirected weighted graph, find the connected bipartite subgraph with the largest total edge weight. I solved it with branch and bound, first sequentially and then in three parallel versions, and benchmarked them on the faculty's cluster.

## Versions

| File | Approach |
|---|---|
| `main_seq.cpp` | Sequential DFS over the edges, heaviest first. Each edge either joins the subgraph (its endpoints get different colours) or is left out. A branch is pruned when the remaining edge weights can't beat the best solution found so far, or when too few edges are left for a connected result. |
| `main_task.cpp` | OpenMP task parallelism: the recursive calls run as OpenMP tasks |
| `main_data.cpp` | OpenMP data parallelism: the tree is expanded breadth-first into a queue of states, which are then solved with `parallel for` |
| `main_mpi.cpp` | MPI + OpenMP: a master process expands states and hands them out to worker processes, which solve them with OpenMP threads |

## Results

Runtimes on `graph-hard/graf-23-20.txt` (23 vertices), measured on the course cluster. Speedup is relative to the sequential version (161.6 s). Efficiency is speedup divided by the number of cores used.

| Version | Processes × threads | Runtime | Speedup | Efficiency |
|---|---|---|---|---|
| Sequential | 1 | 161.6 s | 1.0× | – |
| OpenMP tasks | 20 | 16.4 s | 9.9× | 0.49 |
| OpenMP data | 20 | 10.3 s | 15.7× | 0.79 |
| MPI + OpenMP | 2 × 12 | 17.6 s | 9.2× | 0.38 |
| MPI + OpenMP | 3 × 12 | 11.3 s | 14.3× | 0.40 |
| MPI + OpenMP | 4 × 12 | 6.1 s | 26.5× | 0.55 |

The data-parallel version scaled best on a single node. Branch and bound runtimes depend a lot on how early a good solution is found, so these numbers vary between graphs. Results for more thread counts and graphs are in [`tester/process_results.ipynb`](tester/process_results.ipynb).

## Running

```bash
g++ -std=c++17 -O2 -fopenmp -o main_data main_data.cpp
./main_data graph-hard/graf-23-20.txt 8          # <graph file> <thread count>

mpic++ -std=c++17 -O2 -fopenmp -o main_mpi main_mpi.cpp
mpirun -np 4 ./main_mpi graph-hard/graf-23-20.txt 8
```

An input file holds the vertex count on the first line, followed by the weighted adjacency matrix (0 means no edge). The program prints JSON with the best weight, the runtime and the vertex colouring. `Dockerfile` sets up a build environment with OpenMP and Open MPI. `create_scripts_and_run.py` and the `*_job_TEMPLATE.py` files generate and submit the cluster jobs.
