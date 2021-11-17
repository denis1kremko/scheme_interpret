#include <functions.h>
#include <scheme.h>
#include <error.h>

std::shared_ptr<Bool> GetBoolPtr(bool value) {
    return std::make_shared<Bool>(value);
}

std::shared_ptr<Bool> MakeBool(const std::shared_ptr<Object> &obj) {
    if (auto ptr_bool = std::dynamic_pointer_cast<Bool>(obj)) {
        return ptr_bool;
    }

    return GetBoolPtr(true);
}

std::shared_ptr<Object> SimpleFunction::Compute(
    Scope &, const std::vector<std::shared_ptr<Object>> &objects) {
    int answer = 1;

    if (sign_ == "+") {
        answer = 0;
        for (const auto &obj : objects) {
            CheckForRuntimeError(dynamic_cast<Number *>(obj.get()) == nullptr);
            answer += As<Number>(obj)->GetValue();
        }
    }

    if (sign_ == "-") {
        CheckForRuntimeError(objects.empty());
        CheckForRuntimeError(dynamic_cast<Number *>(objects[0].get()) == nullptr);

        answer = As<Number>(objects[0])->GetValue();
        for (size_t iter = 1; iter < objects.size(); ++iter) {
            CheckForRuntimeError(dynamic_cast<Number *>(objects[iter].get()) == nullptr);
            answer -= As<Number>(objects[iter])->GetValue();
        }
    }

    if (sign_ == "*") {

        answer = 1;
        for (const auto &obj : objects) {
            CheckForRuntimeError(dynamic_cast<Number *>(obj.get()) == nullptr);
            answer *= As<Number>(obj)->GetValue();
        }
    }

    if (sign_ == "/") {
        CheckForRuntimeError(objects.empty());
        CheckForRuntimeError(dynamic_cast<Number *>(objects[0].get()) == nullptr);

        answer = As<Number>(objects[0])->GetValue();
        for (size_t iter = 1; iter < objects.size(); ++iter) {
            CheckForRuntimeError(dynamic_cast<Number *>(objects[iter].get()) == nullptr);
            answer /= As<Number>(objects[iter])->GetValue();
        }
    }

    return std::make_shared<Number>(answer);
}

std::shared_ptr<Object> IsFunction::Compute(Scope &,
                                            const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForSyntaxError(objects.size() != 1);

    auto obj = objects[0];

    if (type_ == Type::Boolean) {
        return GetBoolPtr(Is<Bool>(obj));
    }

    if (type_ == Type::Integer) {
        return GetBoolPtr(Is<Number>(obj));
    }

    if (type_ == Type::Symbol) {
        if (Is<Quoted>(obj) && Is<Symbol>(As<Quoted>(obj)->GetQuotedObj())) {
            return GetBoolPtr(true);
        }

        return GetBoolPtr(false);
    }

    if (type_ == Type::List) {
        if (!Is<Quoted>(obj)) {
            return GetBoolPtr(false);
        }

        auto quoted_obj = As<Quoted>(obj)->GetQuotedObj();
        // пустой список
        if (quoted_obj == nullptr) {
            return GetBoolPtr(true);
        }

        if (Is<Cell>(quoted_obj)) {
            auto next_cell = As<Cell>(quoted_obj)->GetSecond();
            while (next_cell) {
                if (!Is<Cell>(next_cell)) {
                    return GetBoolPtr(false);
                }

                next_cell = As<Cell>(next_cell)->GetSecond();
            }
            return GetBoolPtr(true);
        }

        GetBoolPtr(false);
    }

    if (type_ == Type::Null) {
        if (Is<Quoted>(obj) && As<Quoted>(obj)->GetQuotedObj() == nullptr) {
            return GetBoolPtr(true);
        }

        return GetBoolPtr(false);
    }

    if (type_ == Type::Pair) {
        if (!Is<Quoted>(obj)) {
            return GetBoolPtr(false);
        }

        auto quoted_obj = As<Quoted>(obj)->GetQuotedObj();
        if (Is<Cell>(quoted_obj) && As<Cell>(quoted_obj)->GetSecond()) {

            if (Is<Cell>(As<Cell>(quoted_obj)->GetSecond())) {

                auto next_cell = As<Cell>(As<Cell>(quoted_obj)->GetSecond());
                if (next_cell->GetSecond() == nullptr) {
                    return GetBoolPtr(true);
                }

            } else {
                return GetBoolPtr(true);
            }
        }

        return GetBoolPtr(false);
    }

    throw std::runtime_error("runtime error");
}

std::shared_ptr<Object> NotFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.size() != 1);

    auto obj = MakeBool(objects[0]);
    if (obj->GetValue()) {
        return GetBoolPtr(false);
    } else {
        return GetBoolPtr(true);
    }
}

