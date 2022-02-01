//
// Created by Sam on 12/10/2021.
//

#include "Managers/WindowManager.hpp"

Core::WindowManager::WindowManager(shared_ptr<Application> &app,
                                   string const &title, string const &icon,
                                   int width, int height) {
    mApplication = app;

    glfwInit();

    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    GLFWimage icons[1];
    icons[0].pixels = stbi_load("assets/icons/Icon.png", &icons[0].width,
                                &icons[0].height, 0, 4);
    glfwSetWindowIcon(mWindow, 1, icons);
    stbi_image_free(icons[0].pixels);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwMakeContextCurrent(mWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glfwSwapInterval(2);

    glfwSetWindowUserPointer(mWindow, this);

    glfwSetWindowSizeCallback(
        mWindow, [](GLFWwindow *window, int width, int height) {
            auto *wm =
                static_cast<WindowManager *>(glfwGetWindowUserPointer(window));

            if (wm) {
                shared_ptr<Application> app = wm->mApplication;

                Event event{Events::Window::RESIZE};
                event.SetParam(EventsParams::RESIZE_WIDTH, width);
                event.SetParam(EventsParams::RESIZE_HEIGHT, height);
                app->DispatchEvent(event);

                glViewport(0, 0, width, height);
            } else {
                throw std::runtime_error(
                    "Core::WindowManager::ResizeCallbackError -> Can't resolve "
                    "window manager from callback.");
            }
        });

    // TODO Add anti-aliasing
    /*glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);*/

    /*glClearColor(0.0f, 0.0f, 0.0f, 1.0f);*/
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    mInitialized = true;
}
void Core::WindowManager::Update() { glfwSwapBuffers(mWindow); }

void Core::WindowManager::ProcessEvents() { 
    glfwPollEvents(); 

    if (glfwGetKey(mWindow, GLFW_KEY_F) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_Z) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetKey(mWindow, GLFW_KEY_C) == GLFW_PRESS) {
        Event event{Core::Events::Game::SWITCH_MESH};
        event.SetParam(Core::EventsParams::MESH_TYPE, 0);
        mApplication->DispatchEvent(event);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS) {
        Event event{Core::Events::Game::SWITCH_MESH};
        event.SetParam(Core::EventsParams::MESH_TYPE, 1);
        mApplication->DispatchEvent(event);
    }
}
void Core::WindowManager::Shutdown() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool Core::WindowManager::ShouldClose() {
    return glfwWindowShouldClose(mWindow);
}
