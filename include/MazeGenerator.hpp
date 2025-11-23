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

    static const int N;
    static const int S;
    static const int E;
    static const int W;

private:
    int w, h; // maze width and height
    double p; // Monte Carlo carving probability
    std::vector<std::vector<int>> grid;
};
