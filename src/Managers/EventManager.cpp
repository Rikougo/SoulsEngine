//
// Created by Sam on 12/10/2021.
//

#include "Managers/EventManager.hpp"

void Core::EventManager::AddListener(Core::EventId eventId, EventCallback const &listener) {
    listeners[eventId].push_back(listener);
}

void Core::EventManager::Dispatch(Core::Event &event) {
    EventId type = event.GetType();

    for (auto const &listener : listeners[type]) {
        listener(event);
    }
}

void Core::EventManager::Dispatch(Core::EventId eventId) {
    Event event{eventId};

    for (auto const &listener : listeners[eventId]) {
        listener(event);
    }
}
