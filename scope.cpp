#include <scope.h>

void Scope::Definition(const std::vector<std::shared_ptr<Object>>& objects) {
    variables_[As<Symbol>(objects[0])->GetName()] = objects[1];
}

void Scope::Changing(const std::vector<std::shared_ptr<Object>>& objects) {
    CheckForLogicError(variables_.count(As<Symbol>(objects[0])->GetName()) == 0);
    Definition(objects);
}

void Scope::CarChanging(const std::vector<std::shared_ptr<Object>>& objects) {
    std::string pair_name = As<Symbol>(objects[0])->GetName();
    CheckForLogicError(variables_.count(pair_name) == 0);
    CheckForSyntaxError(!Is<Quoted>(variables_[pair_name]));

    auto ptr_pair = As<Quoted>(variables_[pair_name])->GetQuotedObj();
    CheckForSyntaxError(!Is<Cell>(ptr_pair));

    As<Cell>(ptr_pair)->SetFirst(objects[1]);
}

void Scope::CdrChanging(const std::vector<std::shared_ptr<Object>>& objects) {
    std::string pair_name = As<Symbol>(objects[0])->GetName();
    CheckForLogicError(variables_.count(pair_name) == 0);
    CheckForSyntaxError(!Is<Quoted>(variables_[pair_name]));

    auto ptr_pair = As<Quoted>(variables_[pair_name])->GetQuotedObj();
    CheckForSyntaxError(!Is<Cell>(ptr_pair));

    auto ptr_cell_pair = As<Cell>(ptr_pair);
    CheckForSyntaxError(GetArgsVector(ptr_cell_pair).size() != 2);
    if (Is<Cell>(ptr_cell_pair->GetSecond())) {
        As<Cell>(ptr_cell_pair->GetSecond())->SetFirst(objects[1]);
    } else {
        ptr_cell_pair->SetSecond(objects[1]);
    }
}

bool Scope::CheckVariable(const std::string& var_name) const {
    if (variables_.count(var_name) > 0) {
        return true;
    }

    return false;
}

std::shared_ptr<Object> Scope::GetVariable(const std::string& var_name) const {
    return variables_.at(var_name);
}