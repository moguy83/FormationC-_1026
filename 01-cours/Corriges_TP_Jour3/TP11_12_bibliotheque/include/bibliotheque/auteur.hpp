#pragma once
#include <string>
namespace bibliotheque {
/// Auteur associé à un livre ; identifiant positif et nom non vide.
struct Auteur {int id; std::string nom; void valider() const;};
}
