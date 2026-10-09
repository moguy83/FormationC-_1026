#pragma once
#include "bibliotheque/catalogue.hpp"
#include <memory>
#include <string>
namespace bibliotheque {
// TP12 : adaptateur de persistance PostgreSQL d'un instantané du Catalogue du TP11.
// Pimpl masque libpq dans l'API publique et limite les recompilations.
class CatalogueBDD {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    explicit CatalogueBDD(const std::string& connexion);
    ~CatalogueBDD(); // Défini dans le .cpp : Impl doit y être complet.
    void sauvegarder(const Catalogue& catalogue);
    Catalogue charger() const;
    int backendId() const; // Sert au test de perte réelle de connexion.
};
}
