cmake_minimum_required(VERSION 3.16)
project(LorerMenu VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(DirectX REQUIRED)

# ImGui source files (adjust paths according to your ImGui installation)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui" CACHE PATH "Path to ImGui directory")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# Source files
set(LORER_SOURCES
    LorerMenu/LorerMenu.cpp
)

set(LORER_HEADERS
    LorerMenu/LorerMenu.h
    LorerMenu/LorerOverlay.h
    LorerMenu/LorerIntegration.h
)

# Demo executable
add_executable(LorerDemo WIN32
    LorerMenu/LorerDemo.cpp
    ${LORER_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(LorerDemo PRIVATE
    ${CMAKE_SOURCE_DIR}/LorerMenu
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${DirectX_INCLUDE_DIRS}
)

target_link_libraries(LorerDemo PRIVATE
    d3d11
    dxgi
    user32
    gdi32
)

# Core library (for integration into other projects)
add_library(LorerCore STATIC
    ${LORER_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(LorerCore PUBLIC
    ${CMAKE_SOURCE_DIR}/LorerMenu
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(LorerCore PUBLIC
    d3d11
    dxgi
    user32
)

# MinHook integration (for hooking functionality)
option(USE_MINHOOK "Use MinHook for DirectX hooking" ON)
if(USE_MINHOOK)
    set(MINHOOK_DIR "${CMAKE_SOURCE_DIR}/external/minhook" CACHE PATH "Path to MinHook directory")
    
    if(EXISTS "${MINHOOK_DIR}/include/MinHook.h")
        target_include_directories(LorerCore PRIVATE ${MINHOOK_DIR}/include)
        target_include_directories(LorerDemo PRIVATE ${MINHOOK_DIR}/include)
        target_link_libraries(LorerCore PRIVATE ${MINHOOK_DIR}/lib/libMinHook.x64.lib)
        target_link_libraries(LorerDemo PRIVATE ${MINHOOK_DIR}/lib/libMinHook.x64.lib)
    endif()
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(LorerCore PRIVATE /W4 /utf-8)
    target_compile_options(LorerDemo PRIVATE /W4 /utf-8)
    target_compile_definitions(LorerDemo PRIVATE UNICODE _UNICODE)
    target_compile_definitions(LorerCore PRIVATE UNICODE _UNICODE)
else()
    target_compile_options(LorerCore PRIVATE -Wall -Wextra)
    target_compile_options(LorerDemo PRIVATE -Wall -Wextra)
endif()

# Install rules
install(TARGETS LorerCore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${LORER_HEADERS}
    DESTINATION include/LorerMenu
)
