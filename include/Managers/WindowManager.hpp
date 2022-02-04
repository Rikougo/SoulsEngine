//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_WINDOW_MANAGER_HPP
#define ELYS_WINDOW_MANAGER_HPP

#include <Application.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>

#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

namespace Core {
class WindowManager {
  private:
    GLFWwindow *mWindow;
    shared_ptr<Application> mApplication;
    bool mInitialized = false;

  public:
    explicit WindowManager(shared_ptr<Application> &app,
                           string const &title = "Window", string const &icon = "assets/icons/Icon.png", int width = 1280,
                           int height = 720);
    void Update();
    void ProcessEvents();
    void Shutdown();

    bool ShouldClose();
};
} // namespace Core

#endif // ELYS_WINDOW_MANAGER_HPP
