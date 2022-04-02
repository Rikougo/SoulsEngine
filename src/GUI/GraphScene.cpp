//
// Created by sakeiru on 4/1/2022.
//

#include <GUI/GraphScene.hpp>

namespace Elys::GUI {
    void GraphScene::OnImGUIRender(std::shared_ptr<Scene> &sceneRef) {
        if (ImGui::Begin("Graph scene")) {
            for(auto &entity : (*sceneRef)) {
                if (!entity.Parent().IsValid()) {
                    auto &node = entity.GetComponent<Node>();

                    ImGui::PushID(entity.ID());

                    if (ImGui::Selectable(entity.GetComponent<Tag>().name.c_str(), mSelected == entity))
                        mSelected = entity;

                    ImGui::PopID();
                }
            }

            ImGui::End();
        }
    }
}