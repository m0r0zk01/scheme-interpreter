#include "error.h"
#include "object.h"

ObjectPtr Function::Eval(ScopePtr working_scope) {
    return nullptr;
}

std::string Function::Serialize() {
    return "[Function]";
}

FunctionFactory::FunctionFactory() {
    factory_ = {
        {"+", Heap::Make<Plus>().From()},
        {"-", Heap::Make<Minus>().From()},
        {"*", Heap::Make<Multiply>().From()},
        {"/", Heap::Make<Divide>().From()},
        {"max", Heap::Make<Max>().From()},
        {"min", Heap::Make<Min>().From()},
        {"number?", Heap::Make<IsNumber>().From()},
        {"boolean?", Heap::Make<IsBoolean>().From()},
        {"pair?", Heap::Make<IsPair>().From()},
        {"null?", Heap::Make<IsNull>().From()},
        {"list?", Heap::Make<IsList>().From()},
        {"symbol?", Heap::Make<IsSymbol>().From()},
        {"not", Heap::Make<Not>().From()},
        {"abs", Heap::Make<Abs>().From()},
        {"=", Heap::Make<Equal>().From()},
        {">", Heap::Make<Greater>().From()},
        {"<", Heap::Make<Less>().From()},
        {">=", Heap::Make<NotLess>().From()},
        {"<=", Heap::Make<NotGreater>().From()},
        {"and", Heap::Make<And>().From()},
        {"or", Heap::Make<Or>().From()},
        {"'", Heap::Make<QuoteFunction>().From()},
        {"quote", Heap::Make<QuoteFunction>().From()},
        {"cons", Heap::Make<Cons>().From()},
        {"car", Heap::Make<Car>().From()},
        {"cdr", Heap::Make<Cdr>().From()},
        {"list", Heap::Make<ListFunction>().From()},
        {"list-ref", Heap::Make<ListRef>().From()},
        {"list-tail", Heap::Make<ListTail>().From()},
        {"define", Heap::Make<Define>().From()},
        {"set!", Heap::Make<Set>().From()},
        {"if", Heap::Make<If>().From()},
        {"set-car!", Heap::Make<SetCar>().From()},
        {"set-cdr!", Heap::Make<SetCdr>().From()},
        {"lambda", Heap::Make<LambdaFunction>().From()},
    };
}

FunctionFactory FunctionFactory::GetInstance() {
    return FunctionFactory();
}

FunctionPtr FunctionFactory::Get(const std::string& name) {
    if (factory_.find(name) == factory_.end()) {
        throw RuntimeError("Invalid symbol");
    }
    return factory_.at(name);
}

std::unordered_map<std::string, FunctionPtr>& FunctionFactory::GetAll() {
    return factory_;
}

ObjectPtr CollapseFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    NoNullptr(args);
    if (args.empty()) {
        return ApplyBinary(working_scope);
    } else if (args.size() == 1) {
        return ApplyBinary(working_scope, args.front()->Eval(working_scope));
    }
    ObjectPtr res = args.front()->Eval(working_scope);
    for (size_t i = 1; i < args.size(); ++i) {
        res = ApplyBinary(working_scope, res, args[i]->Eval(working_scope));
    }
    return res;
}

ObjectPtr Plus::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        return As<Object>(MakeNumber(0));
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(a_val + b_val));
}

ObjectPtr Minus::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        throw RuntimeError("RE!");
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(-a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(a_val - b_val));
}

ObjectPtr Multiply::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        return As<Object>(MakeNumber(1));
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(a_val * b_val));
}

ObjectPtr Divide::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        throw RuntimeError("RE!");
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(a_val / b_val));
}

ObjectPtr Max::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        throw RuntimeError("RE!");
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(std::max(a_val, b_val)));
}

ObjectPtr Min::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        throw RuntimeError("RE!");
    } else if (!b) {
        int64_t a_val = As<Number>(a)->GetValue();
        return As<Object>(MakeNumber(a_val));
    }
    auto a_val = As<Number>(a)->GetValue();
    auto b_val = As<Number>(b)->GetValue();
    return As<Object>(MakeNumber(std::min(a_val, b_val)));
}

ObjectPtr UnaryFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 1) {
        throw RuntimeError("RE!");
    }
    return ApplyUnary(args.front()->Eval(working_scope));
}

ObjectPtr IsNumber::ApplyUnary(ObjectPtr a) {
    return MakeBoolean(Is<Number>(a));
}

