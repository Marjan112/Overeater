cmake_minimum_required(VERSION 3.22.1)
project(Overeater)
add_executable(Overeater src/main.cpp src/Game.cpp src/msgbox.cpp)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})

    target_link_libraries(Overeater ${GTK3_LIBRARIES} -static-libstdc++ -lsfml-window -lsfml-audio -lsfml-system -lsfml-graphics)
endif()