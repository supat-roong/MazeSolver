#include "ReplayBuffer.hpp"

ReplayBuffer::ReplayBuffer(size_t capacity)
    : capacity(capacity),
      index(0),
      rng(std::random_device{}())
{
}

void ReplayBuffer::add(const Transition& t) {
    if (buffer.size() < capacity)
        buffer.push_back(t);
    else
        buffer[index] = t;

    index = (index + 1) % capacity;
}

bool ReplayBuffer::ready(size_t batchSize) const {
    return buffer.size() >= batchSize;
}

std::vector<Transition> ReplayBuffer::sample(size_t batchSize) {
    std::vector<Transition> batch;
    std::uniform_int_distribution<size_t> dist(0, buffer.size() - 1);

    for (size_t i = 0; i < batchSize; i++)
        batch.push_back(buffer[dist(rng)]);

    return batch;
}
