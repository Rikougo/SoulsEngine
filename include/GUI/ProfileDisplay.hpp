//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_PROFILE_DISPLAY_HPP
#define ELYS_PROFILE_DISPLAY_HPP

#include <imgui.h>

#include <Core/Profiling.hpp>
#include <GUI/GUI.hpp>

namespace Elys::GUI {
        void ProfileDisplay(Corner corner) {
            ImGuiIO &io = ImGui::GetIO();
            ImVec2 windowPos, windowPosPivot;
            ConfigureWindowPos(corner, windowPos, windowPosPivot);
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

            ImGui::SetNextWindowSize(ImVec2(400, 200));

            if(ImGui::Begin("ProfileDisplay", nullptr, windowFlags)) {
                ImGui::Text("Framerate : %0.1f", Profile::Framerate);
                ImGui::Text("Total time : %0.3f", Profile::DeltaTime);
                ImGui::Text("Draw time : %0.3f", Profile::DrawingTime);
                ImGui::Text("\t- Computing bounding boxes : %0.3f", Profile::ComputingBoundingBoxes);
                ImGui::Text("\t- Drawing meshes : %0.3f", Profile::DrawingMeshes);
                ImGui::Text("GUI time : %0.3f", Profile::GUITime);
                ImGui::Text("DrawnMesh : %d", Profile::DrawnMesh);
                ImGui::End();
            }
        }
    } // namespace Elys

#endif // ELYS_PROFILE_DISPLAY_HPP
