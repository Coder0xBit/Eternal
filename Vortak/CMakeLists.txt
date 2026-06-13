file(GLOB_RECURSE VORTAK_SOURCE src/*.cpp src/*.h)

add_library(VortakCore STATIC ${VORTAK_SOURCE})

target_include_directories(VortakCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

if (WIN32)
    target_link_directories(VortakCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/lib)
    target_compile_definitions(VortakCore PUBLIC GLEW_STATIC GLFW_INCLUDE_VULKAN VK_USE_PLATFORM_WIN32_KHR)
    target_link_libraries(VortakCore PUBLIC opengl32 glew32s glfw3 vulkan-1 ImGui)
else ()
    message(FATAL_ERROR "❌ Unsupported platform: Only Windows is supported right now.")
endif ()