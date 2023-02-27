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
//    uint8_t color;
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
uint8_t vertex_colors[50];
int vertex_count;
//uint8_t all_vertex[50][50];
int vertex_edges[50][50];
uint16_t edges_count = 0;
Edge edges[300];






//class GraphState {
//    // edges 300
//    bool uses_edges[300];
//    // vertex 50
//    uint8_t vertex_colors[50];
////    uint8_t last_picked_edge = 0;
//
//public:
////    uint8_t pick_unassigned_edge() {
////        return uses_edges[++last_picked_edge];
////    }
//
//};

uint64_t calc_sum_of_rest_of_weights(uint16_t current_edge) {
    uint64_t sum = 0;
    for (int i = current_edge; i < edges_count; i++) {
        sum += edges[i].weight;
    }
    return sum;
}
//int main() {
//    std::cout << "Hello, World!" << std::endl;
//    return 0;
//}


//GraphState graph = GraphState();

//
uint64_t rec_func(uint16_t current_edge, uint64_t current_weight) {
    ref_func_call_counter++;
    // check

    // TODO somehow check for
    // >> "Větev ukončíme, i pokud v daném mezistavu zjistíme, že výsledný podgraf nebude souvislý, neboť pro souvislý podgraf musí platit |E|>=|V|-1."


    if (current_edge == edges_count) {
        if (current_weight > current_max_weight_found) {
            // TODO check if graph is 'souvisly'
            current_max_weight_found = current_weight;
            // TODO store current config
//            std::cout << "NEW MAX WEIGHT: " << current_max_weight_found << std::endl;
            return current_weight;  // return smth else?
        }
        return 0;
    }

    Edge *c_edge = &(edges[current_edge]);

    if (current_max_weight_found > 0) {
        if ((current_weight + calc_sum_of_rest_of_weights(current_edge)) < current_max_weight_found) {
            return 0;
        }
    }

    // check vertexes - 1 already colored, both colored (same, different)
    if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        // color RED - BLUE
        vertex_colors[c_edge->a_vertex] = RED;
        vertex_colors[c_edge->b_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);
        // color BLUE - RED
        vertex_colors[c_edge->a_vertex] = BLUE;
        vertex_colors[c_edge->b_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);
        // no color
        vertex_colors[c_edge->a_vertex] = NO_COLOR;
        vertex_colors[c_edge->b_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if (vertex_colors[c_edge->a_vertex] == vertex_colors[c_edge->b_vertex]) {
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if ((vertex_colors[c_edge->a_vertex] == RED  && vertex_colors[c_edge->b_vertex] == BLUE) or
            (vertex_colors[c_edge->a_vertex] == BLUE  && vertex_colors[c_edge->b_vertex] == RED)) {
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == RED) {
        vertex_colors[c_edge->a_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);

        vertex_colors[c_edge->a_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if (vertex_colors[c_edge->a_vertex] == NO_COLOR  && vertex_colors[c_edge->b_vertex] == BLUE) {
        vertex_colors[c_edge->a_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);

        vertex_colors[c_edge->a_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if (vertex_colors[c_edge->a_vertex] == RED  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        vertex_colors[c_edge->b_vertex] = BLUE;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);

        vertex_colors[c_edge->b_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }
    else if (vertex_colors[c_edge->a_vertex] == BLUE  && vertex_colors[c_edge->b_vertex] == NO_COLOR) {
        vertex_colors[c_edge->b_vertex] = RED;
        c_edge->is_used = true;
        rec_func(current_edge + 1, current_weight + edges[current_edge].weight);

        vertex_colors[c_edge->b_vertex] = NO_COLOR;
        c_edge->is_used = false;
        rec_func(current_edge + 1, current_weight);
    }

    return 0; // TODO change to max found weight? or just return None
}


int main(int argc, char *argv[]) {
    int core_count;
    std::string filepath;
    /* check input argument */
    if (argc != 3) {
        return 1;
    }
//    max_weight = int(argv[2])
    core_count = stoi(argv[2]);
    filepath = std::string(argv[1]);
    std::cout << "core_count: " << core_count << ", Filepath: " << filepath << std::endl;

    // TODO load graph
    std::ifstream infile(filepath);
//    int vertex_count;
    infile >> vertex_count;
//    std::cout << "vertex_count: " << vertex_count << std::endl;

    // first load all edges data into memory
    for (int i = 0; i < vertex_count; i++) {
//        for (int j = 0; j <= i; j++) {
        for (int j = 0; j < vertex_count; j++) {
            int weight;
            infile >> weight;
//            std::cout << "Weight: " << weight << std::endl;
//            if (weight > 0) {
            vertex_edges[i][j] = weight;
//            if (weight >= 80 && weight <= 120) {
////                edges_count++;
////                edges[edges_count].a_vertex = i;
////                edges[edges_count].b_vertex = j;
//                vertex_edges[i][j] = weight;
//                std::cout << "orig: " << i << ", dest: " << j << std::endl;
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

    // TODO OPTIONAL sort edges by weights

    // TODO other stuff

    // TODO check if bipartite first

    // TODO other stuff

    rec_func(0, 0);

    std::cout << "MAX WEIGHT: " << current_max_weight_found << " in REC calls " << ref_func_call_counter << std::endl;
//    std::cout << current_max_weight_found << std::endl;
}