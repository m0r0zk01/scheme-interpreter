#include "error.h"
#include "scheme.h"

#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

int main() {
    std::cout << "\nWelcome to Scheme Language Interpreter version 1.33.7!\n" << std::endl;
    Interpreter interpreter;
    while (true) {
        std::cout << ">> ";
        std::cout.flush();

        std::string req;
        std::string line;
        int open = 0;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            req += line;
            for (const char c : line) {
                if (c == '(') {
                    ++open;
                } else if (c == ')') {
                    --open;
                }
            }
            if (open == 0) {
                break;
            }
        }

        try {
            std::string res = interpreter.Run(req);
            std::cout << res + "\n" << std::endl;
        } catch (RuntimeError& e) {
            std::cout << "Runtime Error: " << e.what() << std::endl;
        } catch (SyntaxError& e) {
            std::cout << "Syntax Error: " << e.what() << std::endl;
        } catch (NameError& e) {
            std::cout << "Name Error: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception" << std::endl;
        }
    }
    return 0;
}
