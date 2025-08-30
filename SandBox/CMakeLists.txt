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