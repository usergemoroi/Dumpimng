cmake_minimum_required(VERSION 3.16)
project(Lorer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_DEMO "Build standalone demo application" OFF)
option(BUILD_DLL "Build main DLL for injection" ON)

# Find DirectX
find_path(DIRECTX_INCLUDE_DIRS NAMES d3d11.h 
    PATHS "C:/Program Files (x86)/Windows Kits/10/Include/*/um"
    DOC "DirectX include directory")

# ImGui source files
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui" CACHE PATH "Path to ImGui directory")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# Core source files
set(CORE_SOURCES
    LorerMenu/LorerMenu.cpp
    LorerMenu/MemoryManager.cpp
    LorerMenu/CheatFeatures.cpp
)

set(CORE_HEADERS
    LorerMenu/LorerMenu.h
    LorerMenu/LorerOverlay.h
    LorerMenu/LorerIntegration.h
    LorerMenu/Offsets.h
    LorerMenu/MemoryManager.h
    LorerMenu/CheatFeatures.h
)

# Common include directories
set(CORE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/LorerMenu
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Core library (static)
add_library(LorerCore STATIC
    ${CORE_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(LorerCore PUBLIC
    ${CORE_INCLUDE_DIRS}
)

target_link_libraries(LorerCore PUBLIC
    d3d11
    dxgi
    user32
    gdi32
    psapi
)

target_compile_definitions(LorerCore PRIVATE
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
)

# Main DLL for injection
if(BUILD_DLL)
    add_library(LorerDLL SHARED
        LorerMenu/DllMain.cpp
        ${CORE_SOURCES}
        ${IMGUI_SOURCES}
    )
    
    target_include_directories(LorerDLL PRIVATE
        ${CORE_INCLUDE_DIRS}
    )
    
    target_link_libraries(LorerDLL PRIVATE
        d3d11
        dxgi
        user32
        gdi32
        psapi
    )
    
    # MinHook integration
    set(MINHOOK_DIR "${CMAKE_SOURCE_DIR}/external/minhook" CACHE PATH "Path to MinHook directory")
    
    if(EXISTS "${MINHOOK_DIR}/include/MinHook.h")
        target_include_directories(LorerDLL PRIVATE ${MINHOOK_DIR}/include)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_link_libraries(LorerDLL PRIVATE ${MINHOOK_DIR}/lib/libMinHook.x64.lib)
        else()
            target_link_libraries(LorerDLL PRIVATE ${MINHOOK_DIR}/lib/libMinHook.x86.lib)
        endif()
    endif()
    
    target_compile_definitions(LorerDLL PRIVATE
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS
    )
    
    set_target_properties(LorerDLL PROPERTIES
        OUTPUT_NAME "Lorer"
        PREFIX ""
    )
endif()

# Standalone demo
if(BUILD_DEMO)
    add_executable(LorerDemo WIN32
        LorerMenu/LorerDemo.cpp
        ${CORE_SOURCES}
        ${IMGUI_SOURCES}
    )
    
    target_include_directories(LorerDemo PRIVATE
        ${CORE_INCLUDE_DIRS}
    )
    
    target_link_libraries(LorerDemo PRIVATE
        d3d11
        dxgi
        user32
        gdi32
        psapi
    )
    
    target_compile_definitions(LorerDemo PRIVATE
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /utf-8)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /GL)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    endif()
else()
    add_compile_options(-Wall -Wextra)
endif()

# Install rules
install(TARGETS LorerCore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${CORE_HEADERS}
    DESTINATION include/LorerMenu
)
