//
// Created by Sam on 1/30/2022.
//

#ifndef ELYS_CAMERA_HPP
#define ELYS_CAMERA_HPP

#include "Rendering/Shader.hpp"

namespace Core::Rendering {
class Camera {
  public:
    void apply(Shader &shader);
};
}

#endif // ELYS_CAMERA_HPP
