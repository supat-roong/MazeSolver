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

bool MazeSolver::solveAStar() {
    path.clear();
    visitOrder.assign(h, std::vector<int>(w, 0));

    // Heuristic: Manhattan distance
    auto heuristic = [](int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    };

    struct Node {
        int x, y;
        int g; // cost from start
        int f; // g + heuristic
        bool operator>(const Node &other) const { return f > other.f; }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<>> openSet;
    std::vector<std::vector<int>> gScore(h, std::vector<int>(w, INT_MAX));
    std::vector<std::vector<std::pair<int,int>>> parent(h, std::vector<std::pair<int,int>>(w, {-1,-1}));

    gScore[0][0] = 0;
    openSet.push({0, 0, 0, heuristic(0,0,w-1,h-1)});
    int count = 1;

    while(!openSet.empty()) {
        Node cur = openSet.top(); openSet.pop();
        int x = cur.x, y = cur.y;

        // Mark visit order
        if(visitOrder[y][x] == 0)
            visitOrder[y][x] = count++;

        if(x == w-1 && y == h-1) break; // reached goal

        for(int i=0;i<4;i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx<0 || nx>=w || ny<0 || ny>=h) continue;

            // Only move if there is a passage
            if((grid[y][x] & dir[i]) == 0 || (grid[ny][nx] & opp[i]) == 0) continue;

            int tentative_g = gScore[y][x] + 1;
            if(tentative_g < gScore[ny][nx]) {
                gScore[ny][nx] = tentative_g;
                parent[ny][nx] = {x,y};
                int f = tentative_g + heuristic(nx,ny,w-1,h-1);
                openSet.push({nx, ny, tentative_g, f});
            }
        }
    }

    // Reconstruct path
    int cx = w-1, cy = h-1;
    if(parent[cy][cx].first == -1 && parent[cy][cx].second == -1 && !(cx==0 && cy==0))
        return false; // no path found

    while(cx != -1 && cy != -1){
        path.push_back({cx,cy});
        auto p = parent[cy][cx];
        cx = p.first;
        cy = p.second;
    }
    std::reverse(path.begin(), path.end());
    return true;
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