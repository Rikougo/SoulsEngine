//
// Created by sakeiru on 4/1/2022.
//

#include <GUI/GraphScene.hpp>

namespace Elys::GUI {
    void GraphScene::OnImGUIRender(std::shared_ptr<Scene> &sceneRef, bool *open) {
        if (ImGui::Begin("Graph scene", open)) {
            for (auto entityID : (*sceneRef)) {
                auto entity = sceneRef->EntityFromID(entityID);
                if (!entity.Parent().IsValid()) {
                    DrawEntity(sceneRef, entity);
                }
            }

            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("Create entity")) {
                    sceneRef->CreateEntity();
                    ELYS_CORE_INFO("Create entity");
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
    void GraphScene::DrawEntity(std::shared_ptr<Scene> &sceneRef, Entity &entity) {
        auto &node = entity.GetComponent<Node>();
        ImGuiTreeNodeFlags nodeFlags = BaseNodeFlags;

        if (entity == sceneRef->EntityFromID(sceneRef->GetSelected()))
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        if (node.Children().empty())
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)entity.ID(), nodeFlags, "%s",
                                           entity.GetComponent<Node>().name.c_str());

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Create child")) {
                auto child = sceneRef->CreateEntity();
                child.SetParent(entity);
                ELYS_CORE_INFO("Create child");
            }
            /*if (ImGui::MenuItem("Delete")) {
                sceneRef->PushDestroyQueue(entity);
                ELYS_CORE_INFO("Delete entity");
            }*/
            ImGui::EndPopup();
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            sceneRef->SetSelected(entity.ID());

        if (ImGui::IsItemHovered())
            sceneRef->SetHovered(entity.ID());

        if (!(nodeFlags & ImGuiTreeNodeFlags_Leaf) && !node.Children().empty() && node_open) {
            for(auto child : entity.Children())
                DrawEntity(sceneRef, child);
            ImGui::TreePop();
        }
    }
}