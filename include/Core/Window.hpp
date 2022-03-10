//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_WINDOW_HPP
#define ELYS_WINDOW_HPP

#include <functional>
#include <memory>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <Events/ApplicationEvent.hpp>
#include <Events/Event.hpp>
#include <Events/KeyEvent.hpp>
#include <Events/MouseEvent.hpp>

#include <Core/Logger.hpp>

namespace Elys {
    class Window {
      public:
        using EventCallbackFn = std::function<void(Event &)>;

        struct WindowData {
            std::string Title;
            uint32_t Width;
            uint32_t Height;

            EventCallbackFn EventCallback;

            WindowData(const std::string &title = "Elys Engine", uint32_t width = 1280,
                       uint32_t height = 720)
                : Title(title), Width(width), Height(height) {}
        };

        explicit Window(const WindowData &data);
        ~Window();

        void OnUpdate();

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        void SetEventCallback(const EventCallbackFn &callback);

        static std::unique_ptr<Window> Create(const WindowData &data = WindowData());

      private:
        void Shutdown();

        GLFWwindow *mWindow;
        WindowData mData;
    };
} // namespace Elys

#endif // ELYS_WINDOW_HPP
