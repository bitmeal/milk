cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

# name project and provide version number
set(TEST_NAME interface)
project(${TEST_NAME})


######################
# DEPENDENCIES
######################

######################
# OPTIONS / SETUP
######################
get_filename_component(TESTS_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_PROJECT_DIR ${TESTS_DIR} DIRECTORY)

message(TESTS_DIR = ${TESTS_DIR})
message(PARENT_PROJECT_DIR = ${PARENT_PROJECT_DIR})

######################
# BUILD TARGETS
######################

######################
# TEST
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
target_include_directories(${TEST_NAME} PRIVATE ${PARENT_PROJECT_DIR})
target_link_libraries(${TEST_NAME})
######################

