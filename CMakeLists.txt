cmake_minimum_required(VERSION 3.6)
message("ascend.")
project(
	ascend 
	VERSION 0.0.1
	DESCRIPTION "ascend."
	LANGUAGES CXX)
add_subdirectory(src)
add_subdirectory(test)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ascend)