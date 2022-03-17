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

        ImGuiLayer *tLayer = new ImGuiLayer();
        DebugLayer *debugLayer = new DebugLayer();
        ECSLayer *ecsLayer = new ECSLayer();

        mLayerStack.PushOverlay(tLayer);
        mLayerStack.PushOverlay(debugLayer);
        mLayerStack.PushLayer(ecsLayer);
    }

    Application::~Application() {}

    void Application::Run() {
        while (mRunning) {
            float time = (float)glfwGetTime();
            float deltaTime = time - mLastFrameTime;
            mLastFrameTime = time;

            if (!mMinimized) {
                for (Layer *layer : mLayerStack) {
                    layer->OnUpdate(deltaTime);
                }

                mImGUILayer->Begin();
                for (Layer *layer : mLayerStack) {
                    layer->OnImGuiRender();
                }
                mImGUILayer->End();
            }
            mWindow->OnUpdate();
        }
    }

    void Application::OnEvent(Event &event) {
        // ELYS_CORE_INFO("{0}", event.ToString());

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
