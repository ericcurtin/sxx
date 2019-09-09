include_directories(../include)

set(BOOST_COMPONENTS unit_test_framework)

add_definitions(-DBOOST_TEST_DYN_LINK)

function(add_compute_test TEST_NAME TEST_SOURCE)
  get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_TARGET} ${TEST_SOURCE})
  target_link_libraries(${TEST_TARGET} boost_unit_test_framework)

  add_test(${TEST_NAME} ${TEST_TARGET})
endfunction()

add_compute_test("utils" utils.cpp)

