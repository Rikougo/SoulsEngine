#define STB_IMAGE_IMPLEMENTATION

#include <chrono>
#include <memory>
#include <utility>

#include <Application.hpp>
#include <Managers/WindowManager.hpp>

#include <Physics/RigidBody.hpp>
#include <Physics/Transform.hpp>

#include <Rendering/Mesh.hpp>
#include <Rendering/Shader.hpp>

#include <Systems/RenderingSystem.hpp>
#include <Systems/PhysicSystem.hpp>

#define DEBUG_ true

const auto aspect_ratio = 16.0f / 9.0f;
const int width = 800;
const int height = static_cast<int>(width / aspect_ratio);

using namespace std::chrono;
using namespace Core;

float Time::DeltaTime = 0.0f;

int main(int argc, char *argv[]) {
    try {
        std::cout << "[Running on C++ : " << __cplusplus << "]" << std::endl;
        std::cout << "Please read README.md for any information." << std::endl;

        auto app = make_shared<Core::Application>();

        auto winManager = WindowManager(app, "Elys");

        auto shader = Rendering::Shader{"./shaders/model_vertex.glsl",
                                        "./shaders/model_fragment.glsl"};

        app->RegisterComponent<Rendering::Mesh>();
        app->RegisterComponent<Physics::Transform>();
        app->RegisterComponent<Physics::RigidBody>();

        // ==========================
        // RENDERING SYSTEM
        // ==========================
        auto renderingSystem =
            app->RegisterSystem<Rendering::RenderingSystem>();
        Signature renderSignature;
        renderSignature.set(app->GetComponentType<Physics::Transform>());
        renderSignature.set(app->GetComponentType<Rendering::Mesh>());
        app->SetSystemSignature<Rendering::RenderingSystem>(renderSignature);
        renderingSystem->SetShader(shader);

        // ==========================
        // PHYSIC SYSTEM
        // ==========================
        auto physicSystem =
            app->RegisterSystem<Physics::PhysicSystem>();
        Signature physicSignature;
        physicSignature.set(app->GetComponentType<Physics::Transform>());
        physicSignature.set(app->GetComponentType<Physics::RigidBody>());
        app->SetSystemSignature<Physics::PhysicSystem>(physicSignature);

        auto cubeEntity = app->CreateEntity();
        auto cubeMesh = Rendering::Mesh::Cube();
        app->AddComponent(cubeEntity, cubeMesh);
        app->AddComponent(cubeEntity, Physics::Transform{{1.0f, 0.0f, 0.0f},
                                                         {0.0f, 0.0f, 0.0f},
                                                         {0.5f, 0.5f, 0.5f}});

        auto sphereEntity = app->CreateEntity();
        auto sphereMesh = Rendering::Mesh::Sphere();
        app->AddComponent(sphereEntity, sphereMesh);
        app->AddComponent(sphereEntity, Physics::Transform{{-1.0f, 0.0f, 0.0f},
                                                           {0.0f, 0.0f, 0.0f},
                                                           {0.5f, 0.5f, 0.5f}});
        app->AddComponent(sphereEntity, Physics::RigidBody{{0.0f, 0.0f, 0.5f}});

        auto now = high_resolution_clock::now();
        auto then = now;

        while (!winManager.ShouldClose()) {
            then = now;
            now = high_resolution_clock::now();

            Time::DeltaTime =
                duration_cast<duration<float>>(now - then).count();

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