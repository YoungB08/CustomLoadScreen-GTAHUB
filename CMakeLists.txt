cmake_minimum_required(VERSION 3.20)

cmake_policy(SET CMP0091 NEW)

project(CustomLoadScreen LANGUAGES C CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "CustomLoadScreen must be built with a 32-bit toolchain.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(CustomLoadScreen SHARED
    CustomLoadScreen.cpp
    loader.cpp
    llmo/ccallhook.cpp
    llmo/memsafe.cpp
    llmo/cshortasm.cpp
    sys/mman.c
    d3d9/proxydirectx.cpp
    d3d9/directx.cpp
    d3d9/color.cpp
    d3d9/d3drender.cpp
    d3d9/texture.cpp
    d3d9/MenuManager/ContextMenu.cpp
    d3d9/MenuManager/Listing.cpp
    d3d9/MenuManager/Menu.cpp
    d3d9/MenuManager/Node.cpp
    d3d9/MenuManager/NodeMenu.cpp
    d3d9/MenuManager/Text.cpp
    d3d9/MenuManager/VerticalLayout.cpp
    d3d9/MenuManager/CSlider.cpp
    CGame/CPed.cpp
    CGame/CPhysical.cpp
    CGame/Types.cpp
    CGame/methods.cpp
    CGame/CCam.cpp
    CGame/CCamera.cpp
    CGame/CVehicle.cpp
)

target_include_directories(CustomLoadScreen PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(CustomLoadScreen PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
    target_compile_options(CustomLoadScreen PRIVATE
        /EHsc
    )
    target_link_options(CustomLoadScreen PRIVATE
        /MAP:CustomLoadScreen.map /DEBUG
    )
    set_property(TARGET CustomLoadScreen PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    target_link_options(CustomLoadScreen PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

set(DIRECTX_SDK_ROOT "$ENV{DXSDK_DIR}" CACHE PATH "Path to the DirectX SDK")
if(NOT DIRECTX_SDK_ROOT AND EXISTS "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")
    set(DIRECTX_SDK_ROOT "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)" CACHE PATH "Path to the DirectX SDK" FORCE)
endif()

find_path(DIRECTX_INCLUDE_DIR
    NAMES d3dx9.h
    HINTS "${DIRECTX_SDK_ROOT}/Include"
)

if(DIRECTX_INCLUDE_DIR)
    target_include_directories(CustomLoadScreen BEFORE PRIVATE "${DIRECTX_INCLUDE_DIR}")
endif()

if(MSVC)
    find_library(D3DX9_LIBRARY
        NAMES d3dx9
        HINTS "${DIRECTX_SDK_ROOT}/Lib/x86"
    )

    if(NOT D3DX9_LIBRARY)
        message(FATAL_ERROR "d3dx9.lib was not found. Install DirectX SDK June 2010 or set DIRECTX_SDK_ROOT.")
    endif()

    target_link_libraries(CustomLoadScreen PRIVATE
        d3d9
        "${D3DX9_LIBRARY}"
        gdi32
        user32
        kernel32
    )
else()
    target_link_libraries(CustomLoadScreen PRIVATE
        d3d9
        d3dx9
        gdi32
        user32
        kernel32
    )
endif()

set_target_properties(CustomLoadScreen PROPERTIES
    PREFIX ""
    SUFFIX ".asi"
    OUTPUT_NAME "CustomLoadScreen"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
)
