#pragma once

#include <error.h>

#include <memory>
#include <string>
#include <vector>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Empty : public Object {};

class Bool : public Object {
public:
    explicit Bool(bool value) : value_(value) {
    }

    bool GetValue() const {
        return value_;
    }

private:
    const bool value_;
};

class Number : public Object {
public:
    explicit Number(const int number) : number_(number) {
    }

    int GetValue() const {
        return number_;
    }

private:
    const int number_;
};

class Symbol : public Object {
public:
    explicit Symbol(std::string symbol) : symbol_(std::move(symbol)) {
    }

    const std::string &GetName() const {
        return symbol_;
    }

private:
    const std::string symbol_;
};

class Cell : public Object {
public:
    Cell() : first_(nullptr), second_(nullptr) {
    }

    void SetFirst(std::shared_ptr<Object> new_first) {
        first_ = new_first;
    }

    void SetSecond(std::shared_ptr<Object> new_second) {
        second_ = new_second;
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

private:
    std::shared_ptr<Object> first_, second_;
};

class Quoted : public Object {
public:
    Quoted(std::shared_ptr<Object> obj) : obj_(obj) {
    }

    std::shared_ptr<Object> GetQuotedObj() const {
        return obj_;
    }

private:
    std::shared_ptr<Object> obj_;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object> &obj) {
    std::shared_ptr<T> new_ptr = std::dynamic_pointer_cast<T>(obj);
    return new_ptr;
}

template <class T>
bool Is(const std::shared_ptr<Object> &obj) {
    Object *ptr = obj.get();
    if (dynamic_cast<T *>(ptr)) {
        return true;
    }

    return false;
}

std::vector<std::shared_ptr<Object>> GetArgsVector(const std::shared_ptr<Object> cell);
