#pragma once
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <vector>
struct Statistiques {
    double moyenne, min, max, ecartType;
};
inline Statistiques statistiques(const std::vector<double> &valeurs) {
    if (valeurs.empty())
        throw std::invalid_argument("Aucune mesure");
    const double moyenne =
        std::accumulate(valeurs.begin(), valeurs.end(), 0.0) / static_cast<double>(valeurs.size());
    double variance = 0;
    for (double v : valeurs)
        variance += (v - moyenne) * (v - moyenne);
    // Écart-type de population (division par N), pas estimateur d'échantillon (N-1).
    auto [mini, maxi] = std::minmax_element(valeurs.begin(), valeurs.end());
    return {moyenne, *mini, *maxi, std::sqrt(variance / static_cast<double>(valeurs.size()))};
}
inline double percentile(std::vector<double> valeurs, double p) {
    if (valeurs.empty() || !std::isfinite(p) || p <= 0 || p > 100)
        throw std::invalid_argument("Percentile invalide");
    std::sort(valeurs.begin(), valeurs.end());
    // Convention nearest-rank : ceil(p/100*N)-1. D'autres conventions interpolent.
    const auto rang =
        static_cast<std::size_t>(std::ceil(p / 100 * static_cast<double>(valeurs.size())));
    return valeurs.at(rang - 1);
}
