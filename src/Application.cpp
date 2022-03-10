//
// Created by Sam on 12/10/2021.
//

#include "Application.hpp"

using namespace Elys;

Application::Application(const std::string &name) {
    mWindow = Window::Create();
    mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}

Application::~Application() { }

void Application::Run() {
    while(mRunning) {
        float time = (float)glfwGetTime();
        float deltaTime = time - mLastFrameTime;
        mLastFrameTime = time;

        if (!mMinimized) {
            // serious shit here
        }
        mWindow->OnUpdate();
    }
}

void Application::OnEvent(Event& event) {
    ELYS_CORE_INFO("{0}", event.ToString());

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
}
bool Application::OnWindowClose(WindowCloseEvent &e) {
    mRunning = false;
    return true;
}
bool Application::OnWindowResize(WindowResizeEvent &e) {
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        mMinimized = true;
        return false;
    }

   mMinimized = false;

    return false;
}
