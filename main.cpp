#include <iostream>
#include <string>
using namespace std;

class GraphState {
    // edges 300
    // vertex 50
};

//int main() {
//    std::cout << "Hello, World!" << std::endl;
//    return 0;
//}

int main(int argc, char *argv[]) {
    int max_weight;
    std::string filepath;
    /* check input argument */
    if (argc != 3) {
        return 1;
    }
//    max_weight = int(argv[2])
    max_weight = stoi(argv[2]);
    filepath = std::string(argv[1]);
    std::cout << "Weight: " << max_weight << ", Filepath: " << filepath << std::endl;

}