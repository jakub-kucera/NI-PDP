#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#if __APPLE__
    #include </opt/homebrew/Cellar/open-mpi/4.1.5/include/mpi.h>
#else
    #include <mpi.h>
    #include <omp.h>
#endif

using namespace std;

#define LOG_FILE_PATH "./log.txt"

//#define USE_SUPER_OPTIMIZATION true
#define USE_SUPER_OPTIMIZATION true
//#define MPI_MAX_QUEUE_SIZE 16
#define MPI_MAX_QUEUE_SIZE_MULT 6
//#define MP_MAX_QUEUE_SIZE 100
#define MP_MAX_QUEUE_SIZE_MULT 10
//#define QUEUE_SIZE_PER_THREAD 1000

// MPI tags
#define TAG_DONE 9876
#define TAG_WORK 4321
#define TAG_TERMINATE 5467

#define NO_COLOR 0
#define RED 1
#define BLUE 2

struct Edge {
    uint8_t a_vertex = UINT8_MAX;
    uint8_t b_vertex = UINT8_MAX;
    uint8_t weight;
//    bool is_used = false;

    // generated comparators by Clion
    bool operator<(const Edge &rhs) const {
        return weight < rhs.weight;
    }

    bool operator>(const Edge &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Edge &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Edge &rhs) const {
        return !(*this < rhs);
    }

};

uint64_t ref_func_call_counter = 0;
//uint64_t current_max_weight_found = 0;
//uint8_t vertex_colors_best_solution[50] = {};
int vertex_count;
int vertex_edges[50][50] = {};
uint16_t edges_count = 0;
Edge edges[300] = {};


struct BestSolution {
    uint64_t weight = 0;
    uint8_t vertex_colors[50] = {};


    // comparators generated by CLion
    bool operator==(const BestSolution &rhs) const {
        return weight == rhs.weight;
    }

    bool operator!=(const BestSolution &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const BestSolution &rhs) const {
        return weight < rhs.weight;
    }

    bool operator>(const BestSolution &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const BestSolution &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const BestSolution &rhs) const {
        return !(*this < rhs);
    }
};

BestSolution current_best_solution;

struct State {
//    uint64_t ref_func_call_counter = 0;
    // vertex colors
    uint8_t v_clrs[50] = {};
    // are edges used - currently not needed
//    bool b_edges[300] = {};

    // current edge index
    uint16_t c_edge = 0;

    // number of used edges
    uint16_t used_edges = 0;
    // current max weight
    uint64_t weight = 0;

    void add_c_edge() {
//        b_edges[c_edge] = true;
        weight += edges[c_edge].weight;
        used_edges++;
    }

    State get_next_state() {
//        State new_state = *this;
////        std::copy(std::begin(a), std::end(a), std::begin(b));
////        new_state.c_edge++;
//        return new_state;
        return *this;
    }

    uint8_t a_clr() {
        return v_clrs[edges[c_edge].a_vertex];
    }

    uint8_t b_clr() {
        return v_clrs[edges[c_edge].b_vertex];
    }

    void do_clr_a(uint8_t clr) {
        v_clrs[edges[c_edge].a_vertex] = clr;
    };
    void do_clr_b(uint8_t clr) {
        v_clrs[edges[c_edge].b_vertex] = clr;
    };
};

//MPI_Datatype State_MPI_Type;
//int block_lengths[4] = {50, 1, 1, 1};
//MPI_Datatype types[4] = {MPI_UINT8_T, MPI_UINT16_T, MPI_UINT16_T, MPI_UINT64_T};
//MPI_Aint offsets[4];
//
//offsets[0] = offsetof(struct State, v_clrs);
//offsets[1] = offsetof(struct State, c_edge);
//offsets[2] = offsetof(struct State, used_edges);
//offsets[3] = offsetof(struct State, weight);
//
//MPI_Type_create_struct(4, block_lengths, offsets, types, &State_MPI_Type);
//MPI_Type_commit(&State_MPI_Type);

