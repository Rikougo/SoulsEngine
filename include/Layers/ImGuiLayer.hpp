//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_TEST_LAYER_HPP
#define ELYS_TEST_LAYER_HPP

#include <Core/Layer.hpp>

#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>
#include <Events/ApplicationEvent.hpp>

namespace Elys {
    class ImGuiLayer : public Layer {
      public:
        void Begin();
        void End();
        void Blocking(bool blocking) { mBlockingEvents = blocking; }

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;
        void OnEvent(Event &event) override;
      private:
        bool OnMouseButtonPressed(MouseButtonPressedEvent &event) const;
        bool OnMouseButtonReleased(MouseButtonReleasedEvent &event) const;
        bool OnMouseMoved(MouseMovedEvent &event) const;
        bool OnMouseScrolled(MouseScrolledEvent &event) const;
        bool OnKeyPressedEvent(KeyPressedEvent &event) const;
        bool OnKeyReleasedEvent(KeyReleasedEvent &event) const;
        bool OnKeyTypedEvent(KeyTypedEvent &event) const;
        bool OnWindowResize(WindowResizeEvent &event) const;
      private:
        bool mBlockingEvents = false;
        float mTime = 0.0f;
    };
}

#endif // ELYS_TEST_LAYER_HPP
