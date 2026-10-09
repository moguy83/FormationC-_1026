#pragma once
#include <stdexcept>
#include <string>
// Contrairement à assert(), CHECK reste actif dans une compilation Release (NDEBUG).
#define CHECK(expr)                                                                                \
    do {                                                                                           \
        if (!(expr))                                                                               \
            throw std::runtime_error(std::string("Echec : ") + #expr + " ligne " +                 \
                                     std::to_string(__LINE__));                                    \
    } while (false)
template <class E, class F> void doitLever(F action) {
    try {
        action();
    } catch (const E &) {
        return;
    }
    throw std::runtime_error("Exception attendue absente ou de type incorrect");
}
