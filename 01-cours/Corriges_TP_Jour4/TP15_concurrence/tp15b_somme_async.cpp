#include "test.hpp"
#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <numeric>
#include <vector>
int main() {
    constexpr std::size_t n = 10000000;
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);
    using Horloge = std::chrono::steady_clock;
    auto debut = Horloge::now();
    // L'accumulateur initial décide du type du calcul : 0 serait un int et déborderait.
    auto seq = std::accumulate(v.begin(), v.end(), std::int64_t{0});
    auto milieu = Horloge::now();
    std::vector<std::future<std::int64_t>> resultats;
    for (std::size_t i = 0; i < 4; ++i) {
        auto a = n * i / 4,
             b = n * (i + 1) / 4; // Répartition exacte même si n n'est pas multiple de 4.
        resultats.push_back(std::async(std::launch::async, [&v, a, b] {
            return std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(a),
                                   v.begin() + static_cast<std::ptrdiff_t>(b), std::int64_t{0});
        }));
    }
    std::int64_t par = 0;
    for (auto &f : resultats)
        par += f.get(); // get attend et propage une éventuelle exception.
    auto fin = Horloge::now();
    CHECK(seq == 50000005000000LL);
    CHECK(seq == par);
    double seqMs = std::chrono::duration<double, std::milli>(milieu - debut).count();
    double parMs = std::chrono::duration<double, std::milli>(fin - milieu).count();
    std::cout << "Somme=" << par << " ; sequentiel=" << seqMs << " ms ; async=" << parMs << " ms\n";
    if (parMs > 0)
        std::cout << "Speedup observe=" << seqMs / parMs << "\n";
    // Temps async inclut lancement/join des threads. Aucun gain garanti : bande passante mémoire,
    // charge système et nombre de cœurs peuvent dominer. Mesurer en Release, répéter les mesures.
}
