//
// Created by sakeiru on 4/1/2022.
//

#ifndef ELYS_GRAPH_SCENE_HPP
#define ELYS_GRAPH_SCENE_HPP

#include <imgui.h>

#include <ECS/Scene.hpp>

namespace Elys::GUI {
    class GraphScene {
      public:
        void OnImGUIRender(std::shared_ptr<Scene> &sceneRef, bool *open);

        Entity& GetSelected() { return mSelected; };
        void SetSelected(Entity entity) { mSelected = entity; }
      private:
        void DrawEntity(std::shared_ptr<Scene> &sceneRef, Entity &entity);
      private:
        ImGuiTreeNodeFlags BaseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        Entity mSelected;
    };
}

#endif // ELYS_GRAPH_SCENE_HPP