uint16_t queue_length = 0;
State state_queue[300] = {};



uint64_t calc_sum_of_rest_of_weights(uint16_t current_edge) {
    uint64_t sum = 0;
    for (int i = current_edge; i < edges_count; i++) {
        sum += edges[i].weight;
    }
    return sum;
}


void color_graph_rec(uint8_t vertex, uint8_t * vertex_colors_tmp) {
    if (vertex_colors_tmp[vertex] == RED) {
        return;
    }

    vertex_colors_tmp[vertex] = RED;

//    for (int i = 0; i < vertex_count - vertex - 1; i++) {
//        if (vertex_edges[vertex][vertex_count - i - 1] > 0) {
    for (int i = 0; i < vertex_count; i++) {
        if (vertex_edges[vertex][i] > 0) {
            color_graph_rec(i, vertex_colors_tmp);
        }
    }

}

bool check_consistency() {
    uint8_t start_id = 0;
    uint8_t vertex_colors_tmp[50] = {};

    // reset tmp array for consistency check
    for (int i = 0; i < vertex_count; i++) {
        vertex_colors_tmp[i] = NO_COLOR;
    }

    // color
    color_graph_rec(start_id, vertex_colors_tmp);

    // check if all colored
    for (int i = 0; i < vertex_count; i++) {
        if (vertex_colors_tmp[i] == NO_COLOR) {
            return false;
        }
    }
    return true;
}

uint8_t gen_next_states(State c_st, State * next_states) {
    // colors of both adjacent vertices
    uint8_t a_clr = c_st.a_clr();
    uint8_t b_clr = c_st.b_clr();

//    uint8_t next_state_size = 0;

    // check vertexes - 1 already colored, both colored (same, different)
    if (a_clr == NO_COLOR  && b_clr == NO_COLOR) {
        // color RED - BLUE
        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_a(RED);
        next_states[0].do_clr_b(BLUE);
        next_states[0].add_c_edge();
//        next_state_size++;

        // color BLUE - RED
        next_states[1] = c_st.get_next_state();
        next_states[1].do_clr_a(BLUE);
        next_states[1].do_clr_b(RED);
        next_states[1].add_c_edge();
//        next_state_size++;

        if (USE_SUPER_OPTIMIZATION){
            next_states[2] = c_st.get_next_state();
            next_states[2].do_clr_a(BLUE);
            next_states[2].do_clr_b(BLUE);
//            next_state_size++;

            next_states[3] = c_st.get_next_state();
            next_states[3].do_clr_a(RED);
            next_states[3].do_clr_b(RED);
//            next_state_size++;
            return 4;
        }
        else {
            next_states[2] = c_st.get_next_state();
//            next_state_size++;
            return 3;
        }
    }
    else if (a_clr == b_clr) {
        next_states[0] = c_st.get_next_state();
        return 1;
//        next_state_size++;
    }
    else if ((a_clr == RED  && b_clr == BLUE) or
             (a_clr == BLUE  && b_clr == RED)) {

        next_states[0] = c_st.get_next_state();
        next_states[0].add_c_edge();
        return 1;
//        next_state_size++;
    }
    else if (a_clr == NO_COLOR  && b_clr == RED) {

        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_a(BLUE);
        next_states[0].add_c_edge();
//        next_state_size++;

        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_a(RED);
//        next_state_size++;
        return 2;
    }
    else if (a_clr == NO_COLOR  && b_clr == BLUE) {

        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_a(RED);
        next_states[0].add_c_edge();


        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_a(BLUE);
        return 2;
    }
    else if (a_clr == RED  && b_clr == NO_COLOR) {
        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_b(BLUE);
        next_states[0].add_c_edge();

        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_b(RED);
        return 2;
    }
    else if (a_clr == BLUE  && b_clr == NO_COLOR) {
        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_b(RED);
        next_states[0].add_c_edge();

        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_b(BLUE);
        return 2;
    }
}

