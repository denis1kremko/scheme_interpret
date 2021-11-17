#pragma once

#include <map>
#include <vector>
#include <string>
#include <object.h>

class Scope {
public:
    void Definition(const std::vector<std::shared_ptr<Object>>& objects);
    void Changing(const std::vector<std::shared_ptr<Object>>& objects);
    void CarChanging(const std::vector<std::shared_ptr<Object>>& objects);
    void CdrChanging(const std::vector<std::shared_ptr<Object>>& objects);
    bool CheckVariable(const std::string& var_name) const;
    std::shared_ptr<Object> GetVariable(const std::string& var_name) const;
    std::map<std::string, std::shared_ptr<Object>> variables_;
};