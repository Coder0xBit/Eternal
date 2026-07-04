find_package(Vulkan REQUIRED)

file(GLOB IMGUI_SOURCE
        *.cpp
        *.h
        backends/*.cpp
        backends/*.h
)

add_library(ImGui STATIC ${IMGUI_SOURCE})

target_include_directories(ImGui
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ImGui
        PUBLIC
        Vulkan::Vulkan
        glfw
)