//
// Created by Sam on 2/14/2022.
//

#include "Benchmark/ECSBenchmark.hpp"

void Core::Benchmark::MaxCubeBenchmark(std::shared_ptr<Core::Application> &app) {
    constexpr array<const char *, 8> texturesPath = {
        "assets/textures/June_01.png",   "assets/textures/Nougat_01.png",
        "assets/textures/Nougat_02.png", "assets/textures/wow_dog.png",
        "assets/textures/Wow.jpg",       "assets/textures/fox.png",
        "assets/textures/boom.jpg",      "assets/textures/ground_sci_fi.jpg"};

    // ==========================
    // ENTITY CREATION
    // ==========================
    std::default_random_engine generator;
    std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
    std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> randScale(0.8f, 1.2f);
    std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);
    std::uniform_int randMesh(0, 7);

    vector<Core::Rendering::Texture> textures(texturesPath.size());
    vector<Core::Rendering::Mesh> meshes(textures.size());

    for (uint8_t i = 0; i < textures.size(); i++) {
        textures[i] = Core::Rendering::Texture{.id = TextureFromFile(texturesPath[i], "./"),
                                               .type = "texture_diffuse",
                                               .path = texturesPath[i]};

        meshes[i] = Core::Rendering::Mesh::Cube();
        meshes[i].setupMesh();
        meshes[i].setTexture(textures[i]);
    }

    for (uint32_t i = 0; i < Core::MAX_ENTITIES; i++) {
        auto scale = randScale(generator);

        auto transform = Core::Physics::Transform{
            .position = {randPosition(generator), randPosition(generator), randPosition(generator)},
            .rotation = {0.0f, 0.0f, 0.0f},
            .scale = {scale, scale, scale}};

        auto rigidBody = Core::Physics::RigidBody{.velocity = {0.0f, randGravity(generator), 0.0f}};

        auto entity = app->CreateEntity();

        app->AddComponent(entity, meshes[randMesh(generator)]);
        app->AddComponent(entity, transform);
        app->AddComponent(entity, rigidBody);
    }
}

void Core::Benchmark::HeightMapBenchmark(std::shared_ptr<Core::Application> &app,
                                         Core::WindowManager winManager) {
    auto planeEntity = app->CreateEntity();

    std::cout << "Created entity " << planeEntity << std::endl;

    int resolution = 2;

    auto mesh = Core::Rendering::Mesh::Plane(resolution, resolution);
    mesh.setupMesh();
    auto transform = Core::Physics::Transform{.position = {0.0f, 0.0f, 0.0f},
                                              .rotation = {0.0f, 0.0f, 0.0f},
                                              .scale = {100.0f, 100.0f, 100.0f}};

    mesh.ConfigureHeightMap("assets/textures/Heightmap_Rocky.png");

    mesh.setTexture("assets/textures/snowrocks.png");
    mesh.setTexture("assets/textures/rock.png");
    mesh.setTexture("assets/textures/grass.png");

    app->AddComponent(planeEntity, mesh);
    app->AddComponent(planeEntity, transform);

    app->AddEventListener(Events::Window::UPDATE, [planeEntity, resolution, &app, &winManager, cumulator = 0.0f](Event &event) mutable {
        cumulator += Time::DeltaTime;

        bool moved = false;
        glm::vec3 direction{0.0, 0.0, 0.0};
        if (glfwGetKey(winManager.getWindow(), GLFW_KEY_KP_2) == GLFW_PRESS) {
            direction.y -= 1;
            moved = true;
        }
        if (glfwGetKey(winManager.getWindow(), GLFW_KEY_KP_8) == GLFW_PRESS) {
            direction.y += 1;
            moved = true;
        }

        if (moved) {
            auto &transform = app->GetComponent<Core::Physics::Transform>(planeEntity);
            transform.position += direction * 5.0f * Time::DeltaTime;
        }

        if (cumulator < 0.1) return;

        cumulator = 0;

        bool changed = false;
        if (glfwGetKey(winManager.getWindow(), GLFW_KEY_KP_ADD) == GLFW_PRESS && resolution < (512)) {
            resolution = resolution * 2;
            changed = true;
        }
        if (glfwGetKey(winManager.getWindow(), GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS && resolution > 2) {
            resolution = resolution / 2;
            changed = true;
        }

        if (changed) {
            auto &mesh = app->GetComponent<Core::Rendering::Mesh>(planeEntity);
            mesh.updateMesh(Core::Rendering::Mesh::Plane(resolution, resolution));
        }
    });
}