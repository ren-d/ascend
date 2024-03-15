set(SOURCES ascend/main.cpp)
add_executable(ascend ${SOURCES})
target_link_libraries(
		rendering PRIVATE
		d3d12.lib
		dxgi.lib
		dxguid.lib)