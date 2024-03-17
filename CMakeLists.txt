cmake_minimum_required(VERSION 3.20.0)
message("ascend.")
enable_testing()
project(
	ascend 
	VERSION 0.0.1
	DESCRIPTION "ascend."
	LANGUAGES CXX)
add_subdirectory(src)
add_subdirectory(test)