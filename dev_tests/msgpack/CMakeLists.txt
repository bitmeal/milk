cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 14)

# name project and provide version number
set(TEST_NAME msgpack)
project(${TEST_NAME})


######################
# DEPENDENCIES
######################
# msgpack
find_package(msgpack REQUIRED)

######################
# OPTIONS / SETUP
######################
get_filename_component(TESTS_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PROJECT_DIR ${TESTS_DIR} DIRECTORY)

message(TESTS_DIR = ${TESTS_DIR})
message(PROJECT_DIR = ${PROJECT_DIR})

######################
# BUILD TEST TARGETS
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_DIR})
target_link_libraries(${TEST_NAME} msgpackc-static)
######################

######################
# ADD TEST
enable_testing()
add_test(${TEST_NAME}_test ${TEST_NAME})
######################
