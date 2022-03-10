//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <functional>
#include <memory>

#include <Core/Window.hpp>
#include <Core/Base.hpp>

#include <Events/Event.hpp>
#include <Events/ApplicationEvent.hpp>

using namespace std;

namespace Elys {
class Application {
  private:
    unique_ptr<Window> mWindow;
    bool mRunning = true;
    bool mMinimized = false;
    float mLastFrameTime = 0.0f;
  public:
    Application(const std::string &name="Elys App");
    ~Application();

    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);

    void Run();

    void OnEvent(Event &event);
};
} // namespace Core

#endif // ELYS_APPLICATION_HPP
