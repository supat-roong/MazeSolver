#include "HumanEyeSolver.hpp"

std::vector<RayStep> HumanEyeSolver::getRayObservation(int x, int y, int dir,
                                                      const std::vector<std::vector<int>>& grid)
{
    int h = grid.size();
    int w = grid[0].size();
    std::vector<RayStep> obs;

    const int dx[4] = {0,0,1,-1};
    const int dy[4] = {-1,1,0,0};
    const int dirFlags[4] = {MazeGenerator::N, MazeGenerator::S, MazeGenerator::E, MazeGenerator::W};
    const int leftFlags[4] = {MazeGenerator::W, MazeGenerator::E, MazeGenerator::N, MazeGenerator::S};
    const int rightFlags[4] = {MazeGenerator::E, MazeGenerator::W, MazeGenerator::S, MazeGenerator::N};

    int nx = x, ny = y;
    for(int step=0; step<maxDepth; step++) {
        RayStep r;
        if((grid[ny][nx] & dirFlags[dir])==0) { r.forward=1;r.left=1;r.right=1; obs.push_back(r); break; }
        r.forward=0;
        r.left=((grid[ny][nx]&leftFlags[dir])==0)?1:0;
        r.right=((grid[ny][nx]&rightFlags[dir])==0)?1:0;
        obs.push_back(r);
        nx+=dx[dir]; ny+=dy[dir];
        if(nx<0||nx>=w||ny<0||ny>=h) break;
    }
    while(obs.size()<maxDepth) obs.push_back({1,1,1});
    return obs;
}

std::vector<RayStep> HumanEyeSolver::getFullObservation(int x, int y,
                                                       const std::vector<std::vector<int>>& grid)
{
    std::vector<RayStep> obs;
    for(int d=0; d<4; d++){
        auto ray=getRayObservation(x,y,d,grid);
        obs.insert(obs.end(), ray.begin(), ray.end());
    }
    return obs;
}

std::vector<float> HumanEyeSolver::flattenObservation(int x, int y,
                                                     const std::vector<std::vector<int>>& grid)
{
    auto rays = getFullObservation(x,y,grid);
    std::vector<float> obs;
    // Add normalized agent position
    // int h = grid.size();
    // int w = grid[0].size();
    obs.push_back(static_cast<float>(x)); // x normalized to [0,1]
    obs.push_back(static_cast<float>(y)); // y normalized to [0,1]
    for(auto &r:rays){ obs.push_back(r.forward); obs.push_back(r.left); obs.push_back(r.right);}
    return obs;
}
