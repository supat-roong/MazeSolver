#include "MazeGenerator.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

// Define static direction constants
const int MazeGenerator::N = 1;
const int MazeGenerator::S = 2;
const int MazeGenerator::E = 4;
const int MazeGenerator::W = 8;

MazeGenerator::MazeGenerator(int width, int height, double monteCarloCarveProbability) : w(width), h(height), p(monteCarloCarveProbability){
    grid.assign(h, std::vector<int>(w, 0));
}

void MazeGenerator::generate() {
    std::mt19937 rng(std::random_device{}());

    // Direction helpers: N,S,E,W
    std::vector<std::pair<int,int>> directions = {{0,-1},{0,1},{1,0},{-1,0}};
    std::vector<int> dir_flags = {N,S,E,W};
    std::vector<int> opposite = {S,N,W,E};

    std::function<void(int,int)> dfs = [&](int x, int y) {
        // Randomize directions
        std::vector<int> idx = {0,1,2,3};
        std::shuffle(idx.begin(), idx.end(), rng);

        for (int i : idx) {
            int nx = x + directions[i].first;
            int ny = y + directions[i].second;

            // Bounds check
            if (ny < 0 || ny >= h || nx < 0 || nx >= w) continue;

            if (grid[ny][nx] != 0) continue; // already carved

            // Carve passage
            grid[y][x] |= dir_flags[i];
            grid[ny][nx] |= opposite[i];

            dfs(nx, ny);
        }
    };

    // Initialize all cells to 0
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            grid[y][x] = 0;

    // Start DFS from top-left
    dfs(0, 0);
    std::cout << "Generated Perfect Maze with DFS Backtracking:\n";
    print();

    // Monte Carlo carving
    monteCarloCarve(p);

    std::cout << "Generated Maze after Monte Carlo Carving:\n";
    print();
}

void MazeGenerator::monteCarloCarve(double probability) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Possible directions: N,S,E,W
    std::vector<std::pair<int,int>> directions = {{0,-1},{0,1},{1,0},{-1,0}};
    std::vector<int> dir_flags = {N, S, E, W};
    std::vector<int> opposite  = {S, N, W, E};

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            
            if (dist(rng) > probability)
                continue; // skip this cell

            // --- Find all valid neighbors for potential carving ---
            std::vector<int> candidates;
            for (int i = 0; i < 4; ++i) {
                int nx = x + directions[i].first;
                int ny = y + directions[i].second;

                if (nx < 0 || nx >= w || ny < 0 || ny >= h)
                    continue; // out of bounds
                if (grid[y][x] & dir_flags[i])
                    continue; // already connected

                candidates.push_back(i); // valid direction
            }

            if (candidates.empty())
                continue; // no valid walls to carve

            // Pick a random valid wall
            int i = candidates[rng() % candidates.size()];
            int nx = x + directions[i].first;
            int ny = y + directions[i].second;

            // Carve the passage (creates a loop)
            grid[y][x]   |= dir_flags[i];
            grid[ny][nx] |= opposite[i];
        }
    }
}


void MazeGenerator::print() const {
    // Top border
    std::cout << "┌";
    for (int i = 0; i < w-1; ++i) std::cout << "───┬";
    std::cout << "───┐\n";

    for (int y = 0; y < h; ++y) {
        // Left wall
        std::cout << "│";
        for (int x = 0; x < w; ++x) {
            // Cell space
            std::cout << "   ";

            // East wall
            if ((grid[y][x] & E) != 0) std::cout << " ";
            else std::cout << "│";
        }
        std::cout << "\n";

        if (y==h-1) break;

        // Bottom walls
        std::cout << "├";
        for (int x = 0; x < w; ++x) {
            if ((grid[y][x] & S) != 0) std::cout << "   ";
            else std::cout << "───";

            if (x < w-1) std::cout << "┼";
        }
        std::cout << "┤\n";
    }

    // Bottom border
    std::cout << "└";
    for (int i = 0; i < w-1; ++i) std::cout << "───┴";
    std::cout << "───┘\n";
}

// Accessor to return the grid
const std::vector<std::vector<int>>& MazeGenerator::getMaze() const {
    return grid;
}
