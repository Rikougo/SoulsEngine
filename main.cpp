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
#include "Rendering/Model.hpp"

#define DEBUG_ true

const auto aspect_ratio = 16.0f / 9.0f;
const int width = 800;
const int height = static_cast<int>(width / aspect_ratio);

using namespace std::chrono;

float Core::Time::DeltaTime = 0.0f;

int main(int argc, char *argv[]) {
    std::cout << "[Running on C++ : " << __cplusplus << "]" << std::endl;
    std::cout << "Please read README.md for any information." << std::endl;

    auto app = make_shared<Core::Application>();
    auto winManager = Core::WindowManager(app, "Elys");

    auto shader = Core::Rendering::Shader{
        "./shaders/model_vertex.glsl",
        "./shaders/model_fragment.glsl"
    };
    auto cube = Core::Rendering::Mesh::Cube();
    float rotation = 0.0f;
    auto currentType = 0;

    auto model = Core::Rendering::Model("./assets/model/icosphere.obj");

    auto camera = Core::Rendering::Camera();

    // camera movement
    app->AddEventListener(Core::Events::Game::CAMERA_MOVE, [&camera](Core::Event &event) {
        auto direction = event.GetParam<glm::vec2>(Core::EventsParams::CAMERA_MOVE_DIRECTION);

        camera.move(direction, 3.0f * Core::Time::DeltaTime);
    });

    auto now = high_resolution_clock::now();
    auto then = now;

    while (!winManager.ShouldClose()) {
        then = now;
        now = high_resolution_clock::now();

        Core::Time::DeltaTime = duration_cast<duration<float>>(now - then).count();

        glClearColor(0.7, 0.5, 0.6, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        winManager.ProcessEvents();
        winManager.Update();
    }

    winManager.Shutdown();

    return EXIT_SUCCESS;
}