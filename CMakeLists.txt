cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 14)

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
option(ENABLE_COVERAGE "Check code coverage of tests" ON)
if(BUILD_TESTS)
	enable_testing()

	set(TEST_NAME milk_test)
	add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
	target_compile_definitions(${TEST_NAME} PRIVATE TEST_NAME="${TEST_NAME}")
	target_include_directories(${TEST_NAME} PRIVATE ./)
	add_test(${TEST_NAME} ${TEST_NAME})
endif()
######################


######################
# external tools integration
option(FETCH_SUBMODULES "Fetch external submodules during build" OFF)

######################
# Catch2
if(BUILD_TESTS)
	find_package(Catch2 QUIET)

	# add submodule directory to project if no system version is found
	if(NOT Catch2_FOUND)
		message(STATUS "could not find system Catch2; adding submodule as subdirectory")
		add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/Catch2")
	endif()

	# submodule does not seem to be cloned
	if(NOT TARGET Catch2::Catch2)
		message(STATUS "could not detect Catch2 in submodule directory; cloning submodules when cheched")
		find_package(Git)

		# download submodule Catch2
		if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git")
		# Update submodules as needed
			if(FETCH_SUBMODULES)
				message(STATUS "Submodule update")
				execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
								WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
								RESULT_VARIABLE GIT_SUBMOD_RESULT)
				if(NOT GIT_SUBMOD_RESULT EQUAL "0")
					message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
				endif()
			else()
				message(FATAL_ERROR "enable FETCH_SUBMODULES to automatically fetch a Version of Catch2, or provide a system version of Catch2")
			endif()
		endif()
	endif()

	if(NOT TARGET Catch2::Catch2)
		message(FATAL_ERROR "something went wrong and we could not find or automatically fetch Catch2")
	endif()


	# add extra Catch2 modules
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/Catch2/contrib" ${CMAKE_MODULE_PATH})
endif()
######################

######################
# gcov
if(ENABLE_COVERAGE AND BUILD_TESTS)
find_package(codecov QUIET)

# add submodule to CMAKE_MODULE_PATH if no system version is found
if(NOT codecov_FOUND)
	message(STATUS "could not find system codecov; adding submodules cmake directory to CMAKE_MODULE_PATH")
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/CMake-codecov/cmake" ${CMAKE_MODULE_PATH})
endif()

find_package(codecov)
# if still not found, clone submodules
if(NOT codecov_FOUND)
	message(STATUS "could not detect codecov in submodule directory; cloning submodules when cheched")
	find_package(Git)

	# download submodule Catch2
	if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git")
	# Update submodules as needed
		if(FETCH_SUBMODULES)
			message(STATUS "Submodule update")
			execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
							WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
							RESULT_VARIABLE GIT_SUBMOD_RESULT)
			if(NOT GIT_SUBMOD_RESULT EQUAL "0")
				message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
			endif()
		else()
			message(FATAL_ERROR "enable FETCH_SUBMODULES to automatically fetch a Version of codecov, or provide a system version of Catch2")
		endif()
	endif()
endif()

find_package(codecov REQUIRED)

endif()
######################


