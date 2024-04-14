set(SOURCES ascend/main.cpp 
			ascend/Renderer.cpp 
			ascend/Renderer.h 
			ascend/WindowsApplication.cpp 
			ascend/WindowsApplication.h)
add_executable(ascend WIN32 ${SOURCES})
target_link_libraries(
		ascend PRIVATE
		d3d12.lib
		dxgi.lib
)
add_definitions(-DUNICODE -D_UNICODE)