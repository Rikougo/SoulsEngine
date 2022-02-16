//
// Created by Sam on 2/7/2022.
//

#include "Systems/RenderingSystem.hpp"

using namespace Core::Rendering;

void RenderingSystem::SetShader(Core::Rendering::Shader &shader) {
    mShader = shader;
}

void RenderingSystem::Update(std::shared_ptr<Application> &app) {
    glClearColor(0.7, 0.5, 0.6, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto const &entity : mEntities) {
        try {
            auto const &mesh = app->GetComponent<Mesh>(entity);
            auto const &transform = app->GetComponent<Physics::Transform>(entity);

            mCamera.Apply(mShader);
            mShader.SetMat4("model", transform.ModelMatrix());

            mesh.Draw(mShader);
        } catch (std::runtime_error const &e) {
            std::cerr << "Core::Rendering::RenderingSystem::Update : "
                      << e.what() << std::endl;
        }
    }
}
