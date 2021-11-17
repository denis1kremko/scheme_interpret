#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token next_token = tokenizer->GetToken();
    if (!tokenizer->IsEnd()) {
        tokenizer->Next();
    }

    if (std::get_if<BracketToken>(&next_token)) {
        const BracketToken* bracket_token = std::get_if<BracketToken>(&next_token);
        if (*bracket_token == BracketToken::OPEN) {
            return ReadList(tokenizer);
        } else {
            return std::make_shared<Symbol>(")");
        }

    } else if (std::get_if<ConstantToken>(&next_token)) {
        const ConstantToken* const_token = std::get_if<ConstantToken>(&next_token);
        return std::make_shared<Number>(const_token->value);

    } else if (std::get_if<SymbolToken>(&next_token)) {
        const SymbolToken* symbol_token = std::get_if<SymbolToken>(&next_token);
        return std::make_shared<Symbol>(symbol_token->name);

    } else if (std::get_if<DotToken>(&next_token)) {
        return std::make_shared<Symbol>(".");

    } else if (std::get_if<QuoteToken>(&next_token)) {
        return std::make_shared<Quoted>(Read(tokenizer));
    }

    throw SyntaxError{"error message"};
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    CheckForSyntaxError(tokenizer->IsEnd());

    auto is_close_bracket = [](std::shared_ptr<Object> obj) {
        return Is<Symbol>(obj) && As<Symbol>(obj)->GetName() == ")";
    };
    auto is_dot = [](std::shared_ptr<Object> obj) {
        return Is<Symbol>(obj) && As<Symbol>(obj)->GetName() == ".";
    };

    std::shared_ptr<Cell> list_obj = std::make_shared<Cell>();
    std::shared_ptr<Cell> main_cell = list_obj;

    auto obj = Read(tokenizer);

    // если лист пустой
    if (is_close_bracket(obj)) {
        return nullptr;
    }

    CheckForSyntaxError(tokenizer->IsEnd());

    main_cell->SetFirst(obj);
    obj = Read(tokenizer);
    // заполняем лист
    while (!is_close_bracket(obj) && !is_dot(obj)) {
        auto new_main_cell = std::make_shared<Cell>();
        main_cell->SetSecond(new_main_cell);

        main_cell = new_main_cell;
        main_cell->SetFirst(obj);

        obj = Read(tokenizer);

        CheckForSyntaxError(tokenizer->IsEnd() && !is_close_bracket(obj));
    }

    // последний элемент в листе
    if (is_close_bracket(obj)) {
        main_cell->SetSecond(nullptr);
    } else {
        obj = Read(tokenizer);

        CheckForSyntaxError(tokenizer->IsEnd() || is_close_bracket(obj) || is_dot(obj));

        main_cell->SetSecond(obj);
        // скипаем закрывающую скобку
        obj = Read(tokenizer);

        // проверяем, что это закрывающая скобка
        CheckForSyntaxError(!is_close_bracket(obj));
    }

    return list_obj;
}