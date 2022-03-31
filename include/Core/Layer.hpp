//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_LAYER_HPP
#define ELYS_LAYER_HPP

#include <string>

#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>
#include <utility>

namespace Elys {
    class Layer {
      public:
        Layer(std::string name = "Layer") : mDebugName(std::move(name)) {};
        virtual ~Layer() = default;

        /// \Brief
        /// Consider this as the Layer constructor, Instantiate/initialize variables here.
        virtual void OnAttach() {}

        /// \Brief
        /// Consider this as the Layer destructor, free any memory here.
        virtual void OnDetach() {}

        /// \Brief
        /// Gameloop update
        virtual void OnUpdate(float deltaTime) {}

        /// \Brief
        /// Occurs after gameloop update, used to render GUI. NO NEED TO CALL ImGui::Begin NOR ImGui::End !
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event &event) {}
      protected:
        std::string mDebugName;
    };
}

#endif // ELYS_LAYER_HPP
