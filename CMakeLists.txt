cmake_minimum_required(VERSION 3.29.2)
project(Vortak LANGUAGES CXX)
project(Vortak LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Vendor/spdlog)
add_subdirectory(Vendor/tinyobjloader)
add_subdirectory(Vendor/stb)
add_subdirectory(Vendor/entt)
add_subdirectory(Vendor/glm)
add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/glad)
add_subdirectory(Vendor/imgui)

add_subdirectory(Vortak)
add_subdirectory(SandBox)