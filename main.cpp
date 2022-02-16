#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <memory>
#include <random>
#include <utility>

#include <Application.hpp>

#include <Benchmark/ECSBenchmark.hpp>

#include <Managers/WindowManager.hpp>

#include <Physics/RigidBody.hpp>
#include <Physics/Transform.hpp>

#include <Rendering/Mesh.hpp>
#include <Rendering/Shader.hpp>

#include <Systems/PhysicSystem.hpp>
#include <Systems/RenderingSystem.hpp>

#define DEBUG_ true

constexpr auto aspect_ratio = 16.0f / 9.0f;
constexpr int width = 800;
constexpr int height = static_cast<int>(width / aspect_ratio);

using namespace std::chrono;
using namespace Core;

float Time::DeltaTime = 0.0f;

int main(int argc, char *argv[]) {
    try {
        std::cout << "[Running on C++ : " << __cplusplus << "]" << std::endl;
        std::cout << "Please read README.md for any information." << std::endl;

        auto app = make_shared<Core::Application>();
        auto winManager = WindowManager(app, "Elys");

        // ==========================
        // COMPONENTS REGISTER
        // ==========================
        app->RegisterComponent<Rendering::Mesh>();
        app->RegisterComponent<Physics::Transform>();
        app->RegisterComponent<Physics::RigidBody>();

        // ==========================
        // RENDERING SYSTEM
        // ==========================
        auto renderingSystem = app->RegisterSystem<Rendering::RenderingSystem>();
        Signature renderSignature;
        renderSignature.set(app->GetComponentType<Physics::Transform>());
        renderSignature.set(app->GetComponentType<Rendering::Mesh>());
        app->SetSystemSignature<Rendering::RenderingSystem>(renderSignature);

        // ==========================
        // PHYSIC SYSTEM
        // ==========================
        auto physicSystem = app->RegisterSystem<Physics::PhysicSystem>();
        Signature physicSignature;
        physicSignature.set(app->GetComponentType<Physics::Transform>());
        physicSignature.set(app->GetComponentType<Physics::RigidBody>());
        app->SetSystemSignature<Physics::PhysicSystem>(physicSignature);

        app->AddEventListener(Events::Window::RESIZE, [&renderingSystem](Event &event) {
            auto nSize = event.GetParam<glm::ivec2>(Core::Events::Window::Params::NEW_SIZE);

            renderingSystem->mCamera.SetAspect((float)nSize.x / (float)nSize.y);
        });

        app->AddEventListener(Events::Window::UPDATE, [&winManager, &renderingSystem, centered = true, cumulator = 0.0f, xpos = 0.0, ypos = 0.0, yaw = 0.0f, pitch = 0.0f] (Event &event) mutable {
            auto window = winManager.getWindow();

            cumulator += Time::DeltaTime;

            if (cumulator > 0.1f) {
                if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                    centered = !centered;
                    renderingSystem->mCamera.SetFree(!centered);

                    glfwSetInputMode(window, GLFW_CURSOR, !centered ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

                    cumulator = 0.0f;
                }

                cumulator = 0.2f;
            }

            glm::vec2 cameraDirection{0.0f, 0.0f};

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                cameraDirection.y += 1;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                cameraDirection.y -= 1;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                cameraDirection.x += 1;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                cameraDirection.x -= 1;

            if (glm::length(cameraDirection) > 0) {
                cameraDirection = glm::normalize(cameraDirection);

                renderingSystem->mCamera.move(cameraDirection, centered ? 5.0f : 2.5f);
            }

            if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            // mouse input
            if (!centered) {
                double newXPos, newYPos;
                glfwGetCursorPos(window, &newXPos, &newYPos);
                double xoffset = newXPos - xpos;
                double yoffset = ypos - newYPos; // reversed since y-coordinates range from bottom to top

                xpos = newXPos; ypos = newYPos;

                const float sensitivity = 0.1f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                yaw   += xoffset;
                pitch += yoffset;

                if(pitch > 89.0f)
                    pitch = 89.0f;
                if(pitch < -89.0f)
                    pitch = -89.0f;

                glm::vec3 direction;
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                renderingSystem->mCamera.SetFront(glm::normalize(direction));
            }
        });

        Core::Benchmark::HeightMapBenchmark(app, winManager);

        auto now = high_resolution_clock::now();
        auto then = now;

        while (!winManager.ShouldClose()) {
            then = now;
            now = high_resolution_clock::now();

            Time::DeltaTime = duration_cast<duration<float>>(now - then).count();

            app->DispatchEvent(Events::Window::UPDATE);

            physicSystem->Update(app);
            renderingSystem->Update(app);

            winManager.ProcessEvents();
            winManager.Update();

        }

        winManager.Shutdown();
        return EXIT_SUCCESS;
    } catch (std::runtime_error const &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}