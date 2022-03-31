//
// Created by Sam on 3/24/2022.
//

#ifndef ELYS_SYSTEM_HPP
#define ELYS_SYSTEM_HPP

#include <set>

#include <Core/Base.hpp>

namespace Elys{
    class System {
      public:
        virtual void Update(float deltaTime) = 0;

        virtual void OnKeyPressed(KeyPressedEvent &event) {};
        virtual void OnKeyReleased(KeyReleasedEvent &event) {};
        virtual void OnMouseButtonPressed(MouseButtonPressedEvent &event) {};
        virtual void OnMouseButtonReleased(MouseButtonReleasedEvent &event) {};
        virtual void OnMouseScroll(MouseScrolledEvent &event) {};

        std::set<EntityID> mEntities;
    };
}

#endif // ELYS_SYSTEM_HPP
