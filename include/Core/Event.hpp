//
// Created by Sam on 2/16/2022.
//

#ifndef ELYS_UTILS_EVENT_HPP
#define ELYS_UTILS_EVENT_HPP

#include <functional>
#include <iostream>
#include <sstream>

#include <Core/Base.hpp>

namespace Elys {
    using KeyCode = uint16_t;

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput       = BIT(1),
        EventCategoryKeyboard    = BIT(2),
        EventCategoryMouse       = BIT(3),
        EventCategoryMouseButton = BIT(4),
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
                               virtual EventType GetEventType() const override { return GetStaticType(); } \
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) int GetCategoryFlags() const override { return category; }

    class Event {
        friend class EventDispatcher;
      public:
        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category) {
             return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
      public:
        EventDispatcher(Event &event) : mEvent{event} {}

        template<typename T>
        bool Dispatch(EventFn<T> func) {
            if (mEvent.GetEventType() == T::GetStaticType()) {
                mEvent.Handled |= func(*(T*)&mEvent);
                return true;
            }

            return false;
        }

      private:
        Event& mEvent;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event &e) {
        return os << e.ToString();
    }

    // --- APPLICATION EVENT ---

    class WindowResizeEvent : public Event {
      private:
        unsigned int mWidth, mHeight;

      public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : mWidth(width), mHeight(height) {}

        unsigned int GetWidth() const { return mWidth; }
        unsigned int GetHeight() const { return mHeight; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
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

    // --- KEY EVENT ---

    class KeyEvent : public Event {
      public:
        KeyCode GetKeyCode() const { return mKeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
      protected:
        KeyEvent(const KeyCode keycode) : mKeyCode(keycode) {}

        KeyCode mKeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
      public:
        KeyPressedEvent(const KeyCode keycode, const uint16_t repeatCount)
            : KeyEvent(keycode), mRepeatCount(repeatCount) {}

        uint16_t GetRepeatCount() const { return mRepeatCount; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
      private:
        uint16_t mRepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent {
      public:
        KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
      public:
        KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << mKeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };

    // --- MOUSE EVENT ---

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

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
} // namespace Elys

#endif // ELYS_UTILS_EVENT_HPP
