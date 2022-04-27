//
// Created by Sam on 4/7/2022.
//

#include <ECS/Systems/LightSystem.hpp>

#include <glad/glad.h>

namespace Elys {
    LightSystem::LightSystem(
        shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer)
        : mCurrentScene(scene), mCamera(camera), mShader(shader), mFramebuffer(framebuffer) {
        LIGHT_MESH = Mesh::Sphere();
    }

    void LightSystem::Update(float deltaTime) {
        mFramebuffer->Bind();

        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("LightSystem::Update : {0} not initialized (nullptr) !", mShader ? "Camera" : "Shader");
            mFramebuffer->Unbind();
            return;
        }

        uint8_t lightIndex = 0;

        mShader->Use();
        mShader->SetMat4("uProjection", mCamera->GetProjection());
        mShader->SetMat4("uView", mCamera->GetView());
        mShader->SetBool("uLightsOn", true);
        for (auto id : mEntities) {
            if (lightIndex >= MAX_LIGHT) {
                ELYS_CORE_WARN("More light components than system can handle.");
                break;
            }

            auto entity = Entity(mCurrentScene.get(), id);

            auto const &node = entity.GetComponent<Node>();
            if (!node.InheritedEnabled()) continue;

            auto const &light = entity.GetComponent<Light>();

            mShader->SetVec3("uLights[" + std::to_string(lightIndex) + "].position", node.InheritedPosition());
            mShader->SetVec3("uLights[" + std::to_string(lightIndex) + "].color", light.color);
            mShader->SetFloat("uLights[" + std::to_string(lightIndex) + "].intensity", light.intensity);
            // mShader->SetMat4("uModel", node.InheritedTransform());

            /*if (!LIGHT_MESH.IsInit()) {
                ELYS_CORE_WARN("Light mesh not init !");
            } else {
                glBindVertexArray(LIGHT_MESH.VAO());
                glDrawElements(GL_TRIANGLES, (GLsizei)LIGHT_MESH.IndicesSize(), GL_UNSIGNED_INT, nullptr);
                glBindVertexArray(0);
            }*/

            lightIndex++;
        }

        mShader->SetInt("uLightsAmount", lightIndex);

        mFramebuffer->Unbind();
    }
} // namespace Elys