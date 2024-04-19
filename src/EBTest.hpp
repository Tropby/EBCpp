#pragma once

#include <iostream>

#define XSTR(s) STR(s)
#define STR(s) #s

#define TEST(x)                                                                                                        \
    void x();                                                                                                          \
    std::cout << "Start Test: " << STR(x) << std::endl;                                                                \
    x();

#define CHECK_EXCEPTION(x)                                                                                             \
    std::cout << "Check Exception: " << STR(x) << "\t\t";                                                              \
    try                                                                                                                \
    {                                                                                                                  \
        x;                                                                                                             \
        std::cout << "ERROR" << std::endl;                                                                             \
    }                                                                                                                  \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        std::cout << "OKAY" << std::endl;                                                                              \
    }

#define CHECK(x)                                                                                                       \
    std::cout << "Check: " << STR(x) << "\t\t";                                                                        \
    if (x)                                                                                                             \
    {                                                                                                                  \
        std::cout << "OKAY" << std::endl;                                                                              \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        std::cout << "ERROR" << std::endl;                                                                             \
    }
