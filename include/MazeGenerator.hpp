#pragma once
#include <vector>

class MazeGenerator {
public:
    MazeGenerator(int width, int height);
    void generate();
    void print() const;

    // Add this accessor inside the class
    const std::vector<std::vector<int>>& getMaze() const;

    static const int N;
    static const int S;
    static const int E;
    static const int W;

private:
    int w, h;
    std::vector<std::vector<int>> grid;
};
