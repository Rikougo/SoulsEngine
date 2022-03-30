//
// Created by Sam on 3/29/2022.
//

#include <Core/Profiling.hpp>

namespace Elys {
    float Profile::Framerate;
    float Profile::AverageFramerate;
    float Profile::DeltaTime;
    unsigned int Profile::TotalMesh;
    unsigned int Profile::DrawnMesh;
    float Profile::DrawingTime;
    float Profile::GUITime;
    float Profile::ComputingBoundingBoxes;
    float Profile::DrawingMeshes;

    void Profile::Init() {
        Framerate = 0.0f;
        AverageFramerate = 0.0f;
        TotalMesh = 0;
        DrawnMesh = 0;
        DeltaTime = 0.0f;
        DrawingTime = 0.0f;
        GUITime = 0.0f;
        ComputingBoundingBoxes = 0.0f;
        DrawingMeshes = 0.0f;
    }
}