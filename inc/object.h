#pragma once

#include "error.h"

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

class Object;
class Function;
class Number;
class Symbol;
class Boolean;
class List;
class Cell;
class Lambda;
class Scope;

using ObjectPtr = Object*;
using FunctionPtr = Function*;
using NumberPtr = Number*;
using SymbolPtr = Symbol*;
using BooleanPtr = Boolean*;
using ListPtr = List*;
using CellPtr = Cell*;
using LambdaPtr = Lambda*;
using ScopePtr = Scope*;

///////////////////////////////////////////////////////////////////////////////
// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
T* As(ObjectPtr obj) {
    T* res = dynamic_cast<T*>(obj);
    if constexpr (std::is_same_v<Number, T>) {
        if (!res) {
            throw RuntimeError("RE!");
        }
    }
    if (obj && !res) {
        throw RuntimeError("RE!");
    }
    return res;
}

template <class T>
bool Is(ObjectPtr obj) {
    return dynamic_cast<T*>(obj) != nullptr;
}

///////////////////////////////////////////////////////////////////////////////

#define MakeNumber(x) Heap::GetInstance().Make<Number>().From(x)
#define MakeBoolean(x) Heap::GetInstance().Make<Boolean>().From(x)

class Object : public std::enable_shared_from_this<Object> {
public:
    // Object() = delete;
    // Object(ScopePtr scope) : scope_(scope) {}
    virtual ~Object() = default;

    virtual ObjectPtr Eval(ScopePtr working_scope) = 0;
    virtual std::string Serialize() = 0;

    inline ScopePtr GetScope() {
        return scope_;
    }

    inline void SetScope(ScopePtr scope) {
        scope_ = scope;
    }

protected:
    ScopePtr scope_ = nullptr;
};

class Function : public Object {
public:
    virtual ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) = 0;

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

    //    ScopePtr GetWorkingScope();
    //    void SetWorkingScope(ScopePtr scope);

protected:
};

class FunctionFactory {
public:
    static FunctionFactory GetInstance();

    FunctionPtr Get(const std::string& name);

    std::unordered_map<std::string, FunctionPtr>& GetAll();

private:
    FunctionFactory();
    FunctionFactory(FunctionFactory&) = delete;
    FunctionFactory(FunctionFactory&&) = delete;
    FunctionFactory& operator=(FunctionFactory) = delete;

    std::unordered_map<std::string, FunctionPtr> factory_;
};

class CollapseFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
    virtual ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                  ObjectPtr b = nullptr) = 0;
};

class Plus : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class Minus : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class Multiply : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class Divide : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class Max : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class Min : public CollapseFunction {
public:
    ObjectPtr ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                          ObjectPtr b = nullptr) override;
};

class UnaryFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
    virtual ObjectPtr ApplyUnary(ObjectPtr a) = 0;
};

class IsNumber : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class IsBoolean : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class IsPair : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class IsNull : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class IsList : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class IsSymbol : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class Not : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class Abs : public UnaryFunction {
public:
    ObjectPtr ApplyUnary(ObjectPtr a) override;
};

class MonotoneFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
    virtual std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                                   ObjectPtr b = nullptr) = 0;
};

class Equal : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class Greater : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class Less : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class NotLess : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class NotGreater : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class And : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class Or : public MonotoneFunction {
public:
    std::pair<ObjectPtr, bool> ApplyBinary(ScopePtr working_scope, ObjectPtr a = nullptr,
                                           ObjectPtr b = nullptr) override;
};

class QuoteFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Cons : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Car : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Cdr : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class ListFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class ListRef : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class ListTail : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Define : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Set : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class If : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class SetCar : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class SetCdr : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class LambdaFunction : public Function {
public:
    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;
};

class Number : public Object {
public:
    explicit Number(int64_t value) : value_(value) {
    }

    int64_t GetValue() const;

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    explicit Symbol(std::string_view s) : name_(s) {
    }

    const std::string& GetName() const;

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

private:
    std::string name_;
};

class Boolean : public Object {
public:
    explicit Boolean(bool value) : value_(value) {
    }

    bool GetValue() const;

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

private:
    bool value_;
};

class List : public Object {
public:
    List(const std::vector<ObjectPtr>& objects, bool is_proper)
        : objects_(objects), is_proper_(is_proper) {
    }

    std::vector<ObjectPtr> Get();

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

    CellPtr ToCell();

private:
    std::vector<ObjectPtr> objects_;
    bool is_proper_ = true;
};

class Cell : public Object {
public:
    Cell() = default;
    template <typename U, typename V>
    Cell(U* f, V* s) : first_(As<Object>(f)), second_(As<Object>(s)) {
    }

    ObjectPtr GetFirst();
    ObjectPtr GetSecond();

    void SetFirst(ObjectPtr p);
    void SetSecond(ObjectPtr p);

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

    ListPtr ToList();

private:
    ObjectPtr first_ = nullptr;
    ObjectPtr second_ = nullptr;
};

void NoNullptr(std::vector<ObjectPtr> list);

class Scope : public Object {
public:
    explicit Scope(ScopePtr parent = nullptr) : parent_(parent) {
    }

    ObjectPtr Eval(ScopePtr working_scope) override;
    std::string Serialize() override;

    void Set(const std::string& name, ObjectPtr object);
    void SetRec(const std::string& name, ObjectPtr object);
    ObjectPtr Get(const std::string& name);
    ScopePtr GetParentScope();

    std::vector<ObjectPtr> GetAll();

private:
    ScopePtr parent_ = nullptr;
    std::unordered_map<std::string, ObjectPtr> objects_;
};

class Lambda : public Function {
public:
    Lambda(const std::vector<ObjectPtr>& args, const std::vector<ObjectPtr>& body);

    ObjectPtr Eval(ScopePtr working_scope) override;

    std::string Serialize() override;

    ObjectPtr Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) override;

    std::vector<ObjectPtr> GetArgs();
    std::vector<ObjectPtr> GetBody();

private:
    std::vector<ObjectPtr> args_;
    std::vector<ObjectPtr> body_;
};

class Heap {
public:
    inline ~Heap() {
        for (ObjectPtr ptr : objects_) {
            delete ptr;
        }
        objects_.clear();
    }

    template <typename T>
    static auto Make() {
        return Maker<T>();
    }

    static Heap& GetInstance() {
        static Heap instance;
        return instance;
    }

    inline std::unordered_set<ObjectPtr> GetObjects() {
        return objects_;
    }

    inline void Erase(ObjectPtr ptr) {
        auto it = objects_.find(ptr);
        if (it != objects_.end()) {
            delete ptr;
            objects_.erase(it);
        }
    }

private:
    template <typename T>
    struct Maker {
        template <typename... Args>
        T* From(Args&&... args) {
            if (!std::is_base_of_v<Object, T>) {
                throw RuntimeError("Trying to create Object of wrong type!");
            }
            T* object = new T(std::forward<Args>(args)...);
            GetInstance().objects_.insert(object);
            return object;
        }
    };

    Heap() = default;

    std::unordered_set<ObjectPtr> objects_;
};
