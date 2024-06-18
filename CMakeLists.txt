cmake_minimum_required(VERSION 3.22.1)
project(Overeater)

if(MSVC)
    add_definitions(-DSFML_STATIC)
endif()

add_executable(Overeater src/main.cpp src/Game.cpp)

if(MSVC)
    target_link_libraries(Overeater sfml-system-s sfml-window-s sfml-graphics-s sfml-audio-s freetype winmm gdi32 ogg vorbis vorbisenc vorbisfile openal32 opengl32 flac)
else()
    target_link_libraries(Overeater sfml-window sfml-audio sfml-system sfml-graphics)
endif()