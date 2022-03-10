//
// Created by Sam on 2/18/2022.
//

#ifndef ELYS_MOUSE_EVENT_HPP
#define ELYS_MOUSE_EVENT_HPP

#include "Events/Event.hpp"

namespace Elys {
    class MouseMovedEvent : public Event {
      public:
        MouseMovedEvent(float x, float y) : mMouseX{x}, mMouseY{y} {}

        inline float GetX() const { return mMouseX; }
        inline float GetY() const { return mMouseY; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
      private:
        float mMouseX, mMouseY;
    };

    class MouseScrolledEvent : public Event {
      public:
        MouseScrolledEvent(float x, float y) : mXOffset{x}, mYOffset{y} {}

        inline float GetXOffset() const { return mXOffset; }
        inline float GetYOffset() const { return mYOffset; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << mXOffset << ", " << mYOffset;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
      private:
        float mXOffset, mYOffset;
    };

    class MouseButtonEvent : public Event {
      public:
        inline int GetMouseButton() const { return mButton; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

      protected:
        MouseButtonEvent(int button) : mButton{button} {}
        int mButton;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
      public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << mButton;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
      public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << mButton;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };
} // namespace Elys

#endif // ELYS_MOUSE_EVENT_HPP
