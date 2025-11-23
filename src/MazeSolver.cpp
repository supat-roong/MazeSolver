#include "MazeSolver.hpp"
#include <iostream>
#include <queue>
#include <stack>
#include <iomanip>
#include <algorithm>

MazeSolver::MazeSolver(const MazeGenerator& mg)
    : grid(mg.getMaze()), h(mg.getMaze().size()), w(mg.getMaze()[0].size())
{
    visitOrder.assign(h, std::vector<int>(w, 0));
}

// ----------------------------------
// DFS Solver
// ----------------------------------
bool MazeSolver::solveDFS() {
    visitOrder.assign(h, std::vector<int>(w, 0));
    path.clear();

    std::vector<std::vector<int>> counter(h, std::vector<int>(w, 0)); // for counting visit order
    int count = 1;

    std::vector<std::vector<std::pair<int,int>>> parent(h, std::vector<std::pair<int,int>>(w, {-1,-1}));

    const int dx[4] = {0,0,1,-1};
    const int dy[4] = {-1,1,0,0};
    const int dir[4] = {MazeGenerator::N, MazeGenerator::S, MazeGenerator::E, MazeGenerator::W};
    const int opp[4] = {MazeGenerator::S, MazeGenerator::N, MazeGenerator::W, MazeGenerator::E};

    std::function<bool(int,int)> dfs = [&](int x, int y) -> bool {
        visitOrder[y][x] = count++;
        if (x == w-1 && y == h-1)
            return true; // reached goal

        for(int i=0;i<4;i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx<0||ny<0||nx>=w||ny>=h) continue;
            if(visitOrder[ny][nx] > 0) continue; // already visited

            if((grid[y][x] & dir[i])!=0 && (grid[ny][nx] & opp[i])!=0) {
                parent[ny][nx] = {x,y};
                if(dfs(nx,ny)) return true;
            }
        }
        return false;
    };

    bool found = dfs(0,0);

    if(found) {
        // reconstruct path
        int x = w-1, y = h-1;
        while(x!=-1 && y!=-1) {
            path.push_back({x,y});
            auto p = parent[y][x];
            x = p.first;
            y = p.second;
        }
        std::reverse(path.begin(), path.end());
    }

    return found;
}

// ----------------------------------
// BFS Solver
// ----------------------------------
bool MazeSolver::solveBFS() {
    std::vector<std::vector<bool>> visited(h, std::vector<bool>(w,false));
    std::vector<std::vector<std::pair<int,int>>> parent(h,std::vector<std::pair<int,int>>(w,{-1,-1}));
    std::queue<std::pair<int,int>> q;
    q.push({0,0});
    visited[0][0] = true;

    int count = 1;
    visitOrder.assign(h,std::vector<int>(w,0));
    visitOrder[0][0] = count++;

    const int dx[4] = {0,0,1,-1};
    const int dy[4] = {-1,1,0,0};
    const int dir[4] = {MazeGenerator::N, MazeGenerator::S, MazeGenerator::E, MazeGenerator::W};
    const int opp[4] = {MazeGenerator::S, MazeGenerator::N, MazeGenerator::W, MazeGenerator::E};

    while(!q.empty()) {
        auto [x,y] = q.front(); q.pop();

        if(x==w-1 && y==h-1) {
            // reconstruct path
            path.clear();
            while(x!=-1 && y!=-1) {
                path.push_back({x,y});
                auto p = parent[y][x];
                x = p.first;
                y = p.second;
            }
            std::reverse(path.begin(), path.end());
            return true;
        }

        for(int i=0;i<4;i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx<0||ny<0||nx>=w||ny>=h) continue;
            if(visited[ny][nx]) continue;
            if((grid[y][x] & dir[i])!=0 && (grid[ny][nx] & opp[i])!=0) {
                visited[ny][nx] = true;
                visitOrder[ny][nx] = count++;
                parent[ny][nx] = {x,y};
                q.push({nx,ny});
            }
        }
    }
    return false;
}


void MazeSolver::printPath() const {
    // Lookup for path cells
    std::vector<std::vector<bool>> onPath(h, std::vector<bool>(w, false));
    for (auto& p : path) onPath[p.second][p.first] = true;

    // Top border
    std::cout << "┌";
    for (int i = 0; i < w-1; ++i) std::cout << "───┬";
    std::cout << "───┐\n";

    for (int y = 0; y < h; ++y) {
        // Left wall
        std::cout << "│";
        for (int x=0;x<w;x++) {
            if(onPath[y][x])
                std::cout << " ● ";
            else if(visitOrder[y][x]>0)
                std::cout << " . ";
            else
                std::cout << "   ";

            if ((grid[y][x] & MazeGenerator::E)!=0) std::cout << " ";
            else std::cout << "│";
        }
        std::cout << "\n";

        if (y==h-1) break;

        // Bottom walls
        std::cout << "├";
        for (int x = 0; x < w; ++x) {
            if ((grid[y][x] & MazeGenerator::S) != 0) std::cout << "   ";
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

int MazeSolver::maxVisitOrder() const {
    int maxOrder = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (visitOrder[y][x] > maxOrder)
                maxOrder = visitOrder[y][x];
    return maxOrder;
}