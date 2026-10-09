#include "bibliotheque/auteur.hpp"
#include <stdexcept>
namespace bibliotheque {
void Auteur::valider() const {
    if(id<=0||nom.empty()) throw std::invalid_argument("Auteur invalide");
}
}
