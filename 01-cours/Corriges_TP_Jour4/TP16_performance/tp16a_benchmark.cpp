#include "statistiques.hpp"
#include "test.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
bool rechercherLineaire(const std::vector<int> &v, int cible) {
    return std::find(v.begin(), v.end(), cible) != v.end();
}
bool rechercherBinaire(const std::vector<int> &v, int cible) {
    // PRÉCONDITION : données triées avec le même ordre. Le coût du tri est hors mesure.
    return std::binary_search(v.begin(), v.end(), cible);
}
int main(int argc, char **argv) {
    std::ofstream csv(argc > 1 ? argv[1] : "benchmark.csv");
    if (!csv)
        throw std::runtime_error("CSV inaccessible");
    csv << "taille;algorithme;repetitions;moyenne_ns;min_ns;max_ns;ecart_type_ns;trouves\n";
    auto stat = statistiques({1, 2, 3});
    CHECK(stat.moyenne == 2);
    CHECK(stat.min == 1);
    CHECK(stat.max == 3);
    CHECK(!rechercherBinaire({}, 1));
    CHECK(rechercherLineaire({1, 2, 3}, 2));
    for (int n : {10000, 100000, 1000000}) {
        std::vector<int> v(static_cast<std::size_t>(n));
        std::iota(v.begin(), v.end(), 0);
        for (bool binaire : {false, true}) {
            std::vector<double> temps;
            int trouves = 0;
            // Même série de cibles pour les deux algorithmes : alternance présent/absent.
            for (int i = 0; i < 100; ++i) {
                volatile int entree = i % 2 == 0 ? (i * 7919) % n : n + i;
                int cible =
                    entree; // lecture observable pour empêcher le calcul constant de la boucle.
                auto debut = std::chrono::steady_clock::now();
                bool trouve = binaire ? rechercherBinaire(v, cible) : rechercherLineaire(v, cible);
                auto fin = std::chrono::steady_clock::now();
                trouves += trouve ? 1 : 0; // Résultat réellement consommé, pas un calcul mort.
                temps.push_back(std::chrono::duration<double, std::nano>(fin - debut).count());
            }
            CHECK(trouves == 50);
            auto s = statistiques(temps);
            const char *nom = binaire ? "binaire" : "lineaire";
            csv << n << ';' << nom << ";100;" << s.moyenne << ';' << s.min << ';' << s.max << ';'
                << s.ecartType << ';' << trouves << '\n';
            std::cout << n << ' ' << nom << " : moyenne=" << s.moyenne << " ns min=" << s.min
                      << " max=" << s.max << " ecart-type=" << s.ecartType << '\n';
        }
    }
    csv.close();
    if (!csv)
        throw std::runtime_error("Ecriture CSV echouee");
    // Pour une recherche très courte, le coût de l'horloge n'est pas négligeable.
    // Ce TP illustre les ordres de grandeur ; un benchmark industriel regrouperait les appels.
}
