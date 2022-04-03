//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <functional>
#include <memory>

#include <Core/Base.hpp>
#include <Core/Profiling.hpp>
#include <Core/Window.hpp>
#include <Core/LayerStack.hpp>

#include <Events/Event.hpp>

#include <Layers/ImGuiLayer.hpp>
#include <Layers/EditorLayer.hpp>

using namespace std;

int main(int argc, char* argv[]);

namespace Elys {
    class Application {
      public:
        ~Application();

        static Application &Get() { return *sInstance; }

        Window &GetWindow() { return *mWindow; }
        ImGuiLayer &GetImGUILayer() { return *mImGUILayer; }

        void OnEvent(Event &event);

        void Shutdown() { mRunning = false; }
      private:
        explicit Application(std::string name);

        /// \Brief
        /// Run the Gameloop, everything after this function call will happen once the window
        /// closed.
        void Run();

        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);
      private:
        unique_ptr<Window> mWindow;
        bool mRunning = true;
        bool mMinimized = false;
        float mLastFrameTime = 0.0f;
        ImGuiLayer *mImGUILayer;
        LayerStack mLayerStack;

        // STATIC INSTANCE HANDLE
      private:
        static Application *sInstance;
        friend int ::main(int argc, char *argv[]);
        friend Application *CreateApplication(std::string name);
    };

    /// \Brief
    /// Application is a static instance. There is no need to handle more than one instance. From
    /// this point you can create your application.
    Application *CreateApplication(string name = "Elys3D");
} // namespace Core

#endif // ELYS_APPLICATION_HPP
