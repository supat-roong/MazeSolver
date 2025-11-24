#ifndef REPLAYBUFFER_HPP
#define REPLAYBUFFER_HPP

#include <vector>
#include <random>

// A single transition in the replay memory
struct Transition {
    std::vector<float> state;
    int action;
    float reward;
    std::vector<float> nextState;
    int nextX; 
    int nextY;
};

class ReplayBuffer {
public:
    ReplayBuffer(size_t capacity);

    void add(const Transition& t);

    bool ready(size_t batchSize) const;

    std::vector<Transition> sample(size_t batchSize);

private:
    size_t capacity;
    size_t index;
    std::vector<Transition> buffer;
    std::mt19937 rng;
};

#endif
