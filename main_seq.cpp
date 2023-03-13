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
    bool is_used = false;

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
uint8_t vertex_colors[50] = {};
uint8_t vertex_colors_best_solution[50] = {};
uint8_t vertex_colors_tmp[50] = {};
int vertex_count;
int vertex_edges[50][50] = {};
uint16_t edges_count = 0;
Edge edges[300] = {};




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

void rec_func(uint16_t current_edge, uint64_t current_weight, uint16_t used_edges) {
    ref_func_call_counter++;

    // check for
    // >> "Větev ukončíme, i pokud v daném mezistavu zjistíme, že výsledný podgraf nebude souvislý, neboť pro souvislý podgraf musí platit |E|>=|V|-1."
//    if ((used_edges + (edges_count - (current_edge-1))) < vertex_count-1) {
    if ((used_edges + (edges_count - (current_edge))) < vertex_count-1) {
        return;
    }


    if (current_edge == edges_count) {
        if (current_weight > current_max_weight_found) {
            // TODO check if graph is 'souvisly'
            if (check_consistency()) {
//                std::cout << "CONSISTENT" << std::endl;
                current_max_weight_found = current_weight;
                memcpy( vertex_colors_best_solution, vertex_colors, sizeof(vertex_colors_best_solution) );
//            std::cout << "NEW MAX WEIGHT: " << current_max_weight_found << std::endl;
                return;  // return smth else?
            }
            else {
//                std::cout << "NOT CONSISTENT" << std::endl;
            }
        }
        return;
    }

    Edge *c_edge = &(edges[current_edge]);

    if (current_max_weight_found > 0) {
        if ((current_weight + calc_sum_of_rest_of_weights(current_edge)) < current_max_weight_found) {
            return;
        }
    }

    // check vertexes - 1 already colored, both colored (same, different)
    if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        // color RED - BLUE
        vertex_colors[c_edge->a_vertex] = RED;
        vertex_colors[c_edge->b_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);
        // color BLUE - RED
        vertex_colors[c_edge->a_vertex] = BLUE;
        vertex_colors[c_edge->b_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);
//        // no color
        vertex_colors[c_edge->a_vertex] = NO_COLOR;
        vertex_colors[c_edge->b_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
//        vertex_colors[c_edge->a_vertex] = BLUE;
//        vertex_colors[c_edge->b_vertex] = BLUE;
//        c_edge->is_used = false;
//        rec_func(current_edge + 1, current_weight, used_edges);
//
//        vertex_colors[c_edge->a_vertex] = RED;
//        vertex_colors[c_edge->b_vertex] = RED;
//        c_edge->is_used = false;
//        rec_func(current_edge + 1, current_weight, used_edges);
//
//        vertex_colors[c_edge->a_vertex] = NO_COLOR;
//        vertex_colors[c_edge->b_vertex] = NO_COLOR;
    }
    else if (vertex_colors[c_edge->a_vertex] == vertex_colors[c_edge->b_vertex]) {
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
    }
    else if ((vertex_colors[c_edge->a_vertex] == RED  && vertex_colors[c_edge->b_vertex] == BLUE) or
             (vertex_colors[c_edge->a_vertex] == BLUE  && vertex_colors[c_edge->b_vertex] == RED)) {
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);
//        c_edge->is_used = false;
//        rec_func(current_edge + 1, current_weight, used_edges);
    }
    else if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == RED) {
        vertex_colors[c_edge->a_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);

        vertex_colors[c_edge->a_vertex] = NO_COLOR;
//        vertex_colors[c_edge->a_vertex] = RED;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
//        vertex_colors[c_edge->a_vertex] = NO_COLOR;
    }
    else if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == BLUE) {
        vertex_colors[c_edge->a_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);

        vertex_colors[c_edge->a_vertex] = NO_COLOR;
//        vertex_colors[c_edge->a_vertex] = BLUE;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
//        vertex_colors[c_edge->a_vertex] = NO_COLOR;
    }
    else if (vertex_colors[c_edge->a_vertex] == RED  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        vertex_colors[c_edge->b_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);

        vertex_colors[c_edge->b_vertex] = NO_COLOR;
//        vertex_colors[c_edge->b_vertex] = RED;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
//        vertex_colors[c_edge->b_vertex] = NO_COLOR;
    }
    else if (vertex_colors[c_edge->a_vertex] == BLUE  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        vertex_colors[c_edge->b_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight, used_edges+1);

        vertex_colors[c_edge->b_vertex] = NO_COLOR;
//        vertex_colors[c_edge->b_vertex] = BLUE;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight, used_edges);
//        vertex_colors[c_edge->b_vertex] = NO_COLOR;
    }

    return;
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
    rec_func(0, 0, 0);

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
