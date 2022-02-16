//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_SYSTEMS_RENDERING_SYSTEM_HPP
#define ELYS_SYSTEMS_RENDERING_SYSTEM_HPP

#include <memory>

#include <ECS/System.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Camera.hpp>
#include <Physics/Transform.hpp>
#include <Application.hpp>
#include <Utils.hpp>

namespace Core::Rendering {
class RenderingSystem : public System {
  public:
    Shader mShader;
    Camera mCamera;

  public:
    RenderingSystem() : mShader{"./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl"} { };

    void SetShader(Shader &shader);
    void Update(std::shared_ptr<Application> &app);
};
}

#endif // ELYS_SYSTEMS_RENDERING_SYSTEM_HPP