std::shared_ptr<Object> Comparison::Compute(Scope &,
                                            const std::vector<std::shared_ptr<Object>> &objects) {
    if (objects.empty()) {
        return GetBoolPtr(true);
    }

    for (size_t iter = 0; iter < objects.size() - 1; ++iter) {
        auto ptr_first_num = As<Number>(objects[iter]);
        auto ptr_second_num = As<Number>(objects[iter + 1]);

        CheckForRuntimeError(ptr_second_num == nullptr || ptr_first_num == nullptr);

        if ((symbol_ == "=" && (ptr_first_num->GetValue() != ptr_second_num->GetValue())) ||
            (symbol_ == "<" && (ptr_first_num->GetValue() >= ptr_second_num->GetValue())) ||
            (symbol_ == ">" && (ptr_first_num->GetValue() <= ptr_second_num->GetValue())) ||
            (symbol_ == "<=" && (ptr_first_num->GetValue() > ptr_second_num->GetValue())) ||
            (symbol_ == ">=" && (ptr_first_num->GetValue() < ptr_second_num->GetValue()))) {
            return GetBoolPtr(false);
        }
    }

    return GetBoolPtr(true);
}

std::shared_ptr<Object> MaxFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.empty());

    int answer = (-1) * INT32_MAX;
    for (const auto &obj : objects) {
        CheckForRuntimeError(!Is<Number>(obj));
        answer = (answer < As<Number>(obj)->GetValue()) ? As<Number>(obj)->GetValue() : answer;
    }

    return std::make_shared<Number>(answer);
}

std::shared_ptr<Object> MinFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.empty());

    int answer = INT32_MAX;
    for (const auto &obj : objects) {
        CheckForRuntimeError(!Is<Number>(obj));
        answer = (answer > As<Number>(obj)->GetValue()) ? As<Number>(obj)->GetValue() : answer;
    }

    return std::make_shared<Number>(answer);
}

std::shared_ptr<Object> AbsFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.empty() || objects.size() > 1);
    CheckForRuntimeError(!Is<Number>(objects[0]));

    auto ptr_num = As<Number>(objects[0]);
    return std::make_shared<Number>(std::abs(ptr_num->GetValue()));
}

std::shared_ptr<Object> QuoteFunction::Compute(
    Scope &, const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.size() != 1);

    return std::make_shared<Quoted>(objects[0]);
}

std::shared_ptr<Object> DefineFunction::Compute(Scope &,
                                                const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("define doesnt compute");
}

std::shared_ptr<Object> SetFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("Set doesnt compute");
}

std::shared_ptr<Object> SetCarFunction::Compute(Scope &,
                                                const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("Set doesnt compute");
}

std::shared_ptr<Object> SetCdrFunction::Compute(Scope &,
                                                const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("Set doesnt compute");
}

std::shared_ptr<Object> RawLambdaFunction::Compute(Scope &,
                                                   const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("RawLambdaFunction doesnt compute");
}

std::shared_ptr<Object> IfFunction::Compute(Scope &, const std::vector<std::shared_ptr<Object>> &) {
    throw std::runtime_error("if_func doesnt compute");
}

void LambdaFunction::Init(const std::vector<std::shared_ptr<Object>> &objects) {
    auto variables_vector = GetArgsVector(objects[0]);
    for (const auto &obj : variables_vector) {
        CheckForSyntaxError(!Is<Symbol>(obj));
        lambda_variables_.push_back(As<Symbol>(obj)->GetName());
    }

    for (size_t iter = 1; iter < objects.size(); ++iter) {
        lambda_parts_.push_back(objects[iter]);
    }
}

void LambdaFunction::MergeScopes() {
    for (const auto &pair : outer_scope_.variables_) {
        if (local_scope_.variables_.count(pair.first) == 0) {
            local_scope_.variables_[pair.first] = pair.second;
        }
    }
}

std::shared_ptr<Object> ListFunction::Compute(Scope &,
                                              const std::vector<std::shared_ptr<Object>> &objects) {
    if (objects.empty()) {
        return nullptr;
    }

    std::shared_ptr<Object> new_list = std::make_shared<Cell>();
    auto current_obj = new_list;
    for (size_t iter = 0; iter < objects.size(); ++iter) {
        auto current_cell = As<Cell>(current_obj);
        current_cell->SetFirst(objects[iter]);
        if (iter != objects.size() - 1) {
            current_cell->SetSecond(std::make_shared<Cell>());
            current_obj = current_cell->GetSecond();
        }
    }

    return new_list;
}

