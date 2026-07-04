file(GLOB_RECURSE VORTAK_SOURCE src/*.cpp src/*.h)

find_package(Vulkan REQUIRED)

add_library(VortakCore STATIC ${VORTAK_SOURCE})

target_include_directories(VortakCore
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

if (WIN32)
    target_link_directories(VortakCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/lib)
    target_compile_definitions(VortakCore PUBLIC
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            GLFW_INCLUDE_VULKAN
            VK_USE_PLATFORM_WIN32_KHR
    )
    target_link_libraries(
            VortakCore PUBLIC
            assimp
            Vulkan::Vulkan
            spdlog::spdlog
            EnTT::EnTT
            glm::glm
            stb
            opengl32
            glad
            glfw
            ImGui
    )
else ()
    message(FATAL_ERROR "❌ Unsupported platform: Only Windows is supported right now.")
endif ()

add_custom_command(TARGET VortakCore POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
)

if (Vulkan_GLSLC_EXECUTABLE)
    set(GLSLC_EXECUTABLE ${Vulkan_GLSLC_EXECUTABLE})
else ()
    find_program(GLSLC_EXECUTABLE NAMES glslc HINTS "$ENV{VULKAN_SDK}/Bin")
endif ()

set(SHADER_DIR ../Vortak/res/shader)
set(SHADER_OUTPUT_DIR ../Vortak/res/shader/bin)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

add_custom_target(Shaders ALL
        COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=vertex ${SHADER_DIR}/common_vertex.glsl -o ${SHADER_OUTPUT_DIR}/common_vertex.spv
        COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=fragment ${SHADER_DIR}/common_fragment.glsl -o ${SHADER_OUTPUT_DIR}/common_fragment.spv
        COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=fragment ${SHADER_DIR}/frag_texture_mapping.glsl -o ${SHADER_OUTPUT_DIR}/frag_texture_mapping.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building SPIR-V shaders"
)

add_dependencies(VortakCore Shaders)