enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG 	   v3.4.0
	)
FetchContent_MakeAvailable(Catch2)
set(SOURCES "run_test.cpp"
			"TestRenderer.h")
add_executable(
	unit_tests
	${SOURCES}
	)
set_target_properties(unit_tests Catch2 Catch2WithMain PROPERTIES FOLDER TestTargets)
target_link_libraries(
	unit_tests
	PRIVATE Catch2::Catch2WithMain d3d12.lib dxgi.lib )
add_definitions(-DUNICODE -D_UNICODE)
add_compile_definitions(DEBUG)
include(Catch)
catch_discover_tests(unit_tests)
