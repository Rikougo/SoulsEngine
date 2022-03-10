//
// Created by Sam on 3/10/2022.
//

#ifndef ELYS_BASE_HPP
#define ELYS_BASE_HPP

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#endif // ELYS_BASE_HPP
