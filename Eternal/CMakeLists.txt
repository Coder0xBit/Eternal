file(GLOB_RECURSE ETERNAL_SOURCE src/*.cpp src/*.h)

add_library(EternalCore STATIC ${ETERNAL_SOURCE})

target_include_directories(EternalCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

if (WIN32)
    target_link_directories(EternalCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/lib)
    target_compile_definitions(EternalCore PUBLIC GLEW_STATIC GLFW_INCLUDE_VULKAN VK_USE_PLATFORM_WIN32_KHR)
    target_link_libraries(EternalCore PUBLIC opengl32 glew32s glfw3 vulkan-1 ImGui)
else ()
    message(FATAL_ERROR "❌ Unsupported platform: Only Windows is supported right now.")
endif ()