ObjectPtr IsBoolean::ApplyUnary(ObjectPtr a) {
    return MakeBoolean(Is<Boolean>(a));
}

ObjectPtr IsPair::ApplyUnary(ObjectPtr a) {
    if (Is<Cell>(a)) {
        ObjectPtr s = As<Cell>(a)->GetSecond();
        if (Is<Cell>(s)) {
            return MakeBoolean(As<Cell>(s)->GetSecond() == nullptr);
        }
        return MakeBoolean(true);
    }
    return MakeBoolean(false);
}

ObjectPtr IsNull::ApplyUnary(ObjectPtr a) {
    return MakeBoolean(!bool(a));
}

ObjectPtr IsList::ApplyUnary(ObjectPtr a) {
    if (Is<Cell>(a)) {
        ObjectPtr s = As<Cell>(a)->GetSecond();
        if (Is<Cell>(s)) {
            return MakeBoolean(As<Cell>(s)->GetSecond() == nullptr);
        }
        return MakeBoolean(false);
    }
    return MakeBoolean(a == nullptr);
}

ObjectPtr IsSymbol::ApplyUnary(ObjectPtr a) {
    return MakeBoolean(Is<Symbol>(a));
}

ObjectPtr Not::ApplyUnary(ObjectPtr a) {
    if (!Is<Boolean>(a)) {
        return MakeBoolean(false);
    }
    return MakeBoolean(!As<Boolean>(a)->GetValue());
}

ObjectPtr Abs::ApplyUnary(ObjectPtr a) {
    if (!a) {
        throw RuntimeError("RE!");
    }
    return As<Object>(MakeNumber(std::abs(As<Number>(a)->GetValue())));
}

ObjectPtr MonotoneFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.empty()) {
        return ApplyBinary(working_scope).first;
    }
    auto [res, ok1] = ApplyBinary(working_scope, args.front());
    for (size_t i = 1; i < args.size(); ++i) {
        auto [tres, ok] = ApplyBinary(working_scope, args[i - 1], args[i]);
        res = tres;
        if (ok) {
            return res;
        }
    }
    return res;
}

std::pair<ObjectPtr, bool> Equal::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!b) {
        return {As<Object>(MakeBoolean(true)), false};
    }
    auto a_val = As<Number>(a->Eval(working_scope))->GetValue();
    auto b_val = As<Number>(b->Eval(working_scope))->GetValue();
    bool res = a_val == b_val;
    return {As<Object>(MakeBoolean(res)), !res};
}

std::pair<ObjectPtr, bool> Greater::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!b) {
        return {As<Object>(MakeBoolean(true)), true};
    }
    auto a_val = As<Number>(a->Eval(working_scope))->GetValue();
    auto b_val = As<Number>(b->Eval(working_scope))->GetValue();
    bool res = a_val > b_val;
    return {As<Object>(MakeBoolean(res)), !res};
}

std::pair<ObjectPtr, bool> Less::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!b) {
        return {As<Object>(MakeBoolean(true)), true};
    }
    auto a_val = As<Number>(a->Eval(working_scope))->GetValue();
    auto b_val = As<Number>(b->Eval(working_scope))->GetValue();
    bool res = a_val < b_val;
    return {As<Object>(MakeBoolean(res)), !res};
}

std::pair<ObjectPtr, bool> NotLess::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!b) {
        return {As<Object>(MakeBoolean(true)), true};
    }
    auto a_val = As<Number>(a->Eval(working_scope))->GetValue();
    auto b_val = As<Number>(b->Eval(working_scope))->GetValue();
    bool res = a_val >= b_val;
    return {As<Object>(MakeBoolean(res)), !res};
}

std::pair<ObjectPtr, bool> NotGreater::ApplyBinary(ScopePtr working_scope, ObjectPtr a,
                                                   ObjectPtr b) {
    if (!b) {
        return {As<Object>(MakeBoolean(true)), true};
    }
    auto a_val = As<Number>(a->Eval(working_scope))->GetValue();
    auto b_val = As<Number>(b->Eval(working_scope))->GetValue();
    bool res = a_val <= b_val;
    return {As<Object>(MakeBoolean(res)), !res};
}

