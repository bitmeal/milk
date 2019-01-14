cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

# name project and provide version number
set(TEST_NAME bson)
project(${TEST_NAME})


######################
# DEPENDENCIES
######################
# find bson library
find_package(libbsoncxx REQUIRED)
add_library(libbsoncxx INTERFACE IMPORTED)
target_include_directories(libbsoncxx INTERFACE ${LIBBSONCXX_INCLUDE_DIRS})

# bson needs boost
#find_package(Boost 1.56.0 REQUIRED)
find_package(Boost REQUIRED)
target_include_directories(libbsoncxx INTERFACE ${Boost_INCLUDE_DIRS})

######################
# OPTIONS / SETUP
######################
get_filename_component(TESTS_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PROJECT_DIR ${TESTS_DIR} DIRECTORY)

message(TESTS_DIR = ${TESTS_DIR})
message(PROJECT_DIR = ${PROJECT_DIR})

# if on windows, copy shared libraries to binary directory
IF (WIN32)
	get_filename_component(MONGOCXX_BIN_DIR "./../../bin/" ABSOLUTE BASE_DIR "${LIBMONGOCXX_LIBRARY_PATH}") # two parent dir changes (../) necessary even if it should be only one
	file(COPY "${MONGOCXX_BIN_DIR}/bsoncxx.dll" DESTINATION ${CMAKE_BINARY_DIR})
	get_filename_component(MONGOC_LIB_DIR "${MONGOC_LIBRARY}" DIRECTORY)
	get_filename_component(MONGOC_BIN_DIR "./../bin/" ABSOLUTE BASE_DIR "${MONGOC_LIB_DIR}")
	file(COPY "${MONGOC_BIN_DIR}/libbson-1.0.dll" DESTINATION ${CMAKE_BINARY_DIR})
ENDIF()

######################
# BUILD TEST TARGETS
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_DIR})
target_link_libraries(${TEST_NAME} libbsoncxx)
######################

######################
# ADD TEST
enable_testing()
add_test(${TEST_NAME}_test ${TEST_NAME})
######################

