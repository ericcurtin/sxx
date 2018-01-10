project(sxx)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS "-std=c++03 -O2 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Wcast-qual -Woverloaded-virtual -Wconversion -Wsign-conversion -Wformat-security -Wwrite-strings -fdiagnostics-show-option")

add_executable(${PROJECT_NAME} src/sxx.cpp)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

