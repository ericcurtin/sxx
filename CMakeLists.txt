project(sxx)
cmake_minimum_required(VERSION 2.8)

if (CMAKE_COMPILER_IS_GNUCXX) 
  set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Wcast-qual -Woverloaded-virtual -Wconversion -Wsign-conversion -Wformat-security -Wwrite-strings -fdiagnostics-show-option")
endif()

add_executable(${PROJECT_NAME} src/sxx.cpp)

target_link_libraries(${PROJECT_NAME} PocoFoundation)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

