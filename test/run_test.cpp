#include <catch2/catch_test_macros.hpp>
#include "TestRenderer.h"
TEST_CASE("Do Basic Addition", "[addition]")
{
	REQUIRE(2+2 == 4);
}

TEST_CASE("Initialize Debug Layer successfully", "[d3d12]")
{
	Renderer render;
	REQUIRE(render.InitializeDebugLayer() == true);
}