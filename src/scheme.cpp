#include "scheme.h"
#include "object.h"
#include <iostream>

Interpreter::Interpreter() : scope_(Heap::Make<Scope>().From()) {
    FunctionFactory factory = FunctionFactory::GetInstance();
    for (auto& [name, func] : factory.GetAll()) {
        func->SetScope(scope_);
        scope_->Set(name, func);
    }
}

Interpreter::~Interpreter() {
    // Heap& heap = Heap::GetInstance();
    // for (ObjectPtr ptr : heap.GetObjects()) {
    //     heap.Erase(ptr);
    // }
}

std::string Interpreter::Run(const std::string& s) {
    std::stringstream ss(s);
    Tokenizer tokenizer(&ss);
    ObjectPtr ast = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Parsing failed!");
    }
    if (!ast) {
        throw RuntimeError("RE!");
    }
    ast->SetScope(scope_);
    ObjectPtr res = ast->Eval(scope_);
    std::string ans = res ? res->Serialize() : "()";

    MarkAndSweep();

    return ans;
}

void Interpreter::MarkDFS(ObjectPtr v, std::unordered_set<ObjectPtr>& marks) {
    if (!v) {
        return;
    }
    marks.insert(v);
    std::vector<ObjectPtr> to_go;
    to_go.push_back(v->GetScope());
    if (Is<Cell>(v)) {
        to_go.push_back(As<Cell>(v)->GetFirst());
        to_go.push_back(As<Cell>(v)->GetSecond());
    } else if (Is<List>(v)) {
        for (ObjectPtr to : As<List>(v)->Get()) {
            to_go.push_back(to);
        }
    } else if (Is<Lambda>(v)) {
        for (ObjectPtr to : As<Lambda>(v)->GetArgs()) {
            to_go.push_back(to);
        }
        for (ObjectPtr to : As<Lambda>(v)->GetBody()) {
            to_go.push_back(to);
        }
    } else if (Is<Scope>(v)) {
        for (ObjectPtr to : As<Scope>(v)->GetAll()) {
            to_go.push_back(to);
        }
    }
    for (ObjectPtr to : to_go) {
        if (to && marks.find(to) == marks.end()) {
            MarkDFS(to, marks);
        }
    }
}

void Interpreter::MarkAndSweep() {
    std::unordered_set<ObjectPtr> marks;
    MarkDFS(scope_, marks);

    Heap& heap = Heap::GetInstance();
    std::unordered_set<ObjectPtr> objects = heap.GetObjects();
    for (ObjectPtr ptr : objects) {
        if (marks.find(ptr) == marks.end()) {
            heap.Erase(ptr);
        }
    }
}
