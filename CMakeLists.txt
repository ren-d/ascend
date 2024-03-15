cmake_minimum_required(VERSION 3.20.0)
message("ascend.")
project(
	ascend 
	VERSION 0.0.1
	DESCRIPTION "ascend."
	LANGUAGES CXX)
add_subdirectory(src)
target_link_libraries(
		rendering PRIVATE
		d3d12.lib
		dxgi.lib
		dxguid.lib)