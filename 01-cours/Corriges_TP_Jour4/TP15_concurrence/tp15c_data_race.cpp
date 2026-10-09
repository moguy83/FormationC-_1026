#include <atomic>
#include <iostream>
#include <thread>
#include "test.hpp"
int main() {
#ifdef DEMO_RACE
    // VERSION VOLONTAIREMENT INCORRECTE, compilée uniquement avec l'option dédiée.
    // compteur++ = lecture + calcul + écriture, pas une opération atomique.
    // Même un résultat parfois correct ne supprime pas le comportement indéfini.
    int compteur = 0;
#else
    std::atomic<int> compteur{0};
#endif
    auto travail = [&] {
        for (int i = 0; i < 100000; ++i) {
#ifdef DEMO_RACE
            ++compteur;
#else
            // relaxed suffit : seul le nombre nous intéresse, aucune autre donnée à publier.
            compteur.fetch_add(1, std::memory_order_relaxed);
#endif
        }
    };
    std::jthread a(travail), b(travail);
    a.join();
    b.join();
#ifdef DEMO_RACE
    std::cout << "Version fautive : " << compteur << " (aucune valeur garantie)\n";
#else
    CHECK(compteur.load() == 200000);
    std::cout << "Version corrigee : " << compteur.load() << '\n';
#endif
}
