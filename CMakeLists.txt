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
  execute_process(COMMAND /bin/bash -c "rpm -E %{?dist} | sed -e 's/\\.//g' | sed 's/centos//g'" OUTPUT_VARIABLE PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(rpmbuild_path)

find_program(dpkg_path "dpkg" FALSE)
if (dpkg_path)
  message(STATUS "dpkg found, enabling DEB for the 'package' target")
  list(APPEND CPACK_GENERATOR DEB)
  execute_process(COMMAND /bin/bash -c "echo \"$(lsb_release -i | awk '{print tolower($NF)}')-$(lsb_release -c | awk '{print tolower($NF)}')\"" OUTPUT_VARIABLE PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(dpkg_path)

execute_process(COMMAND /bin/bash -c "uname -m" OUTPUT_VARIABLE CPU_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CPACK_PACKAGE_CONTACT "ericcurtin17@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "sxx is a tool that allows you to easier run shell COMMANDs on multiple hosts")
set(CPACK_PACKAGE_URL "https://github.com/ericcurtin/sxx")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${PLATFORM}-${CPU_ARCH}")

include(CPack)

