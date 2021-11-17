#include "scheme_test.h"

void SchemeTest::ExpectEq(std::string expression, std::string result) {
    std::string answer = Proceed(scope, expression);
    REQUIRE(answer == result);
}

void SchemeTest::ExpectNoError(std::string expression) {
    REQUIRE_NOTHROW(Proceed(scope, expression));
}

void SchemeTest::ExpectSyntaxError(std::string expression) {
    REQUIRE_THROWS_AS(Proceed(scope, expression), SyntaxError);
}

void SchemeTest::ExpectRuntimeError(std::string expression) {
    REQUIRE_THROWS_AS(Proceed(scope, expression), std::runtime_error);
}

void SchemeTest::ExpectNameError(std::string expression) {
    REQUIRE_THROWS_AS(Proceed(scope, expression), std::logic_error);
}