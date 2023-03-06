#pragma once

#include <memory>
#include <variant>

#include "object.h"
#include "tokenizer.h"

ObjectPtr Read(Tokenizer* tokenizer);
CellPtr ReadList(Tokenizer* tokenizer);
