#pragma once
#include <vector>

class MazeGenerator {
public:
    MazeGenerator(int width, int height, double monteCarloCarveProbability);
    void generate();
    void monteCarloCarve(double probability);
    void print() const;

    // Add this accessor inside the class
    const std::vector<std::vector<int>>& getMaze() const;

    static constexpr int N = 1;
    static constexpr int S = 2;
    static constexpr int E = 4;
    static constexpr int W = 8;

private:
    int w, h; // maze width and height
    double p; // Monte Carlo carving probability
    std::vector<std::vector<int>> grid;
};
