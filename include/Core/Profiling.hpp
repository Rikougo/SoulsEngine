//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_PROFILING_HPP
#define ELYS_PROFILING_HPP

namespace Elys {
    class Profile {
      public:
        static void Init();

        static float Framerate;
        static float DeltaTime;
        static unsigned int TotalMesh;
        static unsigned int DrawnMesh;

        static float DrawingTime;
        static float GUITime;
        static float ComputingBoundingBoxes;
        static float DrawingMeshes;
    };
}; // namespace Elys

#endif // ELYS_PROFILING_HPP
