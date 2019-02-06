cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

project(milk)
######################


######################
# INTERFACE-LIBRARY TARGET
add_library(milk INTERFACE)
target_include_directories(milk INTERFACE $<BUILD_INTERFACE:"${CMAKE_CURRENT_LIST_DIR}/milk"> $<INSTALL_INTERFACE:include>)
######################


######################
# INSTALL TARGETS
install(TARGETS milk EXPORT milkTargets)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/milk.h" DESTINATION include)
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/milk/" DESTINATION include/milk FILES_MATCHING PATTERN "*.h")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/README.md" DESTINATION .)
install(FILES milk-config.cmake DESTINATION CMake)
install(EXPORT milkTargets DESTINATION CMake)
######################


######################
# ADD TEST
option(BUILD_TESTS "Build tests. Uncheck for install only runs" ON)

if(BUILD_TESTS)
	enable_testing()

	set(TEST_NAME milk_test)
	add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
	target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
	target_include_directories(${TEST_NAME} PRIVATE ./)
	add_test(${TEST_NAME} ${TEST_NAME})
endif()
######################
