//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_BASE_HPP
#define ELYS_BASE_HPP

#include <bitset>

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn)                                                                          \
    [this](auto &&...args) -> decltype(auto) {                                                     \
        return this->fn(std::forward<decltype(args)>(args)...);                                    \
    }

using EntityID = std::uint32_t;
constexpr EntityID MAX_ENTITIES = 5000;

using ComponentType = std::uint8_t;
constexpr ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

#endif // ELYS_BASE_HPP
