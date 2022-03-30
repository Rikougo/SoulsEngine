//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_INPUT_HPP
#define ELYS_INPUT_HPP

#include <glm/glm.hpp>

#include <Core/KeyCode.hpp>

namespace Elys {
    class Input {
      public:
        static bool IsKeyPressed(KeyCode code);
        static bool IsMouseButtonPressed(MouseCode button);
        static glm::vec2 GetMousePosition();
    };
}

#endif // ELYS_INPUT_HPP
