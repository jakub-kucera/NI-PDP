#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


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
//uint8_t vertex_colors[50] = {};
uint8_t vertex_colors_best_solution[50] = {};
uint8_t vertex_colors_tmp[50] = {};
int vertex_count;
int vertex_edges[50][50] = {};
uint16_t edges_count = 0;
Edge edges[300] = {};

struct State {
//    uint64_t ref_func_call_counter = 0;
    // vertex colors
    uint8_t v_clrs[50] = {};
//    uint8_t vertex_colors_tmp[50] = {};
    // are edges used
    bool b_edges[300] = {};

    // current edge index
//    uint16_t c_edge = 0;
    uint16_t c_edge = -1;
    // number of used edges
    uint16_t used_edges = 0;
    // current max weight
    uint64_t weight = 0;

    void add_c_edge() {
        b_edges[c_edge] = true;
        weight += edges[c_edge].weight;
        used_edges++;
    }

    State get_next_state() {
        State new_state = *this;
//        std::copy(std::begin(a), std::end(a), std::begin(b));
//        new_state.c_edge++;
        return new_state;
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


void color_graph_rec(uint8_t vertex) {
    if (vertex_colors_tmp[vertex] == RED) {
        return;
    }

    vertex_colors_tmp[vertex] = RED;

    for (int i = 0; i < vertex_count; i++) {
        if (vertex_edges[vertex][i] > 0) {
            color_graph_rec(i);
        }
    }

}

bool check_consistency() {
    uint8_t start_id = 0;

    // reset tmp array for consistency check
    for (int i = 0; i < vertex_count; i++) {
        vertex_colors_tmp[i] = NO_COLOR;
    }

    // color
    color_graph_rec(start_id);

    // check if all colored
    for (int i = 0; i < vertex_count; i++) {
        if (vertex_colors_tmp[i] == NO_COLOR) {
            return false;
        }
    }
    return true;
}

//GraphState graph = GraphState();

//void rec_func(State && st) {
void rec_func(State & st) {
    st.c_edge++;
    ref_func_call_counter++;

    // check for
    // >> "Větev ukončíme, i pokud v daném mezistavu zjistíme, že výsledný podgraf nebude souvislý, neboť pro souvislý podgraf musí platit |E|>=|V|-1."
//    if ((used_edges + (edges_count - (current_edge-1))) < vertex_count-1) {
    if ((st.used_edges + (edges_count - (st.c_edge))) < vertex_count-1) {
        return;
    }


    if (st.c_edge == edges_count) {
        if (st.weight > current_max_weight_found) {
            // TODO check if graph is 'souvisly'
            if (check_consistency()) {
//                std::cout << "CONSISTENT" << std::endl;
                #pragma omp critical // update_best_weight
                {
                    // check if this is still the best weight (could have been changed when checking consistency)
                    if (st.weight > current_max_weight_found){
                        current_max_weight_found = st.weight;
                        memcpy( vertex_colors_best_solution, st.v_clrs, sizeof(vertex_colors_best_solution) );
                    }
                }
//            std::cout << "NEW MAX WEIGHT: " << current_max_weight_found << std::endl;
                return;  // return smth else?
            }
            else {
//                std::cout << "NOT CONSISTENT" << std::endl;
            }
        }
        return;
    }

//    Edge *c_edge = &(edges[current_edge]);

    if (current_max_weight_found > 0) {
        if ((st.weight + calc_sum_of_rest_of_weights(st.c_edge)) < current_max_weight_found) {
            return;
        }
    }

    // colors of both adjacent vertices
    uint8_t a_clr = st.a_clr();
    uint8_t b_clr = st.b_clr();

    // check vertexes - 1 already colored, both colored (same, different)
    if (a_clr == NO_COLOR  && b_clr == NO_COLOR) {
        // color RED - BLUE
        State new_st = st.get_next_state();
        new_st.do_clr_a(RED);
        new_st.do_clr_b(BLUE);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);

        // color BLUE - RED
        new_st = st.get_next_state();
        new_st.do_clr_a(BLUE);
        new_st.do_clr_b(RED);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);


//        // no color
//        new_st = st.get_next_state();
//        rec_func(new_st);

        new_st = st.get_next_state();
        new_st.do_clr_a(BLUE);
        new_st.do_clr_b(BLUE);
        #pragma omp task
        rec_func(new_st);

        new_st = st.get_next_state();
        new_st.do_clr_a(RED);
        new_st.do_clr_b(RED);
        #pragma omp task
        rec_func(new_st);
    }
    else if (a_clr == b_clr) {
        State new_st = st.get_next_state();
        rec_func(new_st);
    }
    else if ((a_clr == RED  && b_clr == BLUE) or
            (a_clr == BLUE  && b_clr == RED)) {

        State new_st = st.get_next_state();
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);

//        new_st = st.get_next_state();
//        rec_func(new_st);
    }
    else if (a_clr == NO_COLOR  && b_clr == RED) {

        State new_st = st.get_next_state();
        new_st.do_clr_a(BLUE);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);

        new_st = st.get_next_state();
        new_st.do_clr_a(RED);
        #pragma omp task
        rec_func(new_st);
    }
    else if (a_clr == NO_COLOR  && b_clr == BLUE) {

        State new_st = st.get_next_state();
        new_st.do_clr_a(RED);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);


        new_st = st.get_next_state();
        new_st.do_clr_a(BLUE);
        #pragma omp task
        rec_func(new_st);
    }
    else if (a_clr == RED  && b_clr == NO_COLOR) {
        State new_st = st.get_next_state();
        new_st.do_clr_b(BLUE);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);


        new_st = st.get_next_state();
        new_st.do_clr_b(RED);
        #pragma omp task
        rec_func(new_st);
    }
    else if (a_clr == BLUE  && b_clr == NO_COLOR) {
        State new_st = st.get_next_state();
        new_st.do_clr_b(RED);
        new_st.add_c_edge();
        #pragma omp task
        rec_func(new_st);

        new_st = st.get_next_state();
        new_st.do_clr_b(BLUE);
        #pragma omp task
        rec_func(new_st);
    }
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
//    std::cout << "core_count: " << core_count << ", Filepath: " << filepath << std::endl;

    // load graph
    std::ifstream infile(filepath);
    infile >> vertex_count;
