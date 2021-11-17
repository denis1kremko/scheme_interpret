#include "scheme.h"

std::string ObjectToString(const std::shared_ptr<Object> &obj) {
    if (obj == nullptr) {
        return "()";
    } else if (auto ptr_bool = dynamic_cast<Bool *>(obj.get())) {
        if (ptr_bool->GetValue()) {
            return "#t";
        } else {
            return "#f";
        }

    } else if (auto ptr_num = dynamic_cast<Number *>(obj.get())) {
        return std::to_string(ptr_num->GetValue());

    } else if (auto ptr_symbol = dynamic_cast<Symbol *>(obj.get())) {
        return ptr_symbol->GetName();

    } else if (auto ptr_cell = dynamic_cast<Cell *>(obj.get())) {
        std::string str;

        str += '(';
        while (true) {
            str += ObjectToString(ptr_cell->GetFirst());
            if (ptr_cell->GetSecond() == nullptr) {
                break;
            }

            if (dynamic_cast<Cell *>(ptr_cell->GetSecond().get())) {
                ptr_cell = dynamic_cast<Cell *>(ptr_cell->GetSecond().get());
                str += ' ';
            } else {
                str += " . ";
                str += ObjectToString(ptr_cell->GetSecond());
                break;
            }
        }
        str += ')';

        return str;

    } else if (auto ptr_quoted = dynamic_cast<Quoted *>(obj.get())) {
        return ObjectToString(ptr_quoted->GetQuotedObj());
    }

    throw std::runtime_error("runtime error");
}

void LambdaSugarCase(Scope &scope, const std::vector<std::shared_ptr<Object>> &args) {
    CheckForSyntaxError(args[0] == nullptr);
    CheckForSyntaxError(!Is<Symbol>(As<Cell>(args[0])->GetFirst()));

    auto func_name = As<Cell>(args[0])->GetFirst();

    std::shared_ptr<Object> lambda_def = std::make_shared<Cell>();
    auto ptr_lambda_cell = As<Cell>(lambda_def);

    ptr_lambda_cell->SetFirst(std::make_shared<Symbol>("lambda"));
    ptr_lambda_cell->SetSecond(std::make_shared<Cell>());

    auto ptr_second_lambda_cell = As<Cell>(ptr_lambda_cell->GetSecond());
    ptr_second_lambda_cell->SetFirst(As<Cell>(args[0])->GetSecond());
    ptr_second_lambda_cell->SetSecond(std::make_shared<Cell>());

    auto ptr_third_lambda_cell = As<Cell>(ptr_second_lambda_cell->GetSecond());
    ptr_third_lambda_cell->SetFirst(args[1]);

    scope.Definition({func_name, Eval(scope, lambda_def)});
}

