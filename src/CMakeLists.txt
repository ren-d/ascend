# includes from https://github.com/KStocky/ShaderTestFramework
include(NugetPackages)
include(AssetDependencyManagement)

add_executable(ascend WIN32)
asset_dependency_init(ascend)

nuget_initialize(ascend)
nuget_get_agility_sdk(ShaderTestFramework AGILITY_FOUND AGILITY_INCLUDE AGILITY_DLL SDK_VER)
nuget_get_dxc(ascend DXC_FOUND DXC_INCLUDE DXC_DLL DXC_LIB)
#nuget_get_warp(ShaderTestFramework WARP_FOUND WARP_DLL)
#nuget_get_WinPixEventRuntime(ShaderTestFramework WINPIX_FOUND WINPIX_INCLUDE WINPIX_BIN)
add_library(DXC SHARED IMPORTED)
set_target_properties(DXC PROPERTIES
    IMPORTED_IMPLIB "${DXC_LIB}/dxcompiler.lib"
)


set(SOURCES ascend/main.cpp 
			ascend/Renderer.cpp 
			ascend/Renderer.h 
			ascend/WindowsApplication.cpp 
			ascend/WindowsApplication.h
)
file(GLOB_RECURSE AGILITY_HEADERS "${AGILITY_INCLUDE}/*.h*" )
file(GLOB_RECURSE DXC_HEADERS "${DXC_INCLUDE}/*.h*" )

target_sources(ascend PRIVATE ${SOURCES})
target_sources(ascend PRIVATE ${AGILITY_HEADERS})
target_sources(ascend PRIVATE ${DXC_HEADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source" FILES ${SOURCES})
source_group(TREE ${AGILITY_INCLUDE} PREFIX "ThirdParty/AgilitySDK" FILES ${AGILITY_HEADERS})
source_group(TREE ${DXC_INCLUDE} PREFIX "ThirdParty/DXC" FILES ${DXC_HEADERS})

target_compile_definitions(ascend PRIVATE D3D12_SDK_VERSION_ID=${SDK_VER})
target_include_directories(ascend PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Public/ ${AGILITY_INCLUDE} ${DXC_INCLUDE})

target_link_libraries(ascend PRIVATE d3d12.lib dxgi.lib DXC dxguid.lib)

target_add_asset_directory(ascend ${AGILITY_DLL} "/D3D12")
target_add_asset_directory(ascend ${DXC_DLL} "/")
add_definitions(-DUNICODE -D_UNICODE)
add_compile_definitions(DEBUG)