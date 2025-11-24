#include "MazeGenerator.hpp"
#include "MazeSolver.hpp"
#include "HumanEyeSolver.hpp"
#include "DQN.hpp"
#include "ReplayBuffer.hpp"
#include <torch/torch.h>
#include <torch/serialize.h>
#include <random>
#include <iostream>

int main(){
    const int width=5, height=5;
    const double p=0.2;
    const int maxDepth=4;

    HumanEyeSolver agent(maxDepth);
    const int inputSize = 4*maxDepth*3+2;
    const int hiddenSize = 64;
    const int numActions = 4;

    DQN policyNet(inputSize, hiddenSize, numActions);
    torch::optim::Adam optimizer(policyNet->parameters(), torch::optim::AdamOptions(0.001));

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> uni(0,1.0f);

    float totalReward = 0.0f;              
    static float rewardAccumulator = 0.0f;  

    const int episodes=10000;
    const int maxSteps=width*height*2;
    float epsilon = 1.0f;       // start fully random
    const float epsilon_min = 0.05f;
    const float epsilon_decay = 0.995f;  // decay per episode
    float gamma=0.99f;

    const int batchSize = 64;
    const int bufferSize = 10000;
    const int bufferUpdateEp = 100;           
    ReplayBuffer replay(bufferSize);          

    for(int ep=0; ep<episodes; ep++){
        MazeGenerator mg(width,height,p); mg.generate(false);
        const auto& maze=mg.getMaze();
        int x=0,y=0;
        int step=0;
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));


        while(step<maxSteps && !(x==width-1 && y==height-1)){
            auto obs = agent.flattenObservation(x, y, maze);
            torch::Tensor state = torch::tensor(obs).unsqueeze(0);

            int action;
            std::vector<int> validActions;
            for(int a = 0; a < 4; a++) {
                int nx = x + MazeSolver::dx[a];
                int ny = y + MazeSolver::dy[a];

                // Check bounds and if wall allows movement
                if(nx >= 0 && nx < width && ny >= 0 && ny < height && (maze[y][x] & MazeSolver::dir[a]) != 0) {
                    validActions.push_back(a);
                }
            }
            
            if (uni(rng) < epsilon) {
                std::uniform_int_distribution<int> dist(0, validActions.size() - 1);
                action = validActions[dist(rng)];
            } else {
                // Greedy: select best among valid actions only
                auto qvals = policyNet->forward(state).squeeze(0); // flatten to 1D
                float maxQ = -1e9;
                int bestAction = -1;
                for (int a : validActions) {
                    if (qvals[a].item<float>() > maxQ) {
                        maxQ = qvals[a].item<float>();
                        bestAction = a;
                    }
                }
                action = bestAction;
            }

            int nx = x + MazeSolver::dx[action];
            int ny = y + MazeSolver::dy[action];

            float reward = -0.01f*step;  // step penalty
            bool done = false;

            // Distance to goal
            auto distBefore = std::abs(x - (width - 1)) + std::abs(y - (height - 1));

            // Wall hit
            if (nx < 0 || nx >= width || ny < 0 || ny >= height ||
                (maze[y][x] & MazeSolver::dir[action]) == 0) 
            {
                std::cout << "Hit wall!\n";
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                    nx = x;
                    ny = y;
                }
                reward = -10.0f;
                done = true;
            }
            // Goal reached
            else if (nx == width - 1 && ny == height - 1) {
                std::cout << "Reach goal!\n";
                reward = +10.0f;
                done = true;
            }
            // Valid move
            else {
                auto distAfter = std::abs(nx - (width - 1)) + std::abs(ny - (height - 1));
                // if (distAfter < distBefore)      reward += 0.05f;
                // else if (distAfter > distBefore) reward -= 0.02f;

                // visited penalty
                if (visited[ny][nx]) reward -= 0.05f;
            }

            // Mark position as visited
            visited[ny][nx] = true;

            // Next state
            auto nextObs = agent.flattenObservation(nx, ny, maze);
            totalReward += reward;
            // -------------------------------
            // Store in Replay Buffer
            // -------------------------------
            replay.add({obs, action, reward, nextObs, nx, ny});

            // If end episode, use terminal transition
            if (done) {
                // Break out of loop
                break;
            }

            // -------------------------------
            // Start learning only when buffer has enough samples
            // -------------------------------
            if (replay.ready(batchSize) && ep%bufferUpdateEp==0) {
                auto batch = replay.sample(batchSize);
            
                std::vector<float> statesFlat;
                std::vector<float> nextStatesFlat;
                std::vector<int64_t> actions;
                std::vector<float> rewards;
            
                // Store valid actions per next state
                std::vector<std::vector<int>> validNextActions(batchSize);
            
                for (size_t i = 0; i < batch.size(); i++) {
                    auto& t = batch[i];
                    statesFlat.insert(statesFlat.end(), t.state.begin(), t.state.end());
                    nextStatesFlat.insert(nextStatesFlat.end(), t.nextState.begin(), t.nextState.end());
                    actions.push_back(t.action);
                    rewards.push_back(t.reward);
            
                    // Compute valid actions for next state
                    int x = t.nextX; // make sure your transition struct stores next x,y
                    int y = t.nextY;
                    std::vector<int> valid;
                    for (int a = 0; a < 4; a++) {
                        int nx = x + MazeSolver::dx[a];
                        int ny = y + MazeSolver::dy[a];
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                            (maze[y][x] & MazeSolver::dir[a]) != 0) {
                            valid.push_back(a);
                        }
                    }
                    validNextActions[i] = valid;
                }
            
                auto statesTensor = torch::tensor(statesFlat).view({batchSize, -1});
                auto nextStatesTensor = torch::tensor(nextStatesFlat).view({batchSize, -1});
                auto actionsTensor = torch::tensor(actions, torch::kLong);
                auto rewardsTensor = torch::tensor(rewards);
            
                // Q(s,a)
                auto qvals = policyNet->forward(statesTensor);
                auto qvalsChosen = qvals.gather(1, actionsTensor.unsqueeze(1)).squeeze(1);
            
                // Q(s',a') with masking invalid moves
                auto nextQ = policyNet->forward(nextStatesTensor); // [batch, 4]
                for (size_t i = 0; i < batchSize; i++) {
                    for (int a = 0; a < 4; a++) {
                        if (std::find(validNextActions[i].begin(), validNextActions[i].end(), a) == validNextActions[i].end()) {
                            nextQ[i][a] = -1e9; // mask invalid action
                        }
                    }
                }
            
                torch::Tensor nextQ_values = std::get<0>(nextQ.max(1)).detach();
            
                auto targets = rewardsTensor + gamma * nextQ_values;
            
                optimizer.zero_grad();
                auto loss = torch::mse_loss(qvalsChosen, targets);
                loss.backward();
                optimizer.step();
            }

            x=nx; y=ny; step++;
        }

        rewardAccumulator += totalReward;

        if (ep % 50 == 0) {
            float avgReward = rewardAccumulator / 50.0f;
            std::cout << "Episode " << ep 
                    << " | episodeReward = " << totalReward
                    << " | avgReward(50ep) = " << avgReward
                    << "\n";
            rewardAccumulator = 0.0f; // reset every 50 episodes
        }
        totalReward = 0.0f; 
        epsilon = std::max(epsilon_min, epsilon * epsilon_decay);
    }

    std::cout<<"Training finished\n";

    // ----- Save model -----
    torch::serialize::OutputArchive out_archive;
    policyNet->save(out_archive);
    out_archive.save_to("dqn_maze_weights.pt");
    std::cout << "Saved trained weights\n";

    // ----- Load model for testing -----
    DQN testNet(inputSize, hiddenSize, numActions); // new instance
    torch::serialize::InputArchive in_archive;
    in_archive.load_from("dqn_maze_weights.pt");
    testNet->load(in_archive);
    std::cout << "Loaded trained weights\n";

    MazeGenerator mg(width, height, p);
    mg.generate();

    MazeSolver solver(mg);

    bool solved = solver.solveWithAgent(testNet, agent, 0.0f); // epsilon=0: deterministic

    if (solved) {
        std::cout << "Agent solved the maze!\n";
        solver.printPath(); // works perfectly
        std::cout << "Max visit order (DQN): " << solver.maxVisitOrder() << "\n";
        std::cout << "DQN Path length: " << solver.getPathLength() << "\n";
    } else {
        std::cout << "Agent could not solve the maze.\n";
        solver.printPath();
    }

    return 0;
}

