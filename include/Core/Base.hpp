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

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
    #define M_PI_2 M_PI / 2
#endif

#ifndef M_2_PI
    #define M_2_PI M_PI * 2
#endif

namespace Elys {
    using EntityID = std::uint32_t;
    constexpr EntityID MAX_ENTITIES = 10'000;

    using ComponentType = std::uint8_t;
    constexpr ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

    template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    // explicit deduction guide (not needed as of C++20)
    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

#endif // ELYS_BASE_HPP