std::pair<ObjectPtr, bool> And::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        return {As<Object>(MakeBoolean(true)), true};
    } else if (!b) {
        return {a->Eval(working_scope), true};
    }
    auto a_val =
        !Is<Boolean>(a->Eval(working_scope)) || As<Boolean>(a->Eval(working_scope))->GetValue();
    if (!a_val) {
        return {a->Eval(working_scope), true};
    }
    auto b_val =
        !Is<Boolean>(b->Eval(working_scope)) || As<Boolean>(b->Eval(working_scope))->GetValue();
    return {b->Eval(working_scope), !b_val};
}

std::pair<ObjectPtr, bool> Or::ApplyBinary(ScopePtr working_scope, ObjectPtr a, ObjectPtr b) {
    if (!a && !b) {
        return {As<Object>(MakeBoolean(false)), true};
    } else if (!b) {
        return {a->Eval(working_scope), true};
    }
    auto a_val =
        !Is<Boolean>(a->Eval(working_scope)) || As<Boolean>(a->Eval(working_scope))->GetValue();
    if (a_val) {
        return {a->Eval(working_scope), true};
    }
    auto b_val =
        !Is<Boolean>(b->Eval(working_scope)) || As<Boolean>(b->Eval(working_scope))->GetValue();
    return {b->Eval(working_scope), !b_val};
}

ObjectPtr QuoteFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 1) {
        throw RuntimeError("RE!");
    }
    return args.front();
}

ObjectPtr Cons::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw RuntimeError("RE!");
    }
    CellPtr res =
        Heap::Make<Cell>().From(args[0]->Eval(working_scope), args[1]->Eval(working_scope));
    return As<Object>(res);
}

ObjectPtr Car::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 1) {
        throw RuntimeError("RE!");
    }
    CellPtr arg = As<Cell>(args.front()->Eval(working_scope));
    if (!arg) {
        throw RuntimeError("RE!");
    }
    return arg->GetFirst();
}

ObjectPtr Cdr::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 1) {
        throw RuntimeError("RE!");
    }
    CellPtr arg = As<Cell>(args.front()->Eval(working_scope));
    if (!arg) {
        throw RuntimeError("RE!");
    }
    return arg->GetSecond();
}

ObjectPtr ListFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    ListPtr res = Heap::Make<List>().From(args, true);
    return As<Object>(res->ToCell());
}

ObjectPtr ListRef::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw RuntimeError("RE!");
    }
    ListPtr res = As<Cell>(args.front()->Eval(working_scope))->ToList();
    auto index = As<Number>(args.back()->Eval(working_scope))->GetValue();
    if (index < 0 || static_cast<size_t>(index) >= res->Get().size()) {
        throw RuntimeError("RE!");
    }
    return res->Get()[index];
}

ObjectPtr ListTail::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw RuntimeError("RE!");
    }
    ListPtr list = As<Cell>(args.front()->Eval(working_scope))->ToList();
    auto index = As<Number>(args.back()->Eval(working_scope))->GetValue();
    if (index < 0 || static_cast<size_t>(index) > list->Get().size()) {
        throw RuntimeError("RE!");
    }
    std::vector<ObjectPtr> cur = list->Get();
    std::vector<ObjectPtr> res(cur.begin() + index, cur.end());
    ListPtr res_list = Heap::Make<List>().From(res, true);
    return As<Object>(res_list->ToCell());
}

ObjectPtr Define::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() < 2) {
        throw SyntaxError("Define wrong amount of arguments!");
    }
    ObjectPtr f = args.front();
    if (!f) {
        throw RuntimeError("Define name is empty!");
    }
    if (Is<Cell>(f)) {
        if (!Is<Cell>(f)) {
            throw SyntaxError("Define syntax error!");
        }
        std::vector<ObjectPtr> signature = As<Cell>(f)->ToList()->Get();
        std::string name = As<Symbol>(signature.front())->GetName();
        signature.erase(signature.begin());
        std::vector<ObjectPtr> body = args;
        body.erase(body.begin());
        LambdaPtr lambda = Heap::Make<Lambda>().From(signature, body);
        lambda->SetScope(working_scope);
        working_scope->Set(name, As<Object>(lambda));
    } else {
        if (args.size() > 2) {
            throw SyntaxError("Define wrong amount of arguments!");
        }
        SymbolPtr name = As<Symbol>(args.front());
        ObjectPtr value = args.back()->Eval(working_scope);
        working_scope->Set(name->GetName(), value);
    }
    return nullptr;
}

