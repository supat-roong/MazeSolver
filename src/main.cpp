#include "MazeGenerator.hpp"
#include "MazeSolver.hpp"
#include <iostream>
#include "MazeGenerator.hpp"
#include "MazeSolver.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // ---------------------
    // --- JSON CONFIG -----
    // ---------------------
    std::ifstream f("../config/config.json");
    if (!f.is_open()) {
        std::cerr << "Cannot open config.json";
        return 1;
    }
    json cfg = json::parse(f);

    const int width = cfg["maze"]["width"];
    const int height = cfg["maze"]["height"];
    const double monteCarloCarveProbability = cfg["maze"]["monteCarloCarveProbability"];

    const int maxDepth = cfg["agent"]["maxDepth"];
    const int numActions = cfg["agent"]["numActions"];
    const int hiddenSize = cfg["agent"]["hiddenSize"];
    const std::string ckptPath = cfg["agent"]["ckptPath"];

    const int inputSize = 4*maxDepth*3+2;


    // ----- Load model for testing -----
    HumanEyeSolver agent(maxDepth);
    DQN testNet(inputSize, hiddenSize, numActions); // new instance
    torch::serialize::InputArchive in_archive;
    in_archive.load_from(ckptPath);
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
