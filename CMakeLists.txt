cmake_minimum_required(VERSION 3.29.2)
project(Eternal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Eternal)
add_subdirectory(SandBox)

file(GLOB_RECURSE IMGUI_SOURCE
        Vendor/imgui/*.cpp
        Vendor/imgui/*.h
        Vendor/imgui/backends/*.cpp
        Vendor/imgui/backends/*.h)

add_library(ImGui STATIC ${IMGUI_SOURCE})

target_include_directories(ImGui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/include
)