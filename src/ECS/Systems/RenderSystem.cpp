//
// Created by Sam on 3/30/2022.
//

#include <ECS/Systems/RenderSystem.hpp>

#include <glad/glad.h>

namespace Elys {
    RenderSystem::RenderSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera,
                               shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer)
        : mCurrentScene(scene), mCamera(camera), mShader(shader), mFramebuffer(framebuffer) {
        mOutlineShader = std::make_shared<Shader>("./shaders/outline_vertex.glsl",
                                                  "./shaders/outline_fragment.glsl");
    }

    void RenderSystem::SetCamera(const TrackBallCamera &camera) {
        mCamera = std::make_unique<TrackBallCamera>(camera);
    }

    void RenderSystem::SetScene(shared_ptr<Scene> &sceneRef) {
        if (mCurrentScene) {
            ELYS_CORE_WARN("RenderSystem::SetScene : Shouldn't change scene if already one exists "
                           "(may be due to wrong usage).");
            return;
        }

        mCurrentScene = sceneRef;
    }

    void RenderSystem::Update(float deltaTime) {
        if (mProcessInputs)
            ProcessInput();
        mFramebuffer->Bind();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // float clearColorValue[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        int clearEntityValue[4] = {-1, -1, -1, -1};
        glClearBufferiv(GL_COLOR, 1, clearEntityValue);

        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("RenderSystem::Update : {0} not initialized (nullptr) !",
                            mShader ? "Camera" : "Shader");
            mFramebuffer->Unbind();
            return;
        }

        mShader->Use();
        mShader->SetMat4("uProjection", mCamera->GetProjection());
        mShader->SetMat4("uView", mCamera->GetView());
        mShader->SetVec3("uViewPos", mCamera->GetPosition());

        Profile::DrawnMesh = 0;
        Profile::ComputingBoundingBoxes = 0.0f;
        Profile::DrawingMeshes = 0.0f;

        auto frustum = mCamera->GetFrustum();

        // Enable stencil writing to draw full shape on stencil buffer
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        for (auto id : mEntities) {
            // Only keep draw right on hover/selected entities to avoid
            // useless shape collapses
            glStencilMask(mCurrentScene->GetSelected() == id || mCurrentScene->GetHovered() == id ? 0xFF : 0x00);

            auto entity = Entity(mCurrentScene.get(), id);

            auto const &node = entity.GetComponent<Node>();
            auto model = node.InheritedTransform();
            auto const &renderer = entity.GetComponent<MeshRenderer>();

            auto &mesh = renderer.mesh;
            auto &material = renderer.material;

            auto const &boundingBox = mesh.GetAABB();

            if (mFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
                continue;
            }

            // drawing on stencil mask
            // DRAWING MESH
            mShader->SetVec4("uMaterial.albedo", material.albedo);
            mShader->SetBool("uMaterial.hasTexture", material.texture.has_value());
            if (material.texture) {
                auto const &texture = material.texture;
                glActiveTexture(GL_TEXTURE0);
                mShader->SetInt("uTexture", GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->ID());
            }

            mShader->SetFloat("uMaterial.metallic", material.metallic);
            mShader->SetFloat("uMaterial.roughness", std::max(0.05f, material.roughness));

            mShader->SetMat4("uModel", model);
            mShader->SetBool("uLightsOn", mLightning && !material.selfLight);

            mShader->SetInt("uEntity", static_cast<int>(id));

            mesh.VAO()->Bind();
            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.VAO()->GetIndexBuffer()->Size(),
                           GL_UNSIGNED_INT, nullptr);
            mesh.VAO()->Unbind();

            Profile::DrawnMesh++;
        }

        // OUTLINING
        mOutlineShader->Use();
        mOutlineShader->SetMat4("uProjection", mCamera->GetProjection());
        mOutlineShader->SetMat4("uView", mCamera->GetView());

        mOutlineShader->Use();
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        for (auto id : mEntities) {
            if (id != mCurrentScene->GetSelected() && id != mCurrentScene->GetHovered()) continue;

            auto entity = Entity(mCurrentScene.get(), id);
            auto model = entity.GetComponent<Node>().InheritedTransform();
            auto const &renderer = entity.GetComponent<MeshRenderer>();

            auto &mesh = renderer.mesh;

            mOutlineShader->SetMat4("uModel", model);
            auto outlineScale = mat4{1.0};
            outlineScale = glm::scale(outlineScale, {1.04, 1.04, 1.04});
            mOutlineShader->SetMat4("uOutlineScale", outlineScale);
            glm::vec4 outlineColor =
                (id == mCurrentScene->GetSelected()) ?
                    vec4{191.0f / 255.0f, 139.0f / 255.0f, 99.0f / 255.0f, 1.0f} :
                    vec4{242.0f / 255.0f, 124.0f / 255.0f, 34.0f / 255.0f, 1.0f};
            mOutlineShader->SetVec4("uOutlineColor", outlineColor);

            mesh.VAO()->Bind();
            glDrawElements(
                GL_TRIANGLES,
                (GLsizei)mesh.VAO()->GetIndexBuffer()->Size(),
                GL_UNSIGNED_INT,
                nullptr
            );
            mesh.VAO()->Unbind();
        }

        // Enable modifying of the stencil buffer
        glStencilMask(0xFF);
        // Clear stencil buffer
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        mFramebuffer->Unbind();
        mProcessInputs = false;
    }

    void RenderSystem::ProcessInput() {
        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
            auto mPos = Input::GetMousePosition();
            mCamera->MouseInput(mPos.x, mPos.y, Mouse::ButtonLeft);

            // Input::SetCursorMode(Cursor::Disabled);
        } else if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
            auto mPos = Input::GetMousePosition();
            mCamera->MouseInput(mPos.x, mPos.y, Mouse::ButtonRight);

            // Input::SetCursorMode(Cursor::Disabled);
        } else {
            mCamera->EndCapture();
            // Input::SetCursorMode(Cursor::Normal);
        }
    }

    bool RenderSystem::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == Key::W && event.GetRepeatCount() == 0) mWireframe = !mWireframe;
        if (event.GetKeyCode() == Key::F && event.GetRepeatCount() == 0) mFrustumCulling = !mFrustumCulling;

        return false;
    }
}