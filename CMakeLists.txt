cmake_minimum_required(VERSION 3.14)

project(Elys VERSION 0.1
    DESCRIPTION "Basic OpenGl game engine."
    LANGUAGES CXX C)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/)
find_package(OpenGL REQUIRED)

set(
    SOURCES
    src/Core/Window.cpp
    src/Core/Logger.cpp
    src/Application.cpp
    main.cpp
)

# Copy GLSL files into build directory
# file(COPY shaders/ DESTINATION shaders/)
# file(COPY assets/ DESTINATION assets/)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

if (MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus)
endif()

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw OpenGL::GL IMGUI Glad)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include/ libs/glfw/include/ libs/glad/include/ libs/glm/ libs/include/ libs/spdlog/include/)

