//
// Created by Sam on 12/10/2021.
//

#include "Application.hpp"

using namespace Core;

Application::Application() {
    mEventManager = make_unique<EventManager>();
    mComponentManager = make_unique<ComponentManager>();
    mEntityManager = make_unique<EntityManager>();
    mSystemManager = make_unique<SystemManager>();
}

// --- EVENTS ---

void Application::AddEventListener(
    EventId eventId, function<void(Event &)> const &listener) {
    mEventManager->AddListener(eventId, listener);
}

void Application::DispatchEvent(EventId eventId) {
    mEventManager->Dispatch(eventId);
}

void Application::DispatchEvent(Event &event) {
    mEventManager->Dispatch(event);
}