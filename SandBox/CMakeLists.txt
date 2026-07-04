file(GLOB_RECURSE SANDBOX_SOURCE src/*.cpp src/*.h)

add_executable(VortakApp ${SANDBOX_SOURCE})

target_link_libraries(VortakApp PRIVATE VortakCore)
target_include_directories(VortakApp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor
)

add_custom_command(TARGET VortakApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
)
