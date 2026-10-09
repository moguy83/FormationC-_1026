#include "date_iso.hpp"
#include "test.hpp"
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
struct Vente {
    std::chrono::sys_days date;
    std::string categorie;
    std::int64_t centimes;
};
// Dialecte documenté : séparateur ';', guillemets doublés, un enregistrement par ligne.
// Les champs multilignes ne sont pas acceptés ; les erreurs sont signalées, jamais ignorées.
std::vector<std::string> champsCSV(const std::string &ligne) {
    std::vector<std::string> r;
    std::size_t i = 0;
    for (;;) {
        std::string champ;
        if (i < ligne.size() && ligne[i] == '"') {
            ++i;
            bool ferme = false;
            while (i < ligne.size()) {
                char c = ligne[i++];
                if (c != '"') {
                    champ += c;
                    continue;
                }
                if (i < ligne.size() && ligne[i] == '"') {
                    champ += '"';
                    ++i;
                } else {
                    ferme = true;
                    break;
                }
            }
            if (!ferme)
                throw std::invalid_argument("Champ CSV non ferme");
            if (i < ligne.size() && ligne[i] != ';')
                throw std::invalid_argument("Texte apres guillemet");
        } else {
            while (i < ligne.size() && ligne[i] != ';') {
                if (ligne[i] == '"')
                    throw std::invalid_argument("Guillemet inattendu");
                champ += ligne[i++];
            }
        }
        r.push_back(champ);
        if (i == ligne.size())
            break;
        ++i;
    }
    return r;
}
std::int64_t montant(const std::string &s) {
    // Centimes entiers : 0.10 + 0.20 = exactement 30 centimes, sans arrondi binaire.
    auto point = s.find('.');
    if (point == 0 || point == std::string::npos || point + 3 != s.size())
        throw std::invalid_argument("Montant attendu 123.45");
    std::int64_t euros = 0;
    for (char c : s.substr(0, point)) {
        if (c < '0' || c > '9' || euros > (std::numeric_limits<std::int64_t>::max() / 100 - 9) / 10)
            throw std::invalid_argument("Montant invalide ou trop grand");
        euros = euros * 10 + c - '0';
    }
    if (s[point + 1] < '0' || s[point + 1] > '9' || s[point + 2] < '0' || s[point + 2] > '9')
        throw std::invalid_argument("Centimes invalides");
    return euros * 100 + (s[point + 1] - '0') * 10 + s[point + 2] - '0';
}
std::vector<Vente> charger(const std::string &path) {
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("CSV inaccessible : " + path);
    std::string ligne;
    if (!std::getline(in, ligne))
        throw std::invalid_argument("CSV vide");
    if (!ligne.empty() && ligne.back() == '\r')
        ligne.pop_back();
    if (ligne != "date;categorie;montant")
        throw std::invalid_argument("Entete incorrecte");
    std::vector<Vente> ventes;
    std::size_t numero = 1;
    while (std::getline(in, ligne)) {
        ++numero;
        if (!ligne.empty() && ligne.back() == '\r')
            ligne.pop_back();
        try {
            auto c = champsCSV(ligne);
            if (c.size() != 3 || c[1].empty())
                throw std::invalid_argument("Trois champs requis, categorie non vide");
            ventes.push_back({dateISO(c[0]), c[1], montant(c[2])});
        } catch (const std::exception &e) {
            throw std::runtime_error("Ligne " + std::to_string(numero) + " : " + e.what());
        }
    }
    if (in.bad())
        throw std::runtime_error("Erreur de lecture CSV");
    return ventes;
}
auto topCategories(const std::vector<Vente> &ventes, std::chrono::sys_days reference) {
    // 30 jours calendaires INCLUSIFS : référence-29 jusqu'à référence. Exclure le futur.
    auto recentes = ventes | std::views::filter([=](const Vente &v) {
                        return v.date >= reference - std::chrono::days{29} && v.date <= reference;
                    });
    std::map<std::string, std::int64_t> totaux;
    // C++20 n'a pas de views::group_by : agrégation explicite avec un algorithme ranges.
    std::ranges::for_each(recentes, [&](const Vente &v) {
        auto &total = totaux[v.categorie];
        if (v.centimes > std::numeric_limits<std::int64_t>::max() - total)
            throw std::overflow_error("Total trop grand");
        total += v.centimes;
    });
    std::vector<std::pair<std::string, std::int64_t>> classes(totaux.begin(), totaux.end());
    std::ranges::sort(classes, [](const auto &a, const auto &b) {
        return a.second != b.second ? a.second > b.second : a.first < b.first;
    });
    return classes; // Le vector possède les données ; aucune vue sur une variable détruite.
}
void tests() {
    CHECK(montant("12.34") == 1234);
    CHECK(montant("0.00") == 0);
    doitLever<std::invalid_argument>([] { montant("-1.00"); });
    doitLever<std::invalid_argument>([] { dateISO("2025-02-29"); });
    CHECK(dateISO("2024-02-29") < dateISO("2024-03-01"));
    CHECK(champsCSV("x;\"A;B\";1.00")[1] == "A;B");
    doitLever<std::invalid_argument>([] { champsCSV("\"non ferme"); });
    auto d = dateISO("2026-10-09");
    std::vector<Vente> v{{d, "A", 10},
                         {d - std::chrono::days{29}, "A", 20},
                         {d - std::chrono::days{30}, "A", 1000},
                         {d + std::chrono::days{1}, "A", 1000},
                         {d, "B", 30}};
    auto r = topCategories(v, d);
    CHECK(r.size() == 2);
    CHECK(r[0].first == "A");
    CHECK(r[0].second == 30);
    CHECK(topCategories({}, d).empty());
}
int main(int argc, char **argv) {
    try {
        tests();
        const std::string path = argc > 1 ? argv[1] : "donnees/ventes_1000.csv";
        // Référence fixe pour reproduire les résultats du jeu fourni. Une date peut être passée.
        const auto reference = dateISO(argc > 2 ? argv[2] : "2026-10-09");
        auto ventes = charger(path);
        auto top = topCategories(ventes, reference);
        std::cout << ventes.size() << " enregistrements charges\n";
        for (const auto &[categorie, total] : top | std::views::take(5))
            std::cout << categorie << " : " << total / 100 << '.' << std::setw(2)
                      << std::setfill('0') << total % 100 << " EUR\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
