//
// Created by Sam on 3/30/2022.
//

#include <ECS/Systems/RenderSystem.hpp>

// important to include in cpp file
// otherwise it will cause circular dependency issues
#include <ECS/Scene.hpp>

namespace Elys {
    RenderSystem::RenderSystem() {
        mShader = std::make_unique<Shader>("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");
        mCamera = std::make_unique<TrackBallCamera>();
    }

    void RenderSystem::SetCamera(const TrackBallCamera &camera) { mCamera = std::make_unique<TrackBallCamera>(camera); }
    void RenderSystem::SetScene(Scene *scene) {
        if (mCurrentScene != nullptr) {
            ELYS_CORE_WARN("RenderSystem::SetScene : Shouldn't change scene if already one exists (may be due to wrong usage).");
        }

        mCurrentScene = scene;
    }

    void RenderSystem::Update(float deltaTime) {
        if (!mShader || !mCamera) {
            ELYS_CORE_ERROR("RenderSystem::Update : {0} not initialized (nullptr) !", mShader ? "Camera" : "Shader");
            return;
        }

        ProcessInput();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mShader->Use();
        mShader->SetMat4("uProjection", mCamera->GetProjection());
        mShader->SetMat4("uView", mCamera->GetView());

        mShader->SetVec3("uLightPosition", mCamera->GetPosition());

        Profile::DrawnMesh = 0;
        Profile::ComputingBoundingBoxes = 0.0f;
        Profile::DrawingMeshes = 0.0f;

        auto frustum = mCamera->GetFrustum();

        float then = 0.0f;

        for (auto id : mEntities) {
            auto entity = Entity(mCurrentScene, id);

            then = (float)glfwGetTime();
            auto const &node = entity.GetComponent<Node>();
            auto model = node.InheritedTransform();
            auto const &mesh = entity.GetComponent<MeshRenderer>();
            Profile::DrawingMeshes += (float)glfwGetTime() - then;

            then = (float)glfwGetTime();
            auto const &boundingBox = mesh.Mesh.GetAABB();

            if (mFrustumCulling && !boundingBox.IsInFrustum(frustum, model)) {
                Profile::ComputingBoundingBoxes += (float)glfwGetTime() - then;
                return;
            }
            Profile::ComputingBoundingBoxes += (float)glfwGetTime() - then;

            // TODO DRAW DEBUG INFORMATION SUCH AS BOUNDING BOXES
            /*if (mDebugMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                mShader->SetVec3("uColor", {0.0, 0.5, 0.0});
                mShader->SetBool("uShadingOn", false);
            }*/

            // DRAWING MESH
            then = (float)glfwGetTime();

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
            Profile::DrawingMeshes += (float)glfwGetTime() - then;

            Profile::DrawnMesh++;
        }
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

    void RenderSystem::OnViewportChange(float width, float height) {
        if (mCamera) {
            mCamera->SetViewSize(width, height);
        }

        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderSystem::OnKeyPressed(KeyPressedEvent &event) {
        if (event.GetKeyCode() == Key::W) mWireframe = !mWireframe;
        if (event.GetKeyCode() == Key::F) mFrustumCulling = !mFrustumCulling;
    }

    void RenderSystem::OnMouseScroll(MouseScrolledEvent &event) {
        mCamera->Zoom(event.GetYOffset());
    }
}