cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# name project and provide version number
set(TEST_NAME milk)
project(${TEST_NAME})


######################
# DEPENDENCIES
######################

######################
# OPTIONS / SETUP
######################
get_filename_component(TESTS_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PROJECT_DIR ${TESTS_DIR} DIRECTORY)
get_filename_component(MILK_CPP_DIR "${PROJECT_DIR}/milk" ABSOLUTE)

message(TESTS_DIR = ${TESTS_DIR})
message(PROJECT_DIR = ${PROJECT_DIR})
message(MILK_CPP_DIR = ${MILK_CPP_DIR})
######################
# BUILD TEST TARGETS
add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_DIR})
target_link_libraries(${TEST_NAME})
target_sources(${TEST_NAME} PUBLIC
${MILK_CPP_DIR}/bite.cpp	${MILK_CPP_DIR}/grain.cpp	${MILK_CPP_DIR}/bite_member_proxy.cpp	${MILK_CPP_DIR}/bite_iterator.cpp
#${MILK_CPP_DIR}/bite.h		${MILK_CPP_DIR}/grain.h		${MILK_CPP_DIR}/bite_member_proxy.h		${MILK_CPP_DIR}/bite_iterator.h
)
######################

######################
# ADD TEST
enable_testing()
add_test(${TEST_NAME}_test ${TEST_NAME})
######################
