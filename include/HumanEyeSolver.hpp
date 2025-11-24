#pragma once
#include "MazeGenerator.hpp"
#include <vector>

struct RayStep { int forward, left, right; };

class HumanEyeSolver {
public:
    HumanEyeSolver(int depth) : maxDepth(depth) {}

    std::vector<RayStep> getRayObservation(int x, int y, int dir,
                                           const std::vector<std::vector<int>>& grid);
    std::vector<RayStep> getFullObservation(int x, int y,
                                            const std::vector<std::vector<int>>& grid);
    std::vector<float> flattenObservation(int x, int y,
                                          const std::vector<std::vector<int>>& grid);

private:
    int maxDepth;
};
