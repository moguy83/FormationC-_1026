#pragma once
#include <chrono>
#include <stdexcept>
#include <string_view>
inline std::chrono::sys_days dateISO(std::string_view s) {
    // Une regex seule ne voit pas qu'un 31 février est impossible : valider le calendrier.
    if (s.size() != 10 || s[4] != '-' || s[7] != '-')
        throw std::invalid_argument("Date attendue AAAA-MM-JJ");
    int n[3] = {0, 0, 0};
    int bloc = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (i == 4 || i == 7) {
            ++bloc;
            continue;
        }
        if (s[i] < '0' || s[i] > '9')
            throw std::invalid_argument("Date non numerique");
        n[bloc] = n[bloc] * 10 + (s[i] - '0');
    }
    const std::chrono::year_month_day d{std::chrono::year{n[0]},
                                        std::chrono::month{static_cast<unsigned>(n[1])},
                                        std::chrono::day{static_cast<unsigned>(n[2])}};
    if (n[0] < 1 || !d.ok())
        throw std::invalid_argument("Date civile invalide");
    return std::chrono::sys_days{d};
}
