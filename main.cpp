#define STB_IMAGE_IMPLEMENTATION
#include <utility>

#include <Application.hpp>

#include <Core/Logger.hpp>
#include <Core/Profiling.hpp>

int main(int argc, char *argv[]) {
    try {
        Elys::Log::Init();
        Elys::Profile::Init();

        Elys::Application* app = Elys::CreateApplication();

        app->Run();

        return EXIT_SUCCESS;
    } catch (std::runtime_error const &e) {
        ELYS_CORE_FATAL("Caught error during runtime : {0}", e.what());

        return EXIT_FAILURE;
    }
}