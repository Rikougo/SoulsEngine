//
// Created by Sam on 3/30/2022.
//

#include <ECS/Systems/RenderSystem.hpp>

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

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("RenderSystem::Update : {0} not initialized (nullptr) !", mShader ? "Camera" : "Shader");
            mFramebuffer->Unbind();
            return;
        }

        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("{0} not set in RenderSystem::Update and is required.", mShader ? "Shader" : "Camera");
            return;
        }

        mShader->Use();
        mShader->SetMat4("uProjection", mCamera->GetProjection());
        mShader->SetMat4("uView", mCamera->GetView());

        mShader->SetVec3("uLightPosition", mCamera->GetPosition());

        Profile::DrawnMesh = 0;
        Profile::ComputingBoundingBoxes = 0.0f;
        Profile::DrawingMeshes = 0.0f;

        auto frustum = mCamera->GetFrustum();

        for (auto id : mEntities) {
            auto entity = Entity(mCurrentScene.get(), id);

            auto const &node = entity.GetComponent<Node>();
            auto model = node.InheritedTransform();
            auto const &mesh = entity.GetComponent<MeshRenderer>();

            auto const &boundingBox = mesh.Mesh.GetAABB();

            if (mFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
                return;
            }

            // TODO DRAW DEBUG INFORMATION SUCH AS BOUNDING BOXES
            /*if (mDebugMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                mShader->SetVec3("uColor", {0.0, 0.5, 0.0});
                mShader->SetBool("uShadingOn", false);
            }*/

            // DRAWING MESH
            auto mat = mesh.Material;

            bool hasTexture = mat.texture != nullptr;
            mShader->SetBool("uHasTexture", hasTexture);
            if (hasTexture) {
                auto const &texture = (*mat.texture);
                glActiveTexture(GL_TEXTURE0);
                mShader->SetInt("uTexture", GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture.id);
            } else {
                mShader->SetVec4("uColor", mat.color);
            }

            mShader->SetVec3("uMaterial.Ambient", mat.ambient);
            mShader->SetVec3("uMaterial.Diffuse", mat.diffuse);
            mShader->SetVec3("uMaterial.Specular", mat.specular);

            mShader->SetMat4("uModel", model);
            mShader->SetBool("uShadingOn", true);

            glPolygonMode(GL_FRONT_AND_BACK, mWireframe ? GL_LINE : GL_FILL);
            glBindVertexArray(mesh.Mesh.VAO());
            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.Mesh.IndicesSize(), GL_UNSIGNED_INT, nullptr);
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
        } else if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
            auto mPos = Input::GetMousePosition();
            mCamera->MouseInput(mPos.x, mPos.y, Mouse::ButtonRight);
        } else {
            mCamera->EndCapture();
        }
    }

    void RenderSystem::SetViewportSize(glm::vec2 offset, glm::vec2 size) {
        if (mCamera) {
            mCamera->SetViewSize(size.x, size.y);
        }

        mFramebuffer->Resize(size.x, size.y);
    }

    void RenderSystem::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == Key::W) mWireframe = !mWireframe;
        if (event.GetKeyCode() == Key::F) mFrustumCulling = !mFrustumCulling;
    }

    void RenderSystem::OnMouseScroll(MouseScrolledEvent &event) {
        mCamera->Zoom(event.GetYOffset());
    }
}