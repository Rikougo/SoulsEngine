//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_UTILS_HPP
#define ELYS_UTILS_HPP

#include <cstdint>
#include <cstdlib>

#include <any>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <glm/glm.hpp>

namespace Core {
    struct KeyInfo {
        int key;
        int scancode;
        int action;
        int mods;
    };

    // ECS
    using EntityID = std::uint32_t;
    constexpr EntityID MAX_ENTITIES = 5000;

    using ComponentType = std::uint8_t;
    constexpr ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

    // Source: https://gist.github.com/Lee-R/3839813
    constexpr std::uint32_t fnv1a_32(char const *s, std::size_t count) {
        return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) *
               16777619u; // NOLINT (hicpp-signed-bitwise)
    }

    constexpr std::uint32_t operator"" _hash(char const *s, std::size_t count) {
        return fnv1a_32(s, count);
    }

    using EventId = std::uint32_t;
    using ParamId = std::uint32_t;

    class Event {
      private:
        EventId mType{};
        std::unordered_map<EventId, std::any> mData{};

      public:
        Event() = delete;

        explicit Event(EventId type) : mType{type} {}

        template <typename T> void SetParam(ParamId id, T value) { mData[id] = value; }

        // TODO handle any cast error
        template <typename T> T GetParam(ParamId id) {
            try {
                return std::any_cast<T>(mData[id]);
            } catch (std::bad_any_cast &e) {
                std::cerr << "Core::Event::GetParam<T> : Wrong event param "
                             "type specified."
                          << " In " << mType << std::endl;
                std::cerr << e.what() << std::endl;

                return T{};
            }
        }

        [[nodiscard]] EventId GetType() const { return mType; }
    };

    class Time {
      public:
        static float DeltaTime;
    };

    namespace Events {
        namespace Game {} // namespace Game

        namespace Window {
            constexpr EventId QUIT = "Events::Window::QUIT"_hash;
            constexpr EventId RESIZE = "Events::Window::RESIZE"_hash;
            constexpr EventId UPDATE = "Events::Window::UPDATE"_hash;
            constexpr EventId KEY_PRESSED = "Events::Window::KEY_PRESSED"_hash;
            constexpr EventId MOUSE_MOVE = "Events::Window::MOUSE_MOVE"_hash;

            namespace Params {
                constexpr ParamId NEW_SIZE = "Events::Window::Params::NEW_SIZE"_hash;
                constexpr ParamId KEY_INFO = "Events::Window::Params::KEY_CODE"_hash;
                constexpr ParamId MOUSE_POS = "Events::Window::Params::MOUSE_POS"_hash;
            } // namespace Params
        }     // namespace Window
    }         // namespace Events
} // namespace Core

#endif // ELYS_UTILS_HPP
