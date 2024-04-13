cmake_minimum_required(VERSION 3.5)
message("ascend.")
project(
	ascend 
	VERSION 0.0.1
	DESCRIPTION "ascend."
	LANGUAGES CXX)
add_subdirectory(src)
add_subdirectory(test)