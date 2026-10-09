#include "test.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
struct Produit {
    int id;
    std::string nom;
    double prix;
};
// C++17 : une référence const exprime une collection obligatoire, sans copie.
// optional exprime l'absence dans le type, sans paramètre de sortie ni pointeur nul.
std::optional<Produit> chercher(const std::vector<Produit> &produits, const std::string &nom) {
    auto it = std::find_if(produits.begin(), produits.end(),
                           [&nom](const Produit &p) { return p.nom == nom; });
    if (it == produits.end())
        return std::nullopt;
    return *it; // Copie possédée : ne devient pas pendante quand le vector est détruit/modifié.
}
int main() {
    std::vector<Produit> produits{{1, "Clavier", 35}, {2, "Souris", 20}};
    auto resultat = chercher(produits, "Clavier");
    CHECK(resultat.has_value());
    CHECK(resultat->id == 1);
    resultat->nom = "Autre";
    CHECK(produits.front().nom == "Clavier");
    CHECK(!chercher(produits, "Ecran"));
    CHECK(!chercher({}, "Clavier"));
    if (auto p = chercher(produits, "Souris"); p)
        std::cout << p->nom << " : " << p->prix << '\n';
    // Ne jamais faire *resultat sans vérifier sa présence ; value() lève si absent.
}
