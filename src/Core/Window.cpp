//
// Created by Sam on 3/10/2022.
//

#include "Core/Window.hpp"

namespace Elys {
    std::unique_ptr<Window> Window::Create(const WindowData &data) {
        return std::make_unique<Window>(data);
    }

    Window::Window(const WindowData &data) {
        mData.Width  = data.Width;
        mData.Height = data.Height;
        mData.Title  = data.Title;

        if(glfwInit() == GLFW_FALSE) {
            ELYS_CORE_FATAL("Failed to initialize GLFW.");
            exit(-1);
        }

        mWindow = glfwCreateWindow(mData.Width, mData.Height, mData.Title.c_str(), nullptr, nullptr);

        GLFWimage icons[1];
        icons[0].pixels = stbi_load("assets/icons/Icon.png", &icons[0].width,
                                    &icons[0].height, nullptr, 4);
        glfwSetWindowIcon(mWindow, 1, icons);
        stbi_image_free(icons[0].pixels);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // creating context
        glfwMakeContextCurrent(mWindow);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            ELYS_CORE_FATAL("Failed to initialize GLAD.");
            exit(-1);
        }
        glfwSwapInterval(1);

        ELYS_CORE_INFO("OpenGL specs :");
        ELYS_CORE_INFO("  - Vendor : {0}", glGetString(GL_VENDOR));
        ELYS_CORE_INFO("  - Renderer : {0}", glGetString(GL_RENDERER));
        ELYS_CORE_INFO("  - Version : {0}", glGetString(GL_VERSION));

        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSetWindowUserPointer(mWindow, &mData);

        glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height){
            WindowData &wData = *(WindowData*) glfwGetWindowUserPointer(window);

            WindowResizeEvent event(width, height);
            wData.EventCallback(event);
        });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window){
	    WindowData& wData = *(WindowData*)glfwGetWindowUserPointer(window);
	    WindowCloseEvent event;
	    wData.EventCallback(event);
        });

        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData &wData = *(WindowData*) glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    wData.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    wData.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    wData.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
	{
	    WindowData& wData = *(WindowData*)glfwGetWindowUserPointer(window);

	    KeyTypedEvent event(keycode);
	    wData.EventCallback(event);
        });

        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods){
             WindowData &wData = *(WindowData*) glfwGetWindowUserPointer(window);

             switch (action) {
                 case GLFW_PRESS:
                 {
                     MouseButtonPressedEvent event(button);
                     wData.EventCallback(event);
                     break;
                 }
                 case GLFW_RELEASE:
                 {
                     MouseButtonReleasedEvent event(button);
                     wData.EventCallback(event);
                     break;
                 }
             };
        });

        glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
            WindowData &wData = *(WindowData*) glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float(xoffset)), float(yoffset));
            wData.EventCallback(event);
        });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xpos, double ypos) {
             WindowData &wData = *(WindowData*) glfwGetWindowUserPointer(window);

             MouseMovedEvent event((float(xpos)), float(ypos));
             wData.EventCallback(event);
        });
    }
    Window::~Window() {Shutdown();}

    void Window::Shutdown() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void Window::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(mWindow);
    }

    uint32_t Window::GetWidth() const { return mData.Width; }
    uint32_t Window::GetHeight() const { return mData.Height; }

    void Window::SetEventCallback(const Window::EventCallbackFn &callback) { mData.EventCallback = callback; }
} // namespace Elys
