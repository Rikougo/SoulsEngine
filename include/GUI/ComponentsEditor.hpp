//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_COMPONENTS_EDITOR_HPP
#define ELYS_COMPONENTS_EDITOR_HPP

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

#include <GUI/GUI.hpp>
#include <GUI/ContentBrowser.hpp>

#include <ECS/Scene.hpp>

namespace Elys::GUI {
    class ComponentsEditor {
      public:
        void OnImGUIRender(Entity entity, bool* open);
      private:
        void NodeEditor(std::string const &label, Node &node);
        void MeshRenderEditor(std::string const &label, MeshRenderer &meshRenderer);
        void LightEditor(std::string const &label, Light &light);
        void RigidBodyEditor(std::string const &label, RigidBody &rBody);
        void PlayerEditor(std::string const &label, Player &player);
    };
}

#endif // ELYS_COMPONENTS_EDITOR_HPP
