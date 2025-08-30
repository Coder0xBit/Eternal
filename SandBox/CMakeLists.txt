file(GLOB_RECURSE SANDBOX_SOURCE src/*.cpp src/*.h)

add_executable(EternalEditor ${SANDBOX_SOURCE})

target_link_libraries(EternalEditor PRIVATE EternalCore)
target_include_directories(EternalEditor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

add_custom_command(TARGET EternalEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
)

#set(GLSLC_EXECUTABLE "C:/VulkanSDK/1.3.216.0/Bin/glslc.exe")

if(DEFINED ENV{VULKAN_SDK})
    set(GLSLC_EXECUTABLE "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set! Please install Vulkan SDK and set it properly.")
endif()

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/shader)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res/shader/bin)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

add_custom_target(Shaders ALL
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_DIR}/shader.vert -o ${SHADER_OUTPUT_DIR}/vert.spv
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_DIR}/shader.frag -o ${SHADER_OUTPUT_DIR}/frag.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building SPIR-V shaders"
)

add_dependencies(EternalEditor Shaders)