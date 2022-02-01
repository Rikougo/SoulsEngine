//
// Created by Sam on 12/10/2021.
//

#include "Application.hpp"

Core::Application::Application() {
    mEventManager = make_unique<EventManager>();
}

void Core::Application::AddEventListener(Core::EventId eventId, function<void(Event &)> const &listener) {
    mEventManager->AddListener(eventId, listener);
}

void Core::Application::DispatchEvent(Core::EventId eventId) {
    mEventManager->Dispatch(eventId);
}

void Core::Application::DispatchEvent(Core::Event &event) {
    mEventManager->Dispatch(event);
}