ObjectPtr Set::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw SyntaxError("Set invalid amount of arguments!");
    }
    SymbolPtr name = As<Symbol>(args.front());
    ObjectPtr value = args.back()->Eval(working_scope);
    if (!working_scope->Get(name->GetName())) {
        throw NameError("Set: No such variable!");
    }
    working_scope->SetRec(name->GetName(), value);
    return nullptr;
}

ObjectPtr If::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2 && args.size() != 3) {
        throw SyntaxError("If syntax error!");
    }
    ObjectPtr cond = args.front()->Eval(working_scope);
    ObjectPtr true_branch = args[1];
    ObjectPtr false_branch = args.size() == 3 ? args[2] : nullptr;
    if (!cond || !Is<Boolean>(cond) || As<Boolean>(cond)->GetValue()) {
        return true_branch ? true_branch->Eval(working_scope) : nullptr;
    }
    return false_branch ? false_branch->Eval(working_scope) : nullptr;
}

ObjectPtr SetCar::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw RuntimeError("RE!");
    }
    ObjectPtr to = args.front()->Eval(working_scope);
    ObjectPtr from = args.back();
    if (!Is<Cell>(to)) {
        throw RuntimeError("RE!");
    }
    As<Cell>(to)->SetFirst(from->Eval(working_scope));
    return nullptr;
}

ObjectPtr SetCdr::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != 2) {
        throw RuntimeError("RE!");
    }
    ObjectPtr to = args.front()->Eval(working_scope);
    ObjectPtr from = args.back();
    if (!Is<Cell>(to)) {
        throw RuntimeError("RE!");
    }
    As<Cell>(to)->SetSecond(from->Eval(working_scope));
    return nullptr;
}

ObjectPtr LambdaFunction::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() < 2) {
        throw SyntaxError("Too few arguments in lambda function!");
    }
    std::vector<ObjectPtr> lambda_args =
        args.front() ? As<Cell>(args.front())->ToList()->Get() : std::vector<ObjectPtr>();
    std::vector<ObjectPtr> lambda_body = args;
    lambda_body.erase(lambda_body.begin());
    LambdaPtr lambda = Heap::Make<Lambda>().From(lambda_args, lambda_body);
    lambda->SetScope(working_scope);
    return lambda;
}

int64_t Number::GetValue() const {
    return value_;
}

ObjectPtr Number::Eval(ScopePtr working_scope) {
    return this;
}

std::string Number::Serialize() {
    return std::to_string(value_);
}

const std::string& Symbol::GetName() const {
    return name_;
}

ObjectPtr Symbol::Eval(ScopePtr working_scope) {
    ObjectPtr val = working_scope->Get(name_);
    if (val) {
        return val;
    }
    throw NameError("Symbol not found!");
}

std::string Symbol::Serialize() {
    return name_;
}

bool Boolean::GetValue() const {
    return value_;
}

ObjectPtr Boolean::Eval(ScopePtr working_scope) {
    return this;
}

std::string Boolean::Serialize() {
    return value_ ? "#t" : "#f";
}

std::vector<ObjectPtr> List::Get() {
    return objects_;
}

ObjectPtr List::Eval(ScopePtr working_scope) {
    if (objects_.empty() || !objects_.front()) {
        throw RuntimeError("RE!");
    }

    FunctionPtr func = As<Function>(objects_.front()->Eval(working_scope));
    if (!func) {
        throw RuntimeError("RE!");
    }
    bool is_lambda = Is<Lambda>(func);

    std::vector<ObjectPtr> args = objects_;
    args.erase(args.begin());
    for (size_t i = 0; i < args.size(); ++i) {
        if (is_lambda && args[i]) {
            args[i] = args[i]->Eval(working_scope);
        }
    }

    return func->Apply(args, working_scope);
}

std::string List::Serialize() {
    return "[List]";
}

CellPtr List::ToCell() {
    if (objects_.empty()) {
        return nullptr;
    } else if (objects_.size() == 1) {
        return Heap::Make<Cell>().From(objects_.front(), ObjectPtr(nullptr));
    }
    CellPtr res = Heap::Make<Cell>().From();
    CellPtr cur = res;
    for (size_t i = 0; i + 2 < objects_.size(); ++i) {
        cur->SetFirst(objects_[i]);
        cur->SetSecond(As<Object>(Heap::Make<Cell>().From()));
        cur = As<Cell>(cur->GetSecond());
    }
    if (is_proper_) {
        cur->SetFirst(objects_[objects_.size() - 2]);
        cur->SetSecond(As<Object>(Heap::Make<Cell>().From()));
        cur = As<Cell>(cur->GetSecond());
        cur->SetFirst(objects_.back());
    } else {
        cur->SetFirst(objects_[objects_.size() - 2]);
        cur->SetSecond(objects_.back());
    }
    return res;
}

