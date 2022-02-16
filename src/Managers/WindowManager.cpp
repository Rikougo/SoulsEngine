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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

                glm::ivec2 size = {width, height};

                Event event{Events::Window::RESIZE};
                event.SetParam(Events::Window::Params::NEW_SIZE, size);
                app->DispatchEvent(event);

                glViewport(0, 0, width, height);
            } else {
                throw std::runtime_error(
                    "Core::WindowManager::ResizeCallbackError -> Can't resolve "
                    "window manager from callback.");
            }
        });

    glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto *wm = static_cast<WindowManager *>(glfwGetWindowUserPointer(window));

        if (wm) {
            auto app = wm->mApplication;

            auto keyInfo = Core::KeyInfo{
                .key = key,
                .scancode = scancode,
                .action = action,
                .mods = mods
            };

            Event event{Events::Window::KEY_PRESSED};
            event.SetParam(Events::Window::Params::KEY_INFO, keyInfo);
            app->DispatchEvent(event);
        } else {
            throw std::runtime_error("Core::WindowManager::KeyCallback -> Can't resolve window manager from callback.");
        }
    });

    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
        auto *wm = static_cast<WindowManager *>(glfwGetWindowUserPointer(window));

        if (wm) {
            auto app = wm->mApplication;

            auto mousePos = glm::vec2{xPos, yPos};

            Event event{Events::Window::MOUSE_MOVE};
            event.SetParam(Events::Window::Params::MOUSE_POS, mousePos);
            app->DispatchEvent(event);
        } else {
            throw std::runtime_error("Core::WindowManager::CursorPosCallback -> Can't resolve window manager from callback.");
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

void Core::WindowManager::ProcessEvents() { glfwPollEvents(); }

void Core::WindowManager::Shutdown() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool Core::WindowManager::ShouldClose() {
    return glfwWindowShouldClose(mWindow);
}
