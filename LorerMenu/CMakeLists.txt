cmake_minimum_required(VERSION 3.16)
project(Lorer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_LOADER "Build loader executable" ON)
option(BUILD_DLL "Build cheat DLL" ON)
option(BUILD_DEMO "Build standalone demo" OFF)

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

# Core source files (shared)
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/LorerMenu/LorerMenu.cpp
    ${CMAKE_SOURCE_DIR}/LorerMenu/MemoryManager.cpp
    ${CMAKE_SOURCE_DIR}/LorerMenu/CheatFeatures.cpp
)

set(CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/LorerMenu/LorerMenu.h
    ${CMAKE_SOURCE_DIR}/LorerMenu/LorerOverlay.h
    ${CMAKE_SOURCE_DIR}/LorerMenu/LorerIntegration.h
    ${CMAKE_SOURCE_DIR}/LorerMenu/Offsets.h
    ${CMAKE_SOURCE_DIR}/LorerMenu/MemoryManager.h
    ${CMAKE_SOURCE_DIR}/LorerMenu/CheatFeatures.h
)

# Common include directories
set(CORE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/LorerMenu
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Common libraries
set(WIN_LIBS d3d11 dxgi user32 gdi32 psapi advapi32 shlwapi)

# ============================================================
# Loader Executable (Loader.exe)
# ============================================================
if(BUILD_LOADER)
    add_executable(Loader WIN32
        ${CMAKE_SOURCE_DIR}/LorerMenu/Loader/Loader.cpp
    )
    
    target_include_directories(Loader PRIVATE
        ${CORE_INCLUDE_DIRS}
    )
    
    target_link_libraries(Loader PRIVATE
        ${WIN_LIBS}
    )
    
    target_compile_definitions(Loader PRIVATE
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS
    )
    
    set_target_properties(Loader PROPERTIES
        OUTPUT_NAME "Loader"
    )
endif()

# ============================================================
# Main Cheat DLL (Lorer.dll)
# ============================================================
if(BUILD_DLL)
    add_library(LorerDLL SHARED
        ${CMAKE_SOURCE_DIR}/LorerMenu/DllMain.cpp
        ${CORE_SOURCES}
        ${IMGUI_SOURCES}
    )
    
    target_include_directories(LorerDLL PRIVATE
        ${CORE_INCLUDE_DIRS}
    )
    
    target_link_libraries(LorerDLL PRIVATE
        ${WIN_LIBS}
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
        SUFFIX ".dll"
    )
endif()

# ============================================================
# Standalone Demo (for testing menu without game)
# ============================================================
if(BUILD_DEMO)
    add_executable(LorerDemo WIN32
        ${CMAKE_SOURCE_DIR}/LorerMenu/LorerDemo.cpp
        ${CORE_SOURCES}
        ${IMGUI_SOURCES}
    )
    
    target_include_directories(LorerDemo PRIVATE
        ${CORE_INCLUDE_DIRS}
    )
    
    target_link_libraries(LorerDemo PRIVATE
        ${WIN_LIBS}
    )
    
    target_compile_definitions(LorerDemo PRIVATE
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# ============================================================
# Core Static Library (for reuse)
# ============================================================
add_library(LorerCore STATIC
    ${CORE_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(LorerCore PUBLIC
    ${CORE_INCLUDE_DIRS}
)

target_link_libraries(LorerCore PUBLIC
    ${WIN_LIBS}
)

target_compile_definitions(LorerCore PRIVATE
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
)

# ============================================================
# Compiler Options
# ============================================================
if(MSVC)
    add_compile_options(/W4 /utf-8 /EHsc)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /GL)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    endif()
else()
    add_compile_options(-Wall -Wextra)
endif()

# ============================================================
# Install Rules
# ============================================================
install(TARGETS Loader LorerDLL
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION bin
)

install(FILES ${CORE_HEADERS}
    DESTINATION include/LorerMenu
)

# ============================================================
# Build Info
# ============================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Lorer Build Configuration")
message(STATUS "========================================")
message(STATUS "Build Loader: ${BUILD_LOADER}")
message(STATUS "Build DLL:    ${BUILD_DLL}")
message(STATUS "Build Demo:   ${BUILD_DEMO}")
message(STATUS "ImGui Dir:    ${IMGUI_DIR}")
message(STATUS "MinHook Dir:  ${MINHOOK_DIR}")
message(STATUS "========================================")
message(STATUS "")
