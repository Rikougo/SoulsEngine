//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_GUI_HPP
#define ELYS_GUI_HPP

#include <imgui.h>

namespace Elys::GUI {
        enum Corner {
            TOP_LEFT     = 0b00,
            TOP_RIGHT    = 0b01,
            BOTTOM_LEFT  = 0b10,
            BOTTOM_RIGHT = 0b11
        };

        /// \Brief
        /// Take a corner value and edit windowPos and windowPosPivot given
        /// to position it to the chosen corner. You can also provide padding value.
        void ConfigureWindowPos(Corner corner, ImVec2 &windowPos, ImVec2 &windowPosPivot, float padding = 0.0f) {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_pos = viewport->WorkPos;
            ImVec2 work_size = viewport->WorkSize;
            windowPos.x = (corner & 1) ? (work_pos.x + work_size.x - padding) : (work_pos.x + padding);
            windowPos.y = (corner & 2) ? (work_pos.y + work_size.y - padding) : (work_pos.y + padding);
            windowPosPivot.x = (corner & 1) ? 1.0f : 0.0f;
            windowPosPivot.y = (corner & 2) ? 1.0f : 0.0f;
        }
    }

#endif // ELYS_GUI_HPP
