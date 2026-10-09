#include "test.hpp"
#include <algorithm>
#include <chrono>
#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
using Comptes = std::map<std::string, std::size_t>;
std::vector<std::string> normaliser(const std::vector<std::string> &mots) {
    std::vector<std::string> r;
    r.reserve(mots.size());
    for (auto mot : mots) {
        // cctype attend unsigned char (ou EOF) : char signé négatif => comportement indéfini.
        for (char &c : mot)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        r.push_back(std::move(mot));
    }
    return r; // Normalisation ASCII du jeu fourni, pas un traitement Unicode général.
}
Comptes compterAvant(const std::vector<std::string> &mots) {
    Comptes comptes;
    for (const auto &mot : mots) {
        if (comptes.contains(mot))
            continue;
        // Hotspot attendu : on reparcourt TOUS les mots pour chaque mot distinct : O(U*N).
        comptes[mot] = static_cast<std::size_t>(std::count(mots.begin(), mots.end(), mot));
    }
    return comptes;
}
Comptes compterApres(const std::vector<std::string> &mots) {
    std::unordered_map<std::string, std::size_t> index;
    index.reserve(mots.size());
    for (const auto &mot : mots)
        ++index[mot]; // Un seul passage : coût moyen O(N), hors tri final.
    return Comptes(index.begin(), index.end()); // Même ordre déterministe que la version initiale.
}
std::size_t produireRapport(const Comptes &comptes) {
    std::size_t total = 0;
    for (const auto &[mot, n] : comptes) {
        CHECK(!mot.empty());
        total += n;
    }
    return total;
}
int main(int argc, char **argv) {
    const std::string mode = argc > 1 ? argv[1] : "comparaison";
    if (mode != "avant" && mode != "apres" && mode != "comparaison")
        throw std::invalid_argument("Mode : avant/apres/comparaison");
    std::vector<std::string> brut;
    for (int i = 0; i < 20000; ++i)
        brut.push_back("MOT" + std::to_string(i % 1000));
    auto mots = normaliser(brut);
    auto executer = [&](bool optimise) {
        auto debut = std::chrono::steady_clock::now();
        auto comptes = optimise ? compterApres(mots) : compterAvant(mots);
        CHECK(produireRapport(comptes) == 20000);
        CHECK(comptes.size() == 1000);
        double ms =
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - debut)
                .count();
        std::cout << (optimise ? "apres" : "avant") << " : " << ms << " ms\n";
        return std::pair{comptes, ms};
    };
    if (mode == "avant") {
        executer(false);
        return 0;
    }
    if (mode == "apres") {
        executer(true);
        return 0;
    }
    auto avant = executer(false), apres = executer(true);
    CHECK(avant.first == apres.first);
    if (apres.second > 0)
        std::cout << "Gain observe=" << avant.second / apres.second << '\n';
    // Valgrind mesure des coûts instrumentés : comparer le temps natif séparément en Release.
    // Ne pas inventer de top 3 : Callgrind doit le mesurer sur votre exécution.
}
