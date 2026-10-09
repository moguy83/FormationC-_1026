#include "bibliotheque/catalogue.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
int main() {
    try {
        spdlog::set_level(spdlog::level::debug);
        bibliotheque::Catalogue c;
        c.ajouter({1,"Le Hobbit",{1,"J. R. R. Tolkien"}});
        c.sauvegarder("catalogue.json");
        std::cout<<c.toJson()<<'\n';
    } catch(const std::exception& e) {
        // Capturer par référence préserve le type dynamique ; message au point d'entrée.
        spdlog::error("event=application_echec message={}",e.what()); return 1;
    }
}
