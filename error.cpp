#include <error.h>

void CheckForRuntimeError(bool value) {
    if (value) {
        throw std::runtime_error("runtime error");
    }
}

void CheckForSyntaxError(bool value) {
    if (value) {
        throw SyntaxError("syntax error");
    }
}

void CheckForLogicError(bool value) {
    if (value) {
        throw std::logic_error("name_error");
    }
}