std::shared_ptr<Object> Eval(Scope &scope, const std::shared_ptr<Object> &obj) {
    CheckForRuntimeError(obj == nullptr);

    if (Is<Number>(obj)) {
        return As<Number>(obj);
    }

    if (Is<Symbol>(obj)) {
        std::string name = As<Symbol>(obj)->GetName();

        if (scope.CheckVariable(name)) {
            return scope.GetVariable(name);
        }

        if (name == "+" || name == "-" || name == "*" || name == "/") {
            return std::make_shared<SimpleFunction>(name);
        }

        // bool_function
        if (name == "pair?") {
            return std::make_shared<IsFunction>(Type::Pair);
        }
        if (name == "list?") {
            return std::make_shared<IsFunction>(Type::List);
        }
        if (name == "null?") {
            return std::make_shared<IsFunction>(Type::Null);
        }
        if (name == "symbol?") {
            return std::make_shared<IsFunction>(Type::Symbol);
        }
        if (name == "boolean?") {
            return std::make_shared<IsFunction>(Type::Boolean);
        }
        if (name == "number?") {
            return std::make_shared<IsFunction>(Type::Integer);
        }
        if (name == "not") {
            return std::make_shared<NotFunction>();
        }
        if (name == "and") {
            return std::make_shared<AndFunction>();
        }
        if (name == "or") {
            return std::make_shared<OrFunction>();
        }
        if (name == "if") {
            return std::make_shared<IfFunction>();
        }

        // comparisons
        if (name == "=" || name == "<" || name == ">" || name == ">=" || name == "<=") {
            return std::make_shared<Comparison>(name);
        }

        // max min abs
        if (name == "max") {
            return std::make_shared<MaxFunction>();
        }
        if (name == "min") {
            return std::make_shared<MinFunction>();
        }
        if (name == "abs") {
            return std::make_shared<AbsFunction>();
        }

        // bool
        if (name == "#f") {
            return std::make_shared<Bool>(false);
        }
        if (name == "#t") {
            return std::make_shared<Bool>(true);
        }

        // list func
        if (name == "list") {
            return std::make_shared<ListFunction>();
        }
        if (name == "list-ref") {
            return std::make_shared<ListRefFunction>();
        }
        if (name == "list-tail") {
            return std::make_shared<ListTailFunction>();
        }
        if (name == "cons") {
            return std::make_shared<ConsFunction>();
        }
        if (name == "car") {
            return std::make_shared<CarFunction>();
        }
        if (name == "cdr") {
            return std::make_shared<CdrFunction>();
        }

        if (name == "quote") {
            return std::make_shared<QuoteFunction>();
        }
        if (name == "define") {
            return std::make_shared<DefineFunction>();
        }
        if (name == "set!") {
            return std::make_shared<SetFunction>();
        }
        if (name == "set-car!") {
            return std::make_shared<SetCarFunction>();
        }
        if (name == "set-cdr!") {
            return std::make_shared<SetCdrFunction>();
        }

        if (name == "lambda") {
            return std::make_shared<RawLambdaFunction>();
        }

        // обработка ошибок
        CheckForLogicError(std::isalpha(name[0]));
    }

    if (Is<Cell>(obj)) {
        // превращаем в клетку
        auto ptr_cell = As<Cell>(obj);

        // проверяем, что первый аргумент это функция
        CheckForRuntimeError(!Is<Function>(Eval(scope, ptr_cell->GetFirst())));

        // превращаем первый аргумент в функцию
        auto func = As<Function>(Eval(scope, ptr_cell->GetFirst()));

        // аргументы для функции
        auto args = GetArgsVector(ptr_cell->GetSecond());

        // случай if
        if (dynamic_cast<IfFunction *>(func.get())) {
            CheckForSyntaxError(args.size() != 2 && args.size() != 3);
            auto ptr_eval = Eval(scope, args[0]);
            CheckForRuntimeError(!Is<Bool>(ptr_eval));

            bool if_value = As<Bool>(Eval(scope, args[0]))->GetValue();
            if (if_value) {
                return Eval(scope, args[1]);
            } else if (args.size() == 3) {
                return Eval(scope, args[2]);
            }

            return nullptr;
        }
        // случай quote and or
        if (dynamic_cast<QuoteFunction *>(func.get()) || dynamic_cast<AndFunction *>(func.get()) ||
            dynamic_cast<OrFunction *>(func.get())) {
            return func->Compute(scope, args);
        }
        // случай define
        if (dynamic_cast<DefineFunction *>(func.get())) {
            CheckForSyntaxError(args.size() != 2);

            // сахар для функций
            if (Is<Cell>(args[0])) {
                LambdaSugarCase(scope, args);
            } else {
                args[1] = Eval(scope, args[1]);
                scope.Definition(args);
            }

            return nullptr;
        }
        // случай set!
        if (dynamic_cast<SetFunction *>(func.get())) {
            CheckForSyntaxError(args.size() != 2);

            args[1] = Eval(scope, args[1]);
            scope.Changing(args);
            return nullptr;
        }
        if (dynamic_cast<SetCarFunction *>(func.get())) {
            CheckForSyntaxError(args.size() != 2);

            args[1] = Eval(scope, args[1]);
            scope.CarChanging(args);
            return nullptr;
        }
        if (dynamic_cast<SetCdrFunction *>(func.get())) {
            CheckForSyntaxError(args.size() != 2);

            args[1] = Eval(scope, args[1]);
            scope.CdrChanging(args);
            return nullptr;
        }
        // случай lambda
        if (dynamic_cast<RawLambdaFunction *>(func.get())) {
            CheckForSyntaxError(args.size() < 2);
            std::shared_ptr<Object> lambda_func = std::make_shared<LambdaFunction>(scope);
            As<LambdaFunction>(lambda_func)->Init(args);
            return lambda_func;
        }

        // eval all args
        for (auto &arg : args) {
            arg = Eval(scope, arg);
        }
        return func->Compute(scope, args);
    }

    if (Is<Quoted>(obj)) {
        return As<Quoted>(obj);
    }

    // обработка ошибки
    throw SyntaxError("error massage");
}

std::string Proceed(Scope &scope, const std::string &code) {
    std::stringstream code_stream(code);
    Tokenizer tokenizer(&code_stream);

    // что нужно евалить:
    auto obj = Read(&tokenizer);
    // проверяем на конец потока
    CheckForSyntaxError(!tokenizer.IsEnd());
    return ObjectToString(Eval(scope, obj));
}
