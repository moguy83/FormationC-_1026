#pragma once
#include "bibliotheque/livre.hpp"
#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
namespace bibliotheque {
/// Catalogue en mémoire, avec instantanés JSON. Pas de dépendance JSON dans le header public.
class Catalogue {
    std::map<int,Livre> livres_;
public:
    void ajouter(Livre livre);
    const Livre& lire(int id) const;
    void modifier(Livre livre);
    void supprimer(int id);
    std::size_t size() const noexcept;
    std::string toJson() const;
    void fromJson(const std::string& texte);
    void sauvegarder(const std::filesystem::path& path) const;
    void charger(const std::filesystem::path& path);
};
}
