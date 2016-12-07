message(STATUS "Adding tests")
file(GLOB TESTS "**/*.c")

foreach(TEST ${TESTS})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    message(STATUS "Adding test ${TEST_NAME}")
    add_test(${TEST_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh" ${TEST} "${CMAKE_BINARY_DIR}/skel")
endforeach()
