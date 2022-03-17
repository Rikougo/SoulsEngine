//
// Created by Sam on 3/11/2022.
//

#include "Layers/DebugLayer.hpp"

namespace Elys {
    void DebugLayer::OnUpdate(float deltaTime) {
        mFPS = 1.0f / deltaTime;
    }

    void DebugLayer::OnImGuiRender() {
        if (mShowDemoWindow) {
            ImGui::ShowDemoWindow();
        }
        if (mShowDebugOverlay) {
            DrawDebugOverlay();
        }
    }

    void DebugLayer::DrawDebugOverlay() const {
        static int corner = 0;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1)
        {
            const float PAD = 0.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.35f);
        if(ImGui::Begin("Profiling", NULL, window_flags)) {
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse Position: <invalid>");
            ImGui::Text("FPS : %.0f", mFPS);
        }
        ImGui::End();
    }

    void DebugLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(DebugLayer::OnKeyPressed));
    }

    bool DebugLayer::OnKeyPressed(KeyPressedEvent &e) {
        if (e.GetKeyCode() == GLFW_KEY_D) {
            mShowDebugOverlay = !mShowDebugOverlay;
        }

        if (e.GetKeyCode() == GLFW_KEY_N) {
            mShowDemoWindow = !mShowDemoWindow;
        }

        return false;
    }
}