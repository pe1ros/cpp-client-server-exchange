#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

inline static short port = 5555;

inline namespace Requests {
    static std::string Registration = "Registration";
    static std::string Hello = "Hello";
    static std::string Trading = "Trading";
    static std::string Balance = "Balance";
}

#endif //CLIENSERVERECN_COMMON_HPP
