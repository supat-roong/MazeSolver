cmake_minimum_required(VERSION 3.15)
project(MazeSolver)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- LibTorch ----
# Change this to your LibTorch path
set(Torch_DIR "/path/to/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)

# ---- Optional: Eigen ----
# find_package(Eigen3 REQUIRED)
# include_directories(${EIGEN3_INCLUDE_DIR})

# ---- Include nlohmann_json from homebrew ----
include_directories("/opt/homebrew/include")
find_package(nlohmann_json CONFIG REQUIRED)


# ---- Sources ----
set(SOURCES
    src/main.cpp
    src/MazeGenerator.cpp
    src/MazeSolver.cpp
    src/HumanEyeSolver.cpp
    src/trainDQN.cpp
)

add_executable(MazeSolver src/main.cpp src/MazeGenerator.cpp src/MazeSolver.cpp src/HumanEyeSolver.cpp src/DQN.cpp)
add_executable(TrainDQN src/trainDQN.cpp src/MazeGenerator.cpp src/MazeSolver.cpp src/HumanEyeSolver.cpp src/DQN.cpp src/ReplayBuffer.cpp)

target_include_directories(MazeSolver PRIVATE include nlohmann_json::nlohmann_json)
target_include_directories(TrainDQN PRIVATE include nlohmann_json::nlohmann_json)

target_link_libraries(MazeSolver "${TORCH_LIBRARIES}")
target_link_libraries(TrainDQN "${TORCH_LIBRARIES}")


# ---- Needed flags for LibTorch ----
set_property(TARGET MazeSolver PROPERTY CXX_STANDARD 17)
set_property(TARGET MazeSolver PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET MazeSolver PROPERTY CXX_EXTENSIONS OFF)

# ---- On Mac / Linux ----
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# ---- Print CMake info ----
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")