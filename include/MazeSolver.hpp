#pragma once
#include "MazeGenerator.hpp"
#include <vector>

class MazeSolver {
public:
    MazeSolver(const MazeGenerator& mg);

    // Solve maze using DFS
    bool solveDFS();
    // Solve maze using BFS
    bool solveBFS();

    // Print maze with path overlay and visit numbers
    void printPath() const;

    // Get path length
    int getPathLength() const { return path.size(); }

    // Return the maximum visit number after DFS or BFS
    int maxVisitOrder() const;

private:
    const std::vector<std::vector<int>>& grid;
    int h, w;
    std::vector<std::pair<int,int>> path; // solution path
    std::vector<std::vector<int>> visitOrder; // visit order per cell
};
