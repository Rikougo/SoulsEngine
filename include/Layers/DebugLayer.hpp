//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_DEBUG_LAYER_HPP
#define ELYS_DEBUG_LAYER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <Core/Base.hpp>
#include <Core/Logger.hpp>
#include <Core/Layer.hpp>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>

namespace Elys {
    class DebugLayer : public Layer {
      public:
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event &event) override;
      private:
        bool OnKeyPressed(KeyPressedEvent &event);
        void DrawDebugOverlay() const;
      private:
        float mFPS = 0.0f;
        bool mShowDebugOverlay = false;
        bool mShowDemoWindow = false;
    };
}

#endif // ELYS_DEBUG_LAYER_HPP