bool cannot_beat_max_weight(State & st) {
//    if (current_max_weight_found > 0) {
//        if ((st.weight + calc_sum_of_rest_of_weights(st.c_edge)) < current_max_weight_found) {
//            return true;
//        }
//    }
//    return false;
    if (current_best_solution.weight > 0) {
        if ((st.weight + calc_sum_of_rest_of_weights(st.c_edge)) < current_best_solution.weight) {
            return true;
        }
    }
    return false;
}

bool cannot_be_consistent(State & st) {
//    if ((used_edges + (edges_count - (current_edge-1))) < vertex_count-1) {
    return ((st.used_edges + (edges_count - (st.c_edge))) < vertex_count-1);
}


void rec_func(State & st) {
//    #pragma omp atomic update
//    ref_func_call_counter++;

    // check for
    // >> "Větev ukončíme, i pokud v daném mezistavu zjistíme, že výsledný podgraf nebude souvislý, neboť pro souvislý podgraf musí platit |E|>=|V|-1."
//    if (cannot_be_consistent(st[i])) {
//        return;
//    }


//    if (st.c_edge == edges_count) {
//        if (st.weight > current_max_weight_found and check_consistency()) {
//            #pragma omp critical // update_best_weight
//            {
//                // check if this is still the best weight (could have been changed when checking consistency)
//                if (st.weight > current_max_weight_found){
//                    current_max_weight_found = st.weight;
//                    memcpy( vertex_colors_best_solution, st.v_clrs, sizeof(vertex_colors_best_solution) );
//                }
//            }
//        }
//        return;
//    }

    if (st.c_edge == edges_count) {
        if (st.weight > current_best_solution.weight and check_consistency()) {
        #pragma omp critical // update_best_weight
            {
                // check if this is still the best weight (could have been changed when checking consistency)
                if (st.weight > current_best_solution.weight){
                    current_best_solution.weight = st.weight;
                    memcpy( current_best_solution.vertex_colors, st.v_clrs, sizeof(current_best_solution.vertex_colors) );
                }
            }
        }
        return;
    }

    if (cannot_beat_max_weight(st))
        return;

    State next_states[4] = {};
    uint8_t count = gen_next_states(st, next_states);


    for (uint8_t i = 0; i < count; i++) {

        // do this here, so that the State methods can be easily applied to the previous "current edge"
        next_states[i].c_edge++;

        // skip states that cannot be make a consistent graph
        if (cannot_be_consistent(next_states[i])) {
            continue;
        }

        rec_func(next_states[i]);

    }

//    #pragma omp taskwait
}



