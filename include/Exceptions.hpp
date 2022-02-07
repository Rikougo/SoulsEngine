//
// Created by Sam on 2/5/2022.
//

#ifndef ELYS_EXCEPTIONS_HPP
#define ELYS_EXCEPTIONS_HPP

#include <exception>

struct WrongEventParamType : public std::exception {

    WrongEventParamType() = default;

    [[nodiscard]] const char* what() const noexcept override {
        return "Wrong event param type specified.";
    }
};

#endif // ELYS_EXCEPTIONS_HPP
