//
// Created by Sam on 2/16/2022.
//

#ifndef ELYS_UTILS_EVENT_HPP
#define ELYS_UTILS_EVENT_HPP

#include <functional>

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
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category) {
             return GetCategoryFlags() & category;
        }
      protected:
        bool mHandled = false;
    };

    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
      public:
        EventDispatcher(Event &event) : mEvent{event} {}

        template<typename T>
        bool Dispatch(EventFn<T> func) {
            if (mEvent.GetEventType() == T::GetStaticType()) {
                mEvent.mHandled = func(*(T*)&mEvent);
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
} // namespace Elys


#endif // ELYS_UTILS_EVENT_HPP
