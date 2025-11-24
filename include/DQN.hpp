#pragma once
#include <torch/torch.h>

struct DQNImpl : torch::nn::Module {
    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

    DQNImpl(int inputSize, int hiddenSize, int outputSize) {
        fc1 = register_module("fc1", torch::nn::Linear(inputSize, hiddenSize));
        fc2 = register_module("fc2", torch::nn::Linear(hiddenSize, hiddenSize));
        fc3 = register_module("fc3", torch::nn::Linear(hiddenSize, outputSize));
    }

    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(fc1->forward(x));
        x = torch::relu(fc2->forward(x));
        x = fc3->forward(x);
        return x;
    }
};
TORCH_MODULE(DQN);
