//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_LAYER_HPP
#define ELYS_LAYER_HPP

#include <string>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>

namespace Elys {
    class Layer {
      public:
        Layer(const std::string &name = "Layer") : mDebugName(name) {};
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event &event) {}
      protected:
        std::string mDebugName;
    };
}

#endif // ELYS_LAYER_HPP