//void create_init_tasks_mpi(MPI_Datatype & MPI_STATE, int num_procs) {  // TODO try
void create_init_tasks_mpi(MPI_Datatype MPI_STATE, MPI_Datatype MPI_BEST_SOLUTION, int num_procs, ofstream & myfile) {
    std::queue<State> queue;
    queue.emplace();
//    while(queue.size() < MPI_MAX_QUEUE_SIZE) {
    while(queue.size() < MPI_MAX_QUEUE_SIZE_MULT * num_procs) {
        State c_st = queue.front();
        queue.pop();

        State next_states[4] = {};
        uint8_t count = gen_next_states(c_st, next_states);
        for (uint8_t i = 0; i < count; i++) {
            next_states[i].c_edge++; // TODO move
            queue.push(next_states[i]);
        }
    }

    State items[MPI_MAX_QUEUE_SIZE_MULT*num_procs +5];

    int items_size = 0;
    while(!queue.empty()) {
        items[items_size++] = queue.front();
        queue.pop();
    }

    // TODO coudl random sort

    int queue_idx = 0;

//    std::cout << "from MASTER before sending" << std::endl;

    // inspired by https://courses.fit.cvut.cz/NI-PDP/media/lectures/NI-PDP-Prednaska06-MPIIntro.pdf
    for (int dest = 1; dest < num_procs; dest++) {  //send the work to Ss
        myfile << "MASTER: SENDING task to SLAVE #" << dest << endl;
        MPI_Ssend(&items[queue_idx++], 1, MPI_STATE, dest, TAG_WORK, MPI_COMM_WORLD);
//        MPI_Send(&items[queue_idx++], 1, MPI_STATE, dest, TAG_WORK, MPI_COMM_WORLD);
//        MPI_Send(&items[queue_idx], ...(1?), MPI_STATE, dest, TAG_WORK, MPI_COMM_WORLD);
    }
    int working_slaves = num_procs - 1; // the # of working Ss
    while (working_slaves > 0) { // main loop
        MPI_Status status;

        BestSolution local_solution;
        MPI_Recv(&local_solution, 1, MPI_BEST_SOLUTION, MPI_ANY_SOURCE, TAG_DONE, MPI_COMM_WORLD, &status);
        if (local_solution > current_best_solution) {
            myfile << "MASTER: RECEIVED finished task from SLAVE #" << status.MPI_SOURCE << " with current BEST SOLUTION" << endl;
            current_best_solution = local_solution;
            current_best_solution.weight = local_solution.weight;
        }
        else {
            myfile << "MASTER: RECEIVED finished task from SLAVE #" << status.MPI_SOURCE << " with worse solution" << endl;
        }
//        MPI_Recv(0, 0, 0, MPI_ANY_SOURCE, TAG_DONE, MPI_COMM_WORLD, &status);
        // TODO
//        MPI_Recv(best_solution, 0, MPI_SOLUTION, MPI_ANY_SOURCE, TAG_DONE, MPI_COMM_WORLD, &status);
        if (queue_idx < items_size) {
            myfile << "MASTER: SENDING another task to SLAVE #" << status.MPI_SOURCE << endl;
            MPI_Ssend(&items[queue_idx++], 1, MPI_STATE, status.MPI_SOURCE, TAG_WORK, MPI_COMM_WORLD);
//            MPI_Send(&items[queue_idx++], 1, MPI_STATE, status.MPI_SOURCE, TAG_WORK, MPI_COMM_WORLD);
        }
        else {
            myfile << "MASTER: SENDING TERMINATE signal to SLAVE #" << status.MPI_SOURCE << endl;
            MPI_Ssend(nullptr, 0, MPI_STATE, status.MPI_SOURCE, TAG_TERMINATE, MPI_COMM_WORLD);
//            MPI_Send(nullptr, 0, MPI_STATE, status.MPI_SOURCE, TAG_TERMINATE, MPI_COMM_WORLD);
//            MPI_Send(nullptr, 0, nullptr, status.MPI_SOURCE, TAG_TERMINATE, MPI_COMM_WORLD);
//            MPI_Send(..., status.MPI_SOURCE, TAG_TERMINATE, MPI_COMM_WORLD);
            working_slaves--;
        }
    }

}

//void create_init_tasks_mp(std::queue<State> & queue) {
void create_init_tasks_mp(State state, int num_of_threads) {
    std::queue<State> queue;
    queue.push(state);
    while(queue.size() < MP_MAX_QUEUE_SIZE_MULT * num_of_threads) {
        State c_st = queue.front();
        queue.pop();

        State next_states[4] = {};
        uint8_t count = gen_next_states(c_st, next_states);
        for (uint8_t i = 0; i < count; i++) {
            next_states[i].c_edge++; // TODO move
            queue.push(next_states[i]);
        }
    }

    State items[MP_MAX_QUEUE_SIZE_MULT * num_of_threads + 5];

    int items_size = 0;
    while(!queue.empty()) {
        items[items_size++] = queue.front();
        queue.pop();
    }

//    #pragma omp parallel num_threads(num_of_threads)
    {
        # pragma omp parallel for schedule (dynamic, 1)
        for (int i = 0; i < items_size; i++) {
            rec_func(items[i]);
        }
    }

}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int proc_num, num_procs;
    int core_count;
    std::string filepath;
    if (argc != 3) {
        return 1;
    }
    core_count = stoi(argv[2]);
    filepath = std::string(argv[1]);

    // load graph
    std::ifstream infile(filepath);
    infile >> vertex_count;

    // first load all edges data into memory
    for (int i = 0; i < vertex_count; i++) {
        for (int j = 0; j < vertex_count; j++) {
            int weight;
            infile >> weight;
            vertex_edges[i][j] = weight;
//            }
        }
    }

    // go thought "lower triangle" of neighborhood matrix to only store each edge once.
    for (int i = 0; i < vertex_count; i++) {
        for (int j = 0; j < i; j++) {
            int weight = vertex_edges[i][j];
            if (weight >= 80 && weight <= 120) {
                edges[edges_count].a_vertex = i;
                edges[edges_count].b_vertex = j;
                edges[edges_count].weight = weight;
                edges_count++;
//                std::cout << "orig: " << i << ", dest: " << j << std::endl;
//                std::cout << "Weight: " << weight << std::endl;

            }
        }
    }

    // sort edges by weight
    std::sort(std::rbegin(edges), std::rend(edges));
