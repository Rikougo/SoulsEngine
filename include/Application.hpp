//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <Managers/EventManager.hpp>

#include <functional>
#include <memory>

using namespace std;

namespace Core {
class Application {
  private:
    unique_ptr<EventManager> mEventManager;
  public:
    Application();

    void AddEventListener(EventId eventId, std::function<void(Event&)> const &listener);
    void DispatchEvent(EventId eventId);
    void DispatchEvent(Event &event);
};
} // namespace Core

#endif // ELYS_APPLICATION_HPP
