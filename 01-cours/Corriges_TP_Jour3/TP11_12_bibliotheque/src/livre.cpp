#include "bibliotheque/livre.hpp"
#include <stdexcept>
namespace bibliotheque {
void Livre::valider() const {
    if(id<=0||titre.empty()) throw std::invalid_argument("Livre invalide");
    auteur.valider();
}
}
