//
// Created by Sam on 2/14/2022.
//

#ifndef ELYS_ECS_BENCHMARK_HPP
#define ELYS_ECS_BENCHMARK_HPP

#include <memory>
#include <random>

#include <Application.hpp>

#include <Managers/WindowManager.hpp>

#include <Rendering/Mesh.hpp>

#include <Physics/RigidBody.hpp>
#include <Physics/Transform.hpp>

#include <Utils.hpp>

namespace Core::Benchmark {
    /**
     * Testing application ECS by spawning max amount of cube with 8 different textures.
     * @param app
     */
    void MaxCubeBenchmark(std::shared_ptr<Application> &app);

    void HeightMapBenchmark(std::shared_ptr<Application> &app, Core::WindowManager winManager);
}

#endif // ELYS_ECS_BENCHMARK_HPP
