#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <memory>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.hpp"
#include "Managers/WindowManager.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Camera.hpp"

const auto aspect_ratio = 16.0f / 9.0f;
const int width = 800;
const int height = static_cast<int>(width / aspect_ratio);

using namespace std::chrono;

float Core::Time::DeltaTime = 0.0f;

int main(int argc, char *argv[]) {
    std::cout << "Please read README.md for any information." << std::endl;

    auto app = make_shared<Core::Application>();
    auto winManager = Core::WindowManager(app, "Elys");

    auto shader = Core::Rendering::Shader{
        "./shaders/vertex_shader.glsl",
        "./shaders/fragment_shader.glsl"
    };
    auto cube = Core::Rendering::Mesh::Cube();
    float rotation = 0.0f;
    auto currentType = 0;

    auto camera = Core::Rendering::Camera();

    // mesh modification
    app->AddEventListener(Core::Events::Game::SWITCH_MESH, [&cube, &currentType](Core::Event &event) {
        int type = event.GetParam<int>(Core::EventsParams::MESH_TYPE);

        if (type == 0 && type != currentType) {
            currentType = type;
            cube = Core::Rendering::Mesh::Cube();
        } else if (type == 1 && type != currentType ) {
            currentType = type;
            cube = Core::Rendering::Mesh::Sphere();
        } else if (type == 2 && type != currentType) {
            currentType = type;
            cube = Core::Rendering::Mesh::Plane();
        }
    });

    // camera movement
    app->AddEventListener(Core::Events::Game::CAMERA_MOVE, [&camera](Core::Event &event) {
        auto direction = event.GetParam<glm::vec2>(Core::EventsParams::CAMERA_MOVE_DIRECTION);

        camera.move(direction, 3.0f * Core::Time::DeltaTime);
    });

    // MAIN RENDER
    app->AddEventListener(Core::Events::Window::UPDATE, [&cube, &camera, &rotation, &shader](Core::Event &event) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model{1.0f};
        model = glm::scale(model, {0.5f, 0.5f, 0.5f});
        model = glm::rotate(model, glm::radians(rotation),
                            glm::vec3{1.0f, 0.0f, 0.0f});

        camera.apply(shader);
        shader.setMat4("model", model);

        shader.setVec3("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
        shader.setVec3("lightPos", glm::vec3{1.0f, 1.0f, -1.0f});
        shader.setVec3("viewPos", glm::vec3{0.0f, 0.0f, -1.0f});

        shader.setVec3("objectColor", glm::vec3{0.7f, 0.5f, 1.0f});

        cube.draw(shader);

        rotation += 30.0f * Core::Time::DeltaTime;
    });

    auto now = high_resolution_clock::now();
    auto then = now;

    while (!winManager.ShouldClose()) {
        then = now;
        now = high_resolution_clock::now();

        Core::Time::DeltaTime = duration_cast<duration<float>>(now - then).count();

        app->DispatchEvent(Core::Events::Window::UPDATE);

        winManager.ProcessEvents();
        winManager.Update();
    }

    winManager.Shutdown();

    return EXIT_SUCCESS;
}