//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <functional>
#include <memory>

#include <Core/Window.hpp>
#include <Core/Base.hpp>
#include <Core/LayerStack.hpp>

#include <Events/Event.hpp>
#include <Events/ApplicationEvent.hpp>

#include <Layers/ImGuiLayer.hpp>
#include <Layers/DebugLayer.hpp>
#include <Layers/ECSLayer.hpp>

using namespace std;

int main(int argc, char* argv[]);

namespace Elys {
class Application {
  private:
    unique_ptr<Window> mWindow;
    bool mRunning = true;
    bool mMinimized = false;
    float mLastFrameTime = 0.0f;
    ImGuiLayer* mImGUILayer;
    LayerStack mLayerStack;
  public:
    Application(const std::string &name="Elys App");
    ~Application();

    static Application& Get() { return *sInstance; }
    Window& GetWindow() { return *mWindow; }

    void OnEvent(Event &event);
  private:
    void Run();
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);
  private:
    static Application* sInstance;
    friend int ::main(int argc, char* argv[]);
};

Application* CreateApplication();
} // namespace Core

#endif // ELYS_APPLICATION_HPP
