cmake_minimum_required(VERSION 3.22.1)
project(Overeater)

add_executable(Overeater src/main.cpp src/Game.cpp)
target_link_libraries(Overeater sfml-window sfml-audio sfml-system sfml-graphics)