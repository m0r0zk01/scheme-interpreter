#pragma once

#include "parser.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <unordered_set>

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    std::string Run(const std::string&);

private:
    void MarkDFS(ObjectPtr v, std::unordered_set<ObjectPtr>& marks);
    void MarkAndSweep();

    ScopePtr scope_ = nullptr;
};
