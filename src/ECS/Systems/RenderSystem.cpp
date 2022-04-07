//
// Created by Sam on 3/30/2022.
//

#include <ECS/Systems/RenderSystem.hpp>

#include <glad/glad.h>

namespace Elys {
    RenderSystem::RenderSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer) :
        mCurrentScene(scene), mCamera(camera), mShader(shader), mFramebuffer(framebuffer) {}

    void RenderSystem::SetCamera(const TrackBallCamera &camera) { mCamera = std::make_unique<TrackBallCamera>(camera); }

    void RenderSystem::SetScene(shared_ptr<Scene> &sceneRef) {
        if (mCurrentScene) {
            ELYS_CORE_WARN("RenderSystem::SetScene : Shouldn't change scene if already one exists (may be due to wrong usage).");
            return;
        }

        mCurrentScene = sceneRef;
    }

    void RenderSystem::Update(float deltaTime) {
        if (mProcessInputs) ProcessInput();
        mFramebuffer->Bind();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("RenderSystem::Update : {0} not initialized (nullptr) !", mShader ? "Camera" : "Shader");
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

        for (auto id : mEntities) {
            auto entity = Entity(mCurrentScene.get(), id);

            auto const &node = entity.GetComponent<Node>();
            auto model = node.InheritedTransform();
            auto const &mesh = entity.GetComponent<MeshRenderer>();

            auto const &boundingBox = mesh.mesh.GetAABB();

            if (mFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
                continue;
            }

            // DRAWING MESH
            auto mat = mesh.material;

            mShader->SetVec4("uMaterial.albedo", mat.albedo);
            mShader->SetBool("uMaterial.hasTexture", mat.texture.has_value());
            if (mat.texture) {
                auto const &texture = mat.texture;
                glActiveTexture(GL_TEXTURE0);
                mShader->SetInt("uTexture", GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->id);
            }

            mShader->SetFloat("uMaterial.metallic", mat.metallic);
            mShader->SetFloat("uMaterial.roughness", std::max(0.05f, mat.roughness));

            mShader->SetMat4("uModel", model);
            mShader->SetBool("uLightsOn", mLightning && !mat.selfLight);

            mShader->SetInt("uEntity", static_cast<int>(id));

            glPolygonMode(GL_FRONT_AND_BACK, mWireframe ? GL_LINE : GL_FILL);
            glBindVertexArray(mesh.mesh.VAO());
            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.mesh.IndicesSize(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            Profile::DrawnMesh++;
        }

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

    void RenderSystem::SetViewportSize(glm::vec2 offset, glm::vec2 size) {
        if (mCamera) {
            mCamera->SetViewSize(size.x, size.y);
        }

        if (mFramebuffer) {
            mFramebuffer->Resize(size.x, size.y);
        }
    }

    bool RenderSystem::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == Key::W && event.GetRepeatCount() == 0) mWireframe = !mWireframe;
        if (event.GetKeyCode() == Key::F && event.GetRepeatCount() == 0) mFrustumCulling = !mFrustumCulling;

        return false;
    }
}