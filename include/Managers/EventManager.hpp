//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_MANAGERS_EVENT_MANAGER_HPP
#define ELYS_MANAGERS_EVENT_MANAGER_HPP

#include "Utils.hpp"

#include <functional>
#include <unordered_map>
#include <list>


namespace Core {
using EventCallback = std::function<void(Event&)>;
class EventManager {
  public:
    void AddListener(EventId eventId, EventCallback const &listener);

    void Dispatch(EventId eventId);
    void Dispatch(Event &event);

  private:
    std::unordered_map<EventId, std::list<EventCallback>> listeners;
};
} // namespace Core

#endif // ELYS_MANAGERS_EVENT_MANAGER_HPP
