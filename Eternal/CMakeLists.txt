file(GLOB_RECURSE ETERNAL_SOURCE src/*.cpp src/*.h)

add_library(EternalCore STATIC ${ETERNAL_SOURCE})

target_include_directories(EternalCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

if (WIN32)
    # Windows-specific vendor libs
    target_link_directories(EternalCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/lib)
    target_link_libraries(EternalCore PUBLIC glew32s glfw3 vulkan-1 ImGui)
else ()
    message(FATAL_ERROR "❌ Unsupported platform: Only Windows is supported right now.")
endif ()