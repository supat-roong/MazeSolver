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

    // Parse width and height from command-line arguments
    if (argc >= 3) {
        width = std::atoi(argv[1]);
        height = std::atoi(argv[2]);
        if(width <= 0) width = 20;
        if(height <= 0) height = 20;
    }

    // Generate maze
    MazeGenerator mg(width, height);
    mg.generate();

    std::cout << "Generated Maze:\n";
    mg.print();

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