//    std::cout << "after sorting" << std::endl;

//    MPI_Init(&argc, &argv);
//    int proc_num, num_procs;

//    double start_time = omp_get_wtime();

    ofstream myfile;
    myfile.open(LOG_FILE_PATH, ios::out | ios::app);


    MPI_Comm_rank(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  // TODO split queue into this many parts



    MPI_Datatype MPI_STATE;
    int block_lengths[4] = {50, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_UINT8_T, MPI_UINT16_T, MPI_UINT16_T, MPI_UINT64_T};
    MPI_Aint offsets[4] = {
            offsetof(struct State, v_clrs),
            offsetof(struct State, c_edge),
            offsetof(struct State, used_edges),
            offsetof(struct State, weight),
    };
    MPI_Type_create_struct(4, block_lengths, offsets, types, &MPI_STATE);
    MPI_Type_commit(&MPI_STATE);



    MPI_Datatype MPI_BEST_SOLUTION;
    int bs_block_lengths[2] = {1, 50, };
    MPI_Datatype bs_types[2] = {MPI_UINT64_T,MPI_UINT8_T};
//    MPI_Aint offsets[4];
    MPI_Aint bs_offsets[2] = {
            offsetof(struct BestSolution, weight),
            offsetof(struct BestSolution, vertex_colors),
    };

    MPI_Type_create_struct(2, bs_block_lengths, bs_offsets, bs_types, &MPI_BEST_SOLUTION);
    MPI_Type_commit(&MPI_BEST_SOLUTION);


    if (proc_num == 0) {

        double start_time = omp_get_wtime();
        // find solution
        myfile << "MASTER: before sending tasks" << endl;
//        std::cout << "from MASTER before init tasks" << std::endl;
        create_init_tasks_mpi(MPI_STATE, MPI_BEST_SOLUTION, num_procs, myfile);

        double duration = omp_get_wtime() - start_time;

        std::cout << "{\n";
        std::cout << "\"MAX_WEIGHT\": " << current_best_solution.weight << ", \n";
        std::cout << "\"REC_CALLS\": " << ref_func_call_counter << ", \n";
        std::cout << "\"VERSION\": " << "\"OPEN_MPI\"" << ", \n";
        std::cout << "\"PROCESS_COUNT\": " << num_procs << ", \n";
        std::cout << "\"THREAD_COUNT\": " << core_count << ", \n";
        std::cout << "\"FILE\": \"" << filepath << "\", \n";
        std::cout << "\"RUNTIME\": " << duration << ", \n";

        std::cout << "\"RESULT\": " << "[\n";
        for (int i = 0; i < vertex_count; i++) {
            std::cout << "{";
            std::cout << "\"ID\": " << i << ", ";
            std::cout << "\"COLOR\": " << int(current_best_solution.vertex_colors[i]); //<< ", ";// << std::endl;
            if (i+1 == vertex_count) std::cout << "}\n";
            else std::cout << "}, \n";
        }
        std::cout << "] " << "\n" << "}" << std::endl;
    }
    else {
//        std::cout << "from SLAVE before init tasks" << std::endl;
        myfile << "SLAVE #" << proc_num << ": before receiving tasks" << endl;
        while (true) {
            //receive a message - work assignment or termination
            MPI_Status status;
            State message;
//            MPI_STATE message = {};
//            message.
            MPI_Recv(&message, 1, MPI_STATE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//            std::cout << "from SLAVE after receive" << std::endl;
            if (status.MPI_TAG == TAG_TERMINATE) {
                myfile << "SLAVE #" << proc_num << ": RECEIVED message with tag TAG_TERMINATE" << endl;
                // TODO
//                MPI_Reduce (result,...); //reduce local results to Master
//                cout << "SLAVE terminating" << endl;
                break;
            }
            else if (status.MPI_TAG == TAG_WORK) {
                myfile << "SLAVE #" << proc_num << ": RECEIVED message with tag TAG_WORK" << endl;
//                [C,i] = message;
//                cout << "SLAVE received task" << endl;
                create_init_tasks_mp(message, core_count);
//                create_init_tasks_mp(MPI_STATE, message);
//                cout << "SLAVE finished 1 task" << endl;
                // TODO send state with best solution. Or just send the best solution
                myfile << "SLAVE #" << proc_num << ": SENDING message with tag TAG_DONE" << endl;
                MPI_Ssend(&current_best_solution, 1, MPI_BEST_SOLUTION, 0, TAG_DONE, MPI_COMM_WORLD); //report completion to}
//                MPI_Send(&current_best_solution, 1, MPI_BEST_SOLUTION, 0, TAG_DONE, MPI_COMM_WORLD); //report completion to}
            }
        }
    }

    myfile.close();
    MPI_Finalize();

//    std::cout << "{\n";
//    std::cout << "\"MAX_WEIGHT\": " << current_best_solution.weight << ", \n";
//    std::cout << "\"REC_CALLS\": " << ref_func_call_counter << ", \n";
//
//    std::cout << "\"RESULT\": " << "[\n";
//    for (int i = 0; i < vertex_count; i++) {
//        std::cout << "{";
//        std::cout << "\"ID\": " << i << ", ";
//        std::cout << "\"COLOR\": " << int(current_best_solution.vertex_colors[i]); //<< ", ";// << std::endl;
//        if (i+1 == vertex_count) std::cout << "}\n";
//        else std::cout << "}, \n";
//    }
//    std::cout << "] " << "\n" << "}" << std::endl;

}

//  g++ -std=c++17 -Wall -pedantic -Wno-long-long -O2 -o main main.cpp && ./main graphs/graf_15_5.txt  1

// g++ -std=c++17  -Wall -pedantic -Wno-long-long -O2 -o ../main_seq ../main.cpp  && g++ -std=c++17 -fopenmp  -Wall -pedantic -Wno-long-long -O2 -o ../main_openmp ../main.cpp && python3 main.py

// TASK 2
// g++ -std=c++17  -Wall -pedantic -Wno-long-long -O2 -o ../main_seq ../main_task.cpp  && g++ -std=c++17 -fopenmp  -Wall -pedantic -Wno-long-long -O2 -o ../main_openmp ../main_task.cpp && python3 main.py


// /work/tester# OMPI_CXX=g++ mpic++ -o main_mpi  ../main.cpp
// mpirun --allow-run-as-root  -np 4 ./main_mpi
// OMPI_CXX=g++ mpic++ -fopenmp -O2 -o ../main_mpi  ../main.cpp  && mpirun --allow-run-as-root  -np 4 ../main_mpi   ../graphs/graf_12_9.txt 1 &&
// OMPI_CXX=g++ mpic++ -fopenmp -O2 -o ../main_mpi  ../main.cpp  && python3 main.py

// OMPI_CXX=g++ mpic++ -fopenmp -O2 -o ../main_mpi  ../main_mpi.cpp
//  OMP_NUM_THREADS=6  mpirun --allow-run-as-root  -np 4 ../main_mpi  ../graphs/graf_12_9.txt  4