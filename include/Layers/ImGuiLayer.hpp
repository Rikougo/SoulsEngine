//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_TEST_LAYER_HPP
#define ELYS_TEST_LAYER_HPP

#include <Core/Layer.hpp>

namespace Elys {
    class ImGuiLayer : public Layer {
      public:
        void Begin();
        void End();
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event &event) override;
    };
}

#endif // ELYS_TEST_LAYER_HPP
