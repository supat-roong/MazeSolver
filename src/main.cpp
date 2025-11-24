#include "MazeGenerator.hpp"
#include "MazeSolver.hpp"
#include <iostream>
#include "MazeGenerator.hpp"
#include "MazeSolver.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int width = 20;
    int height = 20;
    double monteCarloCarveProbability = 0.1;
    int maxDepth=4;
    int inputSize = 4*maxDepth*3+2;
    int hiddenSize = 64;
    int numActions = 4;

    // Parse maze parameters from command-line arguments
    if (argc >= 2) {
        int w = std::atoi(argv[1]);
        if (w > 0) width = w;
    }
    
    if (argc >= 3) {
        int h = std::atoi(argv[2]);
        if (h > 0) height = h;
    }
    
    if (argc >= 4) {
        double p = std::atof(argv[3]);
        if (p >= 0.0 && p <= 1.0) monteCarloCarveProbability = p;
    }

    // ----- Load model for testing -----
    HumanEyeSolver agent(maxDepth);
    DQN testNet(inputSize, hiddenSize, numActions); // new instance
    torch::serialize::InputArchive in_archive;
    in_archive.load_from("dqn_maze_weights.pt");
    testNet->load(in_archive);
    std::cout << "Loaded trained weights\n";

    // Generate maze
    MazeGenerator mg(width, height, monteCarloCarveProbability);
    mg.generate();

    // Solve maze
    MazeSolver solver(mg);
    if (solver.solveDFS()) {
        std::cout << "\nDFS Maze Solution Path:\n";
        solver.printPath();  // prints the maze with path using '*'
        std::cout << "Max visit order (DFS): " << solver.maxVisitOrder() << "\n";
        std::cout << "DFS Path length: " << solver.getPathLength() << "\n";
    } else {
        std::cout << "No path found!\n";
    }

    if (solver.solveBFS()) {
        std::cout << "\nBFS Maze Solution Path:\n";
        solver.printPath();  // prints the maze with path using '*'
        std::cout << "Max visit order (BFS): " << solver.maxVisitOrder() << "\n";
        std::cout << "BFS Path length: " << solver.getPathLength() << "\n";
    } else {
        std::cout << "No path found!\n";
    }

    if (solver.solveAStar()) {
        std::cout << "\nA* Maze Solution Path:\n";
        solver.printPath();  // prints the maze with path using '*'
        std::cout << "Max visit order (A*): " << solver.maxVisitOrder() << "\n";
        std::cout << "A* Path length: " << solver.getPathLength() << "\n";
    } else {
        std::cout << "No path found!\n";
    }

    if (solver.solveWithAgent(testNet, agent, 0.0f)) {
        std::cout << "Agent solved the maze!\n";
    } else {
        std::cout << "Agent could not solve the maze.\n";
    }
    solver.printPath(); // works perfectly
    std::cout << "Max visit order (DQN): " << solver.maxVisitOrder() << "\n";
    std::cout << "DQN Path length: " << solver.getPathLength() << "\n";

    return 0;
}
