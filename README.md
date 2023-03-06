# scheme-interpreter

An implementation of basic functions of [Scheme](https://en.wikipedia.org/wiki/Scheme_(programming_language)) programming language in C++. It provides a library with an interpreter that accepts strings, parses them into tokens, builds AST and evaluates it. There is also a simple terminal REPL for testing.

![image](https://user-images.githubusercontent.com/47718803/222995984-4758fb06-62c3-4ce8-b42f-00f8e78c4255.png)

# Installation
```
git clone https://github.com/m0r0zk01/scheme-interpreter.git
cd scheme-interpreter
mkdir build && cd build
cmake .. && make
```

This will build targets: `repl` - REPL, `tests/test_*` - tests, you can run them with `ctest`
