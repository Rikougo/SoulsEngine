//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_ECS_LAYER_HPP
#define ELYS_ECS_LAYER_HPP

#include <filesystem>

#include <glm/glm.hpp>

#include <Core/Layer.hpp>

#include <ECS/Scene.hpp>
#include <ECS/Components.hpp>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/ApplicationEvent.hpp>

#include <Render/Shader.hpp>
#include <Render/Camera.hpp>

using glm::vec3;
using glm::vec2;

namespace Elys {
    struct Vertex {
        vec3 position;
        vec3 normal;
        vec2 texCoord;
    };

    class ECSLayer : public Layer {
      public:
        ECSLayer() = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;

      private:
        void Render();
        void PhysicUpdate(float deltaTime);

        void ImGuiDrawEntityTreeSelector(Entity const &entity);
        void DrawEntity(const Entity &entity, glm::mat4 parent = glm::mat4{1.0f});

        bool OnKeyPressed(KeyPressedEvent &event);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
        bool OnWindowResize(WindowResizeEvent &event);

        void CreateScene();
        void LoadScene(const std::filesystem::path& path);
        void SaveScene(const std::filesystem::path& path);
      private:
        Shader* mShader;
        Camera mCamera;

        // TEMP FOR MESH RENDERING
        std::vector<uint32_t> mTriangles;
        unsigned int mVAO, mVBO, mEBO;

        Scene mCurrentScene;
        bool mWireframeMode = false;

        Entity mSun;
        Entity mEarth;
        Entity mMoon;
        Entity mSelected;
    };
}

#endif // ELYS_ECS_LAYER_HPP
