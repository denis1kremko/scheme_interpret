#pragma once

#include <object.h>
#include <scope.h>

std::shared_ptr<Bool> GetBoolPtr(bool value);

// переводим в bool
std::shared_ptr<Bool> MakeBool(const std::shared_ptr<Object> &obj);

class Function : public Object {
public:
    virtual std::shared_ptr<Object> Compute(
        Scope &scope, const std::vector<std::shared_ptr<Object>> &objects) = 0;
};

// арифметика
class SimpleFunction : public Function {
public:
    explicit SimpleFunction(std::string sign) : sign_(std::move(sign)) {
    }

    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;

private:
    const std::string sign_;
};

// булевы функции
enum class Type { Integer, Boolean, Symbol, Pair, List, Null };

class IsFunction : public Function {
public:
    explicit IsFunction(Type type) : type_(type) {
    }

    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;

private:
    Type type_;
};
class NotFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};
class AndFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};
class OrFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

// уравнения и неравенства
class Comparison : public Function {
public:
    explicit Comparison(std::string symbol) : symbol_(std::move(symbol)) {
    }

    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;

private:
    const std::string symbol_;
};

// максимум, минимум и модуль
class MaxFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};
class MinFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};
class AbsFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class IfFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class QuoteFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class DefineFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class SetFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class SetCarFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class SetCdrFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class RawLambdaFunction : public Function {
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class LambdaFunction : public Function {
public:
    LambdaFunction(const Scope &scope) : outer_scope_(scope) {
    }

    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;

    void Init(const std::vector<std::shared_ptr<Object>> &objects);

    void MergeScopes();

private:
    std::vector<std::string> lambda_variables_;
    std::vector<std::shared_ptr<Object>> lambda_parts_;
    Scope local_scope_;
    const Scope &outer_scope_;
};

class ListFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class ListRefFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class ListTailFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class ConsFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class CarFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};

class CdrFunction : public Function {
public:
    std::shared_ptr<Object> Compute(Scope &scope,
                                    const std::vector<std::shared_ptr<Object>> &objects) override;
};