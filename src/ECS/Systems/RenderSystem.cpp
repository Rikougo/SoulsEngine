//
// Created by Sam on 3/30/2022.
//

#include <ECS/Systems/RenderSystem.hpp>

namespace Elys {
    RenderSystem::RenderSystem(shared_ptr<Scene> &scene, shared_ptr<TrackBallCamera> &camera, shared_ptr<Shader> &shader, shared_ptr<Framebuffer> &framebuffer) :
        mCurrentScene(scene), mCamera(camera), mShader(shader), mFramebuffer(framebuffer) { mLineShader = std::make_shared<Shader>("./shaders/simplevertex.glsl", "./shaders/simplefragment.glsl");}

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

            auto const &boundingBox = mesh.mesh.GetAABB();

            if (mFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
                continue;
            }

            // DRAWING MESH
            auto mat = mesh.material;

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
            glBindVertexArray(mesh.mesh.VAO());
            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.mesh.IndicesSize(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            Profile::DrawnMesh++;

            if(boundingBox.ShouldDraw()) {
                mLineShader->Use();
                mLineShader->SetMat4("uProjection", mCamera->GetProjection());
                mLineShader->SetMat4("uView", mCamera->GetView());
                mLineShader->SetMat4("uModel", model);

                boundingBox.Draw();
                mShader->Use();

            }
        }

        mFramebuffer->Unbind();
        mProcessInputs = false;
    }
    void RenderSystem::ProcessInput() {
        // Keyboard Inputs
        if (Input::IsKeyPressed(Key::E)) {
            mCamera->Translate(TrackBallCamera::UP);
        }
        if (Input::IsKeyPressed(Key::Q)) {
            mCamera->Translate(TrackBallCamera::DOWN);
        }
        if (Input::IsKeyPressed(Key::A)) {
            mCamera->Translate(TrackBallCamera::LEFT);
        }
        if (Input::IsKeyPressed(Key::D)) {
            mCamera->Translate(TrackBallCamera::RIGHT);
        }
        if (Input::IsKeyPressed(Key::W)) {
            mCamera->Translate(TrackBallCamera::FRONT);
        }
        if (Input::IsKeyPressed(Key::S)) {
            mCamera->Translate(TrackBallCamera::BACK);
        }

        // Mouse Inputs
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

        ELYS_CORE_INFO("Frustum {0}", mFrustumCulling);

        return false;
    }
}