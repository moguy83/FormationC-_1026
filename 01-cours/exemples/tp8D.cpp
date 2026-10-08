
#include <vector>
#include <string>
#include <string_view>

std::vector<std::string> filtrerOptimise(
    const std::vector<std::string>& donnees,
    std::string_view prefixe
) {
    std::vector<std::string> resultat;

    // Au maximum, toutes les chaines correspondent
    resultat.reserve(donnees.size());

    for (const std::string& s : donnees) {

        // Le prefixe ne peut pas etre plus long
        if (s.size() < prefixe.size()) {
            continue;
        }

        // Compare uniquement le debut
        if (s.compare(
            0,
            prefixe.size(),
            prefixe.data(),
            prefixe.size()
        ) == 0) {
            resultat.push_back(s);
        }
    }

    return resultat;
}

// g++ -std=c++17 -O2 -g -fno-omit-frame-pointer tp8D.cpp -o tp8D
// perf record -g ./tp8D
// perf report

// g++ -std=c++17 -O2 -pg  tp8D.cpp -o tp8D_gprof
// ./tp8D_gprof
// gprof ./tp8D_gprof gmon.out