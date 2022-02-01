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

const auto aspect_ratio = 16.0f / 9.0f;
const int width = 800;
const int height = static_cast<int>(width / aspect_ratio);

using namespace std::chrono;

int main(int argc, char *argv[]) {
    std::cout << "Please read README.md for any information." << std::endl;

    auto app = make_shared<Core::Application>();
    auto winManager = Core::WindowManager(app, "Elys");

    auto shader = Core::Rendering::Shader{"./shaders/vertex_shader.glsl",
                         "./shaders/fragment_shader.glsl"};
    auto cube = Core::Rendering::Mesh::Sphere();

    app->AddEventListener(Core::Events::Game::SWITCH_MESH, [&cube](Core::Event &event) {
        int type = event.GetParam<int>(Core::EventsParams::MESH_TYPE);

        if (type) {
            cube = Core::Rendering::Mesh::Sphere();
        } else {
            cube = Core::Rendering::Mesh::Cube();
        }
    });

    auto now = high_resolution_clock::now();
    auto then = now;

    float rotation = 0.0f;

    while (!winManager.ShouldClose()) {
        then = now;
        now = high_resolution_clock::now();

        float deltaTime = duration_cast<duration<float>>(now - then).count();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3{0.0f, 0.0f, -2.0f},
                                     {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        glm::mat4 model{1.0f};

        model = glm::scale(model, {0.5f, 0.5f, 0.5f});
        model = glm::rotate(model, glm::radians(rotation),
                            glm::vec3{1.0f, 0.0f, 0.0f});

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        shader.setVec3("lightColor", glm::vec3{1.0f, 1.0f, 1.0f});
        shader.setVec3("lightPos", glm::vec3{1.0f, 1.0f, -1.0f});
        shader.setVec3("viewPos", glm::vec3{0.0f, 0.0f, -1.0f});

        shader.setVec3("objectColor", glm::vec3{0.7f, 0.5f, 1.0f});

        cube.draw(shader);

        rotation += 30.0f * deltaTime;

        winManager.ProcessEvents();
        winManager.Update();
    }

    winManager.Shutdown();

    return EXIT_SUCCESS;
}