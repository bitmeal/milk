# milk-config.cmake - package configuration file for milk library
#
# exports:
# 		milk - as INTERFACE library target with its INTERFACE_INCLUDE_DIRECTORIES set
#
# sets:
# 		milk_INCLUDE_DIR - the directory containing milk.h



if(NOT TARGET milk)
  include("${CMAKE_CURRENT_LIST_DIR}/milkTargets.cmake")
  get_target_property(milk_INCLUDE_DIR milk INTERFACE_INCLUDE_DIRECTORIES)
endif()
