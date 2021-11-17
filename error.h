#pragma once

#include <stdexcept>

struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

void CheckForRuntimeError(bool value);

void CheckForSyntaxError(bool value);

void CheckForLogicError(bool value);