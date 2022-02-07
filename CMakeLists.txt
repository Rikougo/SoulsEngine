cmake_minimum_required(VERSION 3.18)

project(Elys VERSION 0.1
    DESCRIPTION "Basic OpenGl app built with GLFW and Dear imgui to provide basic 3D representation and a raytracing conversion."
    LANGUAGES CXX C)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/)
find_package(OpenGL REQUIRED)

set(
    sources
    src/ECS/System.cpp
    src/ECS/ComponentArray.cpp
    src/Managers/SystemManager.cpp
    src/Managers/ComponentManager.cpp
    src/Managers/EntityManager.cpp
    src/Managers/WindowManager.cpp
    src/Managers/EventManager.cpp
    src/Rendering/Shader.cpp
    src/Rendering/Mesh.cpp
    src/Rendering/Camera.cpp
    src/Rendering/Model.cpp
    src/Systems/RenderingSystem.cpp
    src/Systems/PhysicSystem.cpp
    src/Application.cpp
    main.cpp
)

# Copy GLSL files into build directory
# file(COPY shaders/ DESTINATION shaders/)
# file(COPY assets/ DESTINATION assets/)

add_executable(${CMAKE_PROJECT_NAME} ${sources})

if (MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus)
endif()

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw OpenGL::GL IMGUI Glad assimp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include/ libs/ libs/glfw/include/ libs/glad/include/ libs/glm/ libs/include/)

