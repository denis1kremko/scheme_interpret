#pragma once

#include <object.h>
#include <parser.h>
#include <functions.h>
#include <scope.h>

std::string ObjectToString(const std::shared_ptr<Object> &obj);

void LambdaSugarCase(Scope &scope, const std::vector<std::shared_ptr<Object>> &args);

std::shared_ptr<Object> Eval(Scope &scope, const std::shared_ptr<Object> &obj);

std::string Proceed(Scope &scope, const std::string &code);