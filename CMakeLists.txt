cmake_minimum_required(VERSION 3.22.1)
project(Overeater)

set(CXX_STANDARD_REQUIRED 23)

add_executable(Overeater src/main.cpp)

target_link_libraries(Overeater -lsfml-window -lsfml-audio -lsfml-system -lsfml-graphics)