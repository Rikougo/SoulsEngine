//
// Created by Sam on 2/18/2022.
//

#ifndef ELYS_APPLICATION_EVENT_HPP
#define ELYS_APPLICATION_EVENT_HPP

#include <Events/Event.hpp>

namespace Elys {
    class WindowResizeEvent : public Event {
      private:
        unsigned int m_Width, m_Height;

      public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) {}

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowCloseEvent : public Event {
      public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent : public Event {
      public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event {
      public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public Event {
      public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
} // namespace Elys

#endif // ELYS_APPLICATION_EVENT_HPP
