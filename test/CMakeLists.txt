enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG 	   v3.4.0
	)
FetchContent_MakeAvailable(Catch2)
add_executable(
	unit_tests
	"run_test.cpp")
set_target_properties(unit_tests Catch2 Catch2WithMain PROPERTIES FOLDER TestTargets)
target_link_libraries(
	unit_tests
	PRIVATE Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(unit_tests)
