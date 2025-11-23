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

    return 0;
}
