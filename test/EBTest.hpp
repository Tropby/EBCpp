#pragma once

#include <iostream>

#define XSTR(s) STR(s)
#define STR(s) #s

#define TEST(x)                                                                                                        \
    std::cout << "Start Test: " << STR(x) << std::endl;                                                                \
    x();

#define CHECK(x)                                                                                                       \
    std::cout << "Result: " << STR(x) << "\t\t";                                                                       \
    if (x)                                                                                                             \
    {                                                                                                                  \
        std::cout << "OKAY" << std::endl;                                                                              \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        std::cout << "ERROR" << std::endl;                                                                             \
    }
