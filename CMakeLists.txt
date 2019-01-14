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
# ADD TESTS
option(BUILD_TESTS "Build tests. Uncheck for install only runs" OFF)

if(BUILD_TESTS)
	# include tests CMakeLists.txt
endif()
######################
