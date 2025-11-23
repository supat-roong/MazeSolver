cmake_minimum_required(VERSION 3.10)
project(MazeSolver)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(
    MazeSolver
    src/main.cpp
    src/MazeGenerator.cpp
    src/MazeSolver.cpp
)