//    std::cout << "vertex_count: " << vertex_count << std::endl;

    // first load all edges data into memory
    for (int i = 0; i < vertex_count; i++) {
        for (int j = 0; j < vertex_count; j++) {
            int weight;
            infile >> weight;
//            std::cout << "Weight: " << weight << std::endl;
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

    #pragma omp parallel
    {
        #pragma omp single
        rec_func(init_state);

    }

    std::cout << "{" << std::endl;
//    std::cout << "MAX WEIGHT: " << current_max_weight_found << " in REC calls " << ref_func_call_counter << std::endl;
    std::cout << "\"MAX_WEIGHT\": " << current_max_weight_found << ", " << std::endl;
    std::cout << "\"REC_CALLS\": " << ref_func_call_counter << ", " << std::endl;
//
    std::cout << "\"RESULT\": " << "[" << std::endl;
    for (int i = 0; i < vertex_count; i++) {
//        std::cout << "ID: " << i << " COLOR: " << int(vertex_colors_best_solution[i]) << std::endl;
        std::cout << "{"; // << std::endl;
        std::cout << "\"ID\": " << i << ", ";// << std::endl;
        std::cout << "\"COLOR\": " << int(vertex_colors_best_solution[i]); //<< ", ";// << std::endl;
//        std::cout << "}" << std::endl;
        if (i+1 == vertex_count) {
            std::cout << "}" << std::endl;
        }
        else {
            std::cout << "}, " << std::endl;
        }
    }
    std::cout << "] " << std::endl;

    std::cout << "}" << std::endl;
}

//  g++ -std=c++17 -Wall -pedantic -Wno-long-long -O2 -o main main.cpp && ./main graphs/graf_15_5.txt  1

// g++ -std=c++17  -Wall -pedantic -Wno-long-long -O2 -o ../main_seq ../main.cpp  && g++ -std=c++17 -fopenmp  -Wall -pedantic -Wno-long-long -O2 -o ../main_openmp ../main.cpp && python3 main.py