//
// Created by sakeiru on 4/1/2022.
//

#include <GUI/GraphScene.hpp>

namespace Elys::GUI {
    void GraphScene::OnImGUIRender(std::shared_ptr<Scene> &sceneRef) {
        if (ImGui::Begin("Graph scene")) {
            for (auto const &entity : (*sceneRef)) {
                if (!entity.Parent().IsValid()) {
                    DrawEntity(sceneRef, entity);
                }
            }
        }
        ImGui::End();
    }
    void GraphScene::DrawEntity(std::shared_ptr<Scene> &sceneRef, Entity const &entity) {
        auto const &node = entity.GetComponent<Node>();
        ImGuiTreeNodeFlags nodeFlags = BaseNodeFlags;
        if (entity == mSelected)
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        if (node.Children().empty())
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)entity.ID(), nodeFlags, "%s", entity.GetComponent<Tag>().name.c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            mSelected = entity;

        if (!node.Children().empty() && node_open) {
            for(const auto child : entity.Children())
                DrawEntity(sceneRef, child);
            ImGui::TreePop();
        }
    }
}