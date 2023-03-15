#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

//#define USE_SUPER_OPTIMIZATION true
#define USE_SUPER_OPTIMIZATION false

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
uint64_t current_max_weight_found = 0;
uint8_t vertex_colors_best_solution[50] = {};
int vertex_count;
int vertex_edges[50][50] = {};
uint16_t edges_count = 0;
Edge edges[300] = {};

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
//        next_state_size++;


        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_a(BLUE);
//        next_state_size++;
        return 2;
    }
    else if (a_clr == RED  && b_clr == NO_COLOR) {
        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_b(BLUE);
        next_states[0].add_c_edge();
//        next_state_size++;

        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_b(RED);
//        next_state_size++;
        return 2;
    }
    else if (a_clr == BLUE  && b_clr == NO_COLOR) {
        next_states[0] = c_st.get_next_state();
        next_states[0].do_clr_b(RED);
        next_states[0].add_c_edge();
//        next_state_size++;

        next_states[1] = c_st.get_next_state();
        if (USE_SUPER_OPTIMIZATION) next_states[1].do_clr_b(BLUE);
//        next_state_size++;
        return 2;
    }
}

bool cannot_beat_max_weight(State & st) {
    if (current_max_weight_found > 0) {
        if ((st.weight + calc_sum_of_rest_of_weights(st.c_edge)) < current_max_weight_found) {
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


    if (st.c_edge == edges_count) {
        if (st.weight > current_max_weight_found and check_consistency()) {
            #pragma omp critical // update_best_weight
            {
                // check if this is still the best weight (could have been changed when checking consistency)
                if (st.weight > current_max_weight_found){
                    current_max_weight_found = st.weight;
                    memcpy( vertex_colors_best_solution, st.v_clrs, sizeof(vertex_colors_best_solution) );
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
//    for (int i = count-1; i >= 0; i--) {

        // do this here, so that the State methods can be easily applied to the previous "current edge"
        next_states[i].c_edge++;

        // skip states that cannot be make a consistent graph
        if (cannot_be_consistent(next_states[i])) {
            continue;
        }

        if ((i < count-1) &&  (next_states[i].c_edge / float(edges_count) < 0.20)) {
//        if ((i > 0) &&  (next_states[i].c_edge / float(edges_count) < 0.20)) {
            #pragma omp task
            rec_func(next_states[i]);
        }
        else {
            rec_func(next_states[i]);
        }

//        this is actually slower than the if else upwards
//        #pragma omp task if ((i < count-1) && (next_states[i].c_edge / float(edges_count) < 0.20))
//            rec_func(next_states[i]);
    }

    #pragma omp taskwait
}


int main(int argc, char *argv[]) {
    int core_count;
    std::string filepath;
    /* check input argument */
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

    State init_state;

    #pragma omp parallel //reduction(+:ref_func_call_counter)
    {
        #pragma omp single
        rec_func(init_state);

    }

    std::cout << "{\n";
    std::cout << "\"MAX_WEIGHT\": " << current_max_weight_found << ", \n";
    std::cout << "\"REC_CALLS\": " << ref_func_call_counter << ", \n";

    std::cout << "\"RESULT\": " << "[\n";
    for (int i = 0; i < vertex_count; i++) {
        std::cout << "{";
        std::cout << "\"ID\": " << i << ", ";
        std::cout << "\"COLOR\": " << int(vertex_colors_best_solution[i]); //<< ", ";// << std::endl;
        if (i+1 == vertex_count) std::cout << "}\n";
        else std::cout << "}, \n";
    }
    std::cout << "] " << "\n" << "}" << std::endl;

}

//  g++ -std=c++17 -Wall -pedantic -Wno-long-long -O2 -o main main.cpp && ./main graphs/graf_15_5.txt  1

// g++ -std=c++17  -Wall -pedantic -Wno-long-long -O2 -o ../main_seq ../main.cpp  && g++ -std=c++17 -fopenmp  -Wall -pedantic -Wno-long-long -O2 -o ../main_openmp ../main.cpp && python3 main.py

// TASK 2
// g++ -std=c++17  -Wall -pedantic -Wno-long-long -O2 -o ../main_seq ../main_task.cpp  && g++ -std=c++17 -fopenmp  -Wall -pedantic -Wno-long-long -O2 -o ../main_openmp ../main_task.cpp && python3 main.py