ObjectPtr Cell::GetFirst() {
    return first_;
}

ObjectPtr Cell::GetSecond() {
    return second_;
}

void Cell::SetFirst(ObjectPtr p) {
    first_ = p;
}

void Cell::SetSecond(ObjectPtr p) {
    second_ = p;
}

std::string Cell::Serialize() {
    if (first_ == nullptr && second_ == nullptr) {
        return "(())";
    }
    std::string res = "(";
    CellPtr cur = As<Cell>(this);
    while (true) {
        ObjectPtr s = cur->GetSecond();
        res += cur->GetFirst() ? cur->GetFirst()->Serialize() + " " : "()";
        if (s && !Is<Cell>(s)) {
            res += ". " + s->Serialize();
            break;
        }
        if (!s) {
            break;
        }
        cur = As<Cell>(s);
    }
    if (!res.empty() && res.back() == ' ') {
        res.pop_back();
    }
    res += ")";
    return res;
}

ObjectPtr Cell::Eval(ScopePtr working_scope) {
    ListPtr list = ToList();
    return list->Eval(working_scope);
}

ListPtr Cell::ToList() {
    std::vector<ObjectPtr> res;
    bool is_proper;
    CellPtr cur = As<Cell>(this);
    while (true) {
        res.push_back(cur->GetFirst());
        ObjectPtr s = cur->GetSecond();
        if (!s) {
            is_proper = true;
            break;
        }
        if (Is<Cell>(s)) {
            cur = As<Cell>(s);
        } else {
            res.push_back(s);
            is_proper = false;
            break;
        }
    }
    ListPtr res_list = Heap::Make<List>().From(res, is_proper);
    return res_list;
}

void NoNullptr(std::vector<ObjectPtr> list) {
    for (ObjectPtr el : list) {
        if (!el) {
            throw RuntimeError("RE!");
        }
    }
}

ObjectPtr Scope::Eval(ScopePtr working_scope) {
    return nullptr;
}

std::string Scope::Serialize() {
    return "[Scope]";
}

void Scope::Set(const std::string& name, ObjectPtr object) {
    objects_[name] = object;
}

void Scope::SetRec(const std::string& name, ObjectPtr object) {
    ScopePtr cur_scope = this;
    while (cur_scope) {
        if (cur_scope->objects_.find(name) != cur_scope->objects_.end()) {
            cur_scope->objects_[name] = object;
            return;
        }
        cur_scope = cur_scope->GetParentScope();
    }
}

ObjectPtr Scope::Get(const std::string& name) {
    ScopePtr cur_scope = this;
    while (cur_scope) {
        if (cur_scope->objects_.find(name) != cur_scope->objects_.end()) {
            return cur_scope->objects_[name];
        }
        cur_scope = cur_scope->GetParentScope();
    }
    return nullptr;
}

ScopePtr Scope::GetParentScope() {
    return parent_;
}

std::vector<ObjectPtr> Scope::GetAll() {
    std::vector<ObjectPtr> res;
    for (auto& [name, ptr] : objects_) {
        res.push_back(ptr);
    }
    return res;
}

Lambda::Lambda(const std::vector<ObjectPtr>& args, const std::vector<ObjectPtr>& body)
    : args_(args), body_(body) {
}

ObjectPtr Lambda::Eval(ScopePtr working_scope) {
    return nullptr;
}

std::string Lambda::Serialize() {
    return "[Lambda]";
}

ObjectPtr Lambda::Apply(std::vector<ObjectPtr> args, ScopePtr working_scope) {
    if (args.size() != args_.size()) {
        throw RuntimeError("RE!");
    }
    ScopePtr lambda_scope = Heap::Make<Scope>().From(scope_);
    for (size_t i = 0; i < args.size(); ++i) {
        std::string name = As<Symbol>(args_[i])->GetName();
        ObjectPtr val = args[i];
        lambda_scope->Set(name, val);
    }
    ObjectPtr res = nullptr;
    for (ObjectPtr func : body_) {
        res = func->Eval(lambda_scope);
    }
    return res;
}

std::vector<ObjectPtr> Lambda::GetArgs() {
    return args_;
}

std::vector<ObjectPtr> Lambda::GetBody() {
    return body_;
}
