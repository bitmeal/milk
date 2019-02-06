cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 14)

# name project and provide version number
set(TEST_NAME installed)
project(${TEST_NAME})


######################
# DEPENDENCIES
######################
find_package(milk REQUIRED)
message("milk_INCLUDE_DIR=${milk_INCLUDE_DIR}")

######################
# OPTIONS / SETUP
######################

######################
# BUILD TEST TARGETS
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_DIR})
target_link_libraries(${TEST_NAME} milk)
######################

######################
# ADD TEST
enable_testing()
add_test(${TEST_NAME}_test ${TEST_NAME})
######################
