project(sxx)
cmake_minimum_required(VERSION 2.8)

include_directories(include)

set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align -Wunused -Wcast-qual -Woverloaded-virtual -Wconversion -Wformat-security -Wwrite-strings -fdiagnostics-show-option")

set(COVERAGE OFF CACHE BOOL "Coverage")

if (COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

add_executable(${PROJECT_NAME} src/sxx.cpp)

target_link_libraries(${PROJECT_NAME} PocoFoundation)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

find_program(rpmbuild_path "rpmbuild" FALSE)
if (rpmbuild_path) # is rpm based
  message(STATUS "rpmbuild found, enabling RPM for the 'package' target")
  list(APPEND CPACK_GENERATOR RPM)
  if (EXISTS "/etc/redhat-release") # is rhel or centos
    execute_process(COMMAND /bin/bash -c "rpm -E %{?dist} | sed -e 's/\\.//g' | sed 's/centos//g'"
      OUTPUT_VARIABLE DISTRO_AND_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CPACK_RPM_PACKAGE_REQUIRES "poco-foundation")
  else () # is opensuse
    execute_process(COMMAND /bin/bash -c ". /etc/os-release; printf \"%s%.0f\" \"$ID\" \"$VERSION_ID\""
      OUTPUT_VARIABLE DISTRO_AND_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CPACK_RPM_PACKAGE_REQUIRES "libPocoFoundation60")
  endif ()
endif ()

find_program(dpkg_path "dpkg" FALSE)
if (dpkg_path) # is dpkg based
  message(STATUS "dpkg found, enabling DEB for the 'package' target")
  list(APPEND CPACK_GENERATOR DEB)
  execute_process(COMMAND /bin/bash -c "printf \"$(lsb_release -i | awk '{printf tolower($NF)}')\""
    OUTPUT_VARIABLE DISTRO)

  if (${DISTRO} STREQUAL "debian") # is debian
    execute_process(COMMAND /bin/bash -c "printf \"%.f\" \"$(lsb_release -r | awk '{printf int($NF)}')\""
      OUTPUT_VARIABLE DISTRO_VERSION)
    if (${DISTRO_VERSION} EQUAL 8)
      set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpocofoundation9")
    else ()
      set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpocofoundation46")
    endif ()
  else () # is ubuntu
    execute_process(COMMAND /bin/bash -c "printf \"$(lsb_release -r | awk '{printf $NF}')\""
      OUTPUT_VARIABLE DISTRO_VERSION)
    if (${DISTRO_VERSION} EQUAL 14.04) 
      set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpocofoundation9")
    else ()
      set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpocofoundation9v5")
    endif ()
  endif ()

  set(DISTRO_AND_VERSION "${DISTRO}${DISTRO_VERSION}")
endif ()

execute_process(COMMAND /bin/bash -c "uname -m" OUTPUT_VARIABLE CPU_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CPACK_PACKAGE_CONTACT "ericcurtin17@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "sxx is a tool that allows you to easily run shell commands on multiple hosts or transfer files to multiple hosts")
set(CPACK_PACKAGE_URL "https://github.com/ericcurtin/sxx")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_PACKAGE_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${DISTRO_AND_VERSION}-${CPU_ARCH}")

include(CPack)

ADD_CUSTOM_TARGET(doc)

ADD_CUSTOM_COMMAND(TARGET doc SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/doc/sxx.pod
  COMMAND podchecker --nowarnings ${CMAKE_CURRENT_SOURCE_DIR}/doc/sxx.pod
  COMMAND pod2man -c 'sxx manual' -q none -r 'sxx ${CPACK_PACKAGE_VERSION}' ${CMAKE_CURRENT_SOURCE_DIR}/doc/sxx.pod ${CMAKE_CURRENT_BINARY_DIR}/sxx.1
  COMMAND gzip -f ${CMAKE_CURRENT_BINARY_DIR}/sxx.1
  OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/sxx.1.gz)

ADD_CUSTOM_COMMAND(TARGET doc SOURCE doc
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sxx.1.gz)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/sxx.1.gz
  DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)

enable_testing()
add_subdirectory(test)