std::shared_ptr<Object> ListRefFunction::Compute(
    Scope &, const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.size() != 2);
    CheckForSyntaxError(!Is<Number>(objects[1]));
    CheckForSyntaxError(!Is<Quoted>(objects[0]) ||
                        !Is<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    size_t arg_num = As<Number>(objects[1])->GetValue();
    auto vector_args = GetArgsVector(As<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    CheckForRuntimeError(arg_num >= vector_args.size());

    return vector_args[arg_num];
}

std::shared_ptr<Object> ListTailFunction::Compute(
    Scope &scope, const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForRuntimeError(objects.size() != 2);
    CheckForSyntaxError(!Is<Number>(objects[1]));
    CheckForSyntaxError(!Is<Quoted>(objects[0]) ||
                        !Is<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    size_t arg_num = As<Number>(objects[1])->GetValue();
    auto vector_args = GetArgsVector(As<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    CheckForRuntimeError(arg_num > vector_args.size());
    if (arg_num == vector_args.size()) {
        return nullptr;
    }

    std::vector<std::shared_ptr<Object>> tail;
    for (size_t iter = arg_num; iter < vector_args.size(); ++iter) {
        tail.push_back(vector_args[iter]);
    }

    ListFunction list_func;

    return list_func.Compute(scope, tail);
}

std::shared_ptr<Object> ConsFunction::Compute(Scope &,
                                              const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForSyntaxError(objects.size() != 2);

    std::shared_ptr<Object> new_pair = std::make_shared<Cell>();
    As<Cell>(new_pair)->SetFirst(objects[0]);
    As<Cell>(new_pair)->SetSecond(objects[1]);

    return new_pair;
}

std::shared_ptr<Object> CarFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForSyntaxError(objects.size() != 1);
    CheckForSyntaxError(!Is<Quoted>(objects[0]) ||
                        !Is<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    auto pair_vector = GetArgsVector(As<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));
    CheckForSyntaxError(pair_vector.size() != 2);

    return pair_vector[0];
}

std::shared_ptr<Object> CdrFunction::Compute(Scope &,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForSyntaxError(objects.size() != 1);
    CheckForSyntaxError(!Is<Quoted>(objects[0]) ||
                        !Is<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));

    auto pair_vector = GetArgsVector(As<Cell>(As<Quoted>(objects[0])->GetQuotedObj()));
    CheckForSyntaxError(pair_vector.size() != 2);

    return pair_vector[1];
}

std::shared_ptr<Object> AndFunction::Compute(Scope &scope,
                                             const std::vector<std::shared_ptr<Object>> &objects) {
    if (objects.empty()) {
        return GetBoolPtr(true);
    }

    for (const auto &obj : objects) {
        auto ptr_bool = MakeBool(Eval(scope, obj));
        if (!(ptr_bool->GetValue())) {
            if (auto ptr_quoted = std::dynamic_pointer_cast<Quoted>(obj)) {
                return ptr_quoted;
            } else if (auto ptr_num = std::dynamic_pointer_cast<Number>(obj)) {
                return ptr_num;
            }

            return GetBoolPtr(false);
        }
    }

    auto last_obj = objects.back();
    if (auto ptr_quoted = std::dynamic_pointer_cast<Quoted>(last_obj)) {
        return ptr_quoted;
    } else if (auto ptr_num = std::dynamic_pointer_cast<Number>(last_obj)) {
        return ptr_num;
    }

    return GetBoolPtr(true);
}

std::shared_ptr<Object> OrFunction::Compute(Scope &scope,
                                            const std::vector<std::shared_ptr<Object>> &objects) {
    if (objects.empty()) {
        return GetBoolPtr(false);
    }

    for (const auto &obj : objects) {
        auto ptr_bool = MakeBool(Eval(scope, obj));
        if (ptr_bool->GetValue()) {
            if (auto ptr_quoted = std::dynamic_pointer_cast<Quoted>(obj)) {
                return ptr_quoted;
            } else if (auto ptr_num = std::dynamic_pointer_cast<Number>(obj)) {
                return ptr_num;
            }
            return GetBoolPtr(true);
        }
    }

    auto last_obj = objects.back();
    if (auto ptr_quoted = std::dynamic_pointer_cast<Quoted>(last_obj)) {
        return ptr_quoted;
    } else if (auto ptr_num = std::dynamic_pointer_cast<Number>(last_obj)) {
        return ptr_num;
    }
    return GetBoolPtr(false);
}

std::shared_ptr<Object> LambdaFunction::Compute(
    Scope &, const std::vector<std::shared_ptr<Object>> &objects) {
    CheckForSyntaxError(objects.size() != lambda_variables_.size());

    MergeScopes();

    for (size_t iter = 0; iter < lambda_variables_.size(); ++iter) {
        std::vector<std::shared_ptr<Object>> for_definition(2);
        for_definition[0] = std::make_shared<Symbol>(lambda_variables_[iter]);
        for_definition[1] = objects[iter];

        local_scope_.Definition(for_definition);
    }

    for (const auto &part : lambda_parts_) {
        auto evaluation = Eval(local_scope_, part);
        if (evaluation) {
            return evaluation;
        }
    }

    throw std::runtime_error("error");
}