project(sxx)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Wcast-qual -Woverloaded-virtual -Wconversion -Wsign-conversion -Wformat-security -Wwrite-strings -fdiagnostics-show-option")

add_executable(${PROJECT_NAME} src/sxx.cpp)

target_link_libraries(${PROJECT_NAME} PocoFoundation)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

find_program(rpmbuild_path "rpmbuild" FALSE)
if(rpmbuild_path)
  message(STATUS "rpmbuild found, enabling RPM for the 'package' target")
  list(APPEND CPACK_GENERATOR RPM)
endif(rpmbuild_path)

find_program(dpkg_path "dpkg" FALSE)
if (dpkg_path)
  message(STATUS "dpkg found, enabling DEB for the 'package' target")
  list(APPEND CPACK_GENERATOR DEB)
endif(dpkg_path)

set(CPACK_PACKAGE_NAME "sxx")
set(CPACK_PACKAGE_CONTACT "ericcurtin17@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "sxx is a tool that allows you to easier run shell commands on multiple hosts")

include(CPack)

