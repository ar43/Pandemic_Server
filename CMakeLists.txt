cmake_minimum_required(VERSION 3.20)
project(Pandemic_Server LANGUAGES CXX)

# Enforce C++20 standard as specified in .vcxproj
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories matching .vcxproj configuration
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Collect all C++ source files recursively
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Define executable output
add_executable(Pandemic_Server ${SOURCES})

# Copy assets next to the executable
add_custom_command(TARGET Pandemic_Server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Pandemic_Server>/assets
)

# Platform-specific linking & definitions
if(WIN32)
    target_link_libraries(Pandemic_Server PRIVATE Ws2_32)
    target_compile_definitions(Pandemic_Server PRIVATE WIN32_LEAN_AND_MEAN _CONSOLE)
else()
    target_link_libraries(Pandemic_Server PRIVATE pthread)
endif()