#include <fmt/core.h>
#include <iostream>
#include <libformal/regexp_algorithms.hpp>

int main() {
    std::string regexp;
    char letter = 0;
    int count = 0;

    fmt::print("Hello! I can tell you length of shortest word starting with x^k "
               "that accepted by regular expression\n"
               "Enter regular expression, x and k: ");

    std::cin >> regexp >> letter >> count;
    int result = formal::GetPrefixedMin(regexp, letter, count);
    if (result != formal::INT_NONE) {
        fmt::print("Result: {}\n", result);
    } else {
        fmt::print("There is no such word ):\n");
    }

    return 0;
}