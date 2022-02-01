//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_UTILS_HPP
#define ELYS_UTILS_HPP

#include <cstdint>
#include <cstdlib>

#include <any>
#include <unordered_map>


namespace Core {

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

    template <typename T> void SetParam(ParamId id, T value) {
        mData[id] = value;
    }

    template <typename T> T GetParam(ParamId id) {
        return std::any_cast<T>(mData[id]);
    }

    [[nodiscard]] EventId GetType() const { return mType; }
};

namespace Events {
namespace Game {
constexpr EventId SWITCH_MESH = "Events::Window::RESIZE"_hash;
}

namespace Window {
constexpr EventId QUIT = "Events::Window::QUIT"_hash;
constexpr EventId RESIZE = "Events::Window::RESIZE"_hash;
} // namespace Window
} // namespace Events

namespace EventsParams {
constexpr ParamId RESIZE_WIDTH = "Events::Window::RESIZE_WIDTH"_hash;
constexpr ParamId RESIZE_HEIGHT = "Events::Window::RESIZE_HEIGHT"_hash;
constexpr ParamId MESH_TYPE = "Events::Window::MESH_TYPE"_hash;
} // namespace EventsParams
} // namespace Core

#endif // ELYS_UTILS_HPP
