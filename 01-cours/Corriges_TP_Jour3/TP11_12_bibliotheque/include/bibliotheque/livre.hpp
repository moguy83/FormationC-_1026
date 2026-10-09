#pragma once
#include "bibliotheque/auteur.hpp"
#include <string>
namespace bibliotheque {
/// Entité simple ; valider() centralise les invariants métier.
struct Livre {int id; std::string titre; Auteur auteur; void valider() const;};
}
