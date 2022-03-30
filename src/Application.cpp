//
// Created by Sam on 12/10/2021.
//

#include "Application.hpp"

namespace Elys {
    Application *Application::sInstance = nullptr;

    Application::Application(const std::string &name) {
        ELYS_CORE_INFO("[Running app on C++ : {0}]", __cplusplus);

        if (sInstance) {
            ELYS_CORE_FATAL("Application instance already exists.");
            exit(-1);
        }

        sInstance = this;

        mWindow = Window::Create();
        mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        mImGUILayer = new ImGuiLayer();
        mLayerStack.PushOverlay(mImGUILayer);
        mLayerStack.PushLayer(new EditorLayer());
    }

    Application::~Application() {
        delete mImGUILayer;
    }

    void Application::Run() {
        while (mRunning) {
            auto time = (float)glfwGetTime();
            float deltaTime = time - mLastFrameTime;

            Profile::Framerate = 1.0f / deltaTime;
            Profile::AverageFramerate = (Profile::AverageFramerate == 0) ? Profile::Framerate : (Profile::Framerate + Profile::AverageFramerate) * 0.5f;
            Profile::DeltaTime = deltaTime;

            mLastFrameTime = time;

            if (!mMinimized) {
                time = (float)glfwGetTime();
                for (Layer *layer : mLayerStack) {
                    layer->OnUpdate(deltaTime);
                }
                Profile::DrawingTime = (float)glfwGetTime() - time;

                time = (float)glfwGetTime();
                mImGUILayer->Begin();
                for (Layer *layer : mLayerStack) {
                    layer->OnImGuiRender();
                }
                mImGUILayer->End();
                Profile::GUITime = (float)glfwGetTime() - time;
            }
            mWindow->OnUpdate();
        }
    }

    void Application::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = mLayerStack.rbegin(); it != mLayerStack.rend(); ++it) {
            if (event.Handled)
                break;
            (*it)->OnEvent(event);
        }
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

    Application *CreateApplication() { return new Application(); }
} // namespace Elys
