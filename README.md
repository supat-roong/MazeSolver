# MazeSolver

**MazeSolver** is a C++ project for generating and solving mazes using classical algorithms as well as machine learning (DQN). It provides a flexible framework to experiment with maze generation, solving strategies, and reinforcement learning for pathfinding.  

---

## Features

### Maze Generator
- Generates **perfect mazes** (no loops, single solution) using **DFS backtracking**.
- Optionally **removes walls** using **Monte Carlo-based carving** to create multiple paths and increase maze complexity.

### Maze Solver
Supports multiple solving algorithms with different trade-offs:

- **DFS (Depth-First Search)**  
  - Implemented recursively.  
  - Explores paths deeply before backtracking.  
  - Simple and fast in small mazes, but **does not guarantee the shortest path** and may visit fewer cells but get stuck in dead-ends.

- **BFS (Breadth-First Search)**  
  - Uses a queue to explore all neighboring cells level by level.  
  - **Always finds the shortest path** if one exists.  
  - Tends to **visit many cells**, so it can be slower in large mazes.

- **A\*** (with Manhattan distance heuristic)  
  - Uses a **priority queue** with cost = distance traveled + heuristic.  
  - **Finds the shortest path** while visiting fewer cells than BFS by prioritizing paths likely to reach the goal.  
  - Efficient in mazes with open spaces and multiple branches.

- **ML-based solver (DQN)** using **Deep Q-Network**  
  - Learns an approximate policy from experience.  
  - Can sometimes **find the optimal path with fewer visits**, especially in mazes similar to training.  
  - Performance depends heavily on training: may **fail to find a solution** in unseen or complex mazes.  

### DQN Solver
- Observations include **ray-based vision** up to a configurable depth and  agent position.
- Rewards based on reaching goal, hitting walls, and visited cells.
- Supports **epsilon-greedy exploration**, replay buffer, and training loop with PyTorch C++ API ([libtorch](https://docs.pytorch.org/cppdocs/installing.html)).  

---

## Installation

1. Create a build directory and enter it:

```bash
mkdir build
cd build
```
2. Run CMake to configure the project:

```bash
cmake ..
```

3. Build the project:

```bash
make
```

After building, you will see **two executables:**

## Executables
### 🧩 MazeSolver

Generate a maze and solve it using DFS, BFS, A*, DQN (from pretrained model)

This executable is used to compare algorithms, print solutions, and visualize paths.

### 🤖 TrainDQN

Train the Deep Q-Network agent using:

- Randomly generated mazes

- Valid-action masking

- Replay buffer

- Epsilon-greedy exploration

- Reward shaping

This executable produces a trained model file `dqn_maze_weights.pt` which is then used by MazeSolver for ML-based solving.


## Dependencies

- [libtorch](https://docs.pytorch.org/cppdocs/installing.html) (C++ PyTorch library)

- C++17 compatible compiler

- CMake ≥ 3.12

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
