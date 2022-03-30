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
        void SetBlocking(bool blocking) { mBlockingEvents = blocking; }

        void OnAttach() override;
        void OnDetach() override;
        void OnImGuiRender() override;
        void OnEvent(Event &event) override;
      private:
        bool mBlockingEvents = false;
    };
}

#endif // ELYS_TEST_LAYER_HPP
