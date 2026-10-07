// A. Buffer avec move semantics

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

class Buffer {
private:
    std::uint8_t* data_;
    std::size_t taille_;

public:
    // Constructeur
    explicit Buffer(std::size_t taille)
        : data_(taille > 0 ? new std::uint8_t[taille] : nullptr),
          taille_(taille)
    {
        std::cout << "Constructeur (" << taille_ << " octets)\n";
    }

    // Constructeur de copie : copie profonde
    Buffer(const Buffer& autre)
        : data_(autre.taille_ > 0
                    ? new std::uint8_t[autre.taille_]
                    : nullptr),
          taille_(autre.taille_)
    {
        std::copy_n(autre.data_, taille_, data_);
        std::cout << "Constructeur de copie\n";
    }

    // Opérateur d'affectation par copie
    Buffer& operator=(const Buffer& autre)
    {
        std::cout << "Affectation par copie\n";

        if (this != &autre) {
            // On alloue d'abord pour éviter de perdre
            // l'ancien Buffer en cas d'échec.
            std::uint8_t* nouveau =
                autre.taille_ > 0
                    ? new std::uint8_t[autre.taille_]
                    : nullptr;

            std::copy_n(autre.data_, autre.taille_, nouveau);

            delete[] data_;

            data_ = nouveau;
            taille_ = autre.taille_;
        }

        return *this;
    }

    // Constructeur de déplacement
    Buffer(Buffer&& autre) noexcept
        : data_(autre.data_),
          taille_(autre.taille_)
    {
        std::cout << "Constructeur de déplacement\n";

        autre.data_ = nullptr;
        autre.taille_ = 0;
    }

    // Opérateur d'affectation par déplacement
    Buffer& operator=(Buffer&& autre) noexcept
    {
        std::cout << "Affectation par déplacement\n";

        if (this != &autre) {
            delete[] data_;

            data_ = autre.data_;
            taille_ = autre.taille_;

            autre.data_ = nullptr;
            autre.taille_ = 0;
        }

        return *this;
    }

    // Destructeur
    ~Buffer()
    {
        delete[] data_;
        std::cout << "Destructeur\n";
    }

    std::size_t taille() const
    {
        return taille_;
    }

    std::uint8_t& operator[](std::size_t index)
    {
        return data_[index];
    }

    const std::uint8_t& operator[](std::size_t index) const
    {
        return data_[index];
    }
};

int main()
{
    Buffer a(10);

    a[0] = 42;
    a[1] = 100;

    std::cout << "\n--- Copie ---\n";

    Buffer b = a;

    std::cout << "a[0] = "
              << static_cast<int>(a[0])
              << '\n';

    std::cout << "b[0] = "
              << static_cast<int>(b[0])
              << '\n';

    // Vérification de la copie profonde
    b[0] = 99;

    std::cout << "\nAprès modification de b :\n";

    std::cout << "a[0] = "
              << static_cast<int>(a[0])
              << '\n';

    std::cout << "b[0] = "
              << static_cast<int>(b[0])
              << '\n';

    std::cout << "\n--- Déplacement ---\n";

    Buffer c = std::move(a);

    std::cout << "taille de a = "
              << a.taille()
              << '\n';

    std::cout << "taille de c = "
              << c.taille()
              << '\n';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B.
 
// #include <iostream>
// #include <memory>
// #include <string>
// #include <vector>

// class Noeud
//     : public std::enable_shared_from_this<Noeud>
// {
// private:
//     std::string nom_;

//     std::vector<std::shared_ptr<Noeud>> enfants_;

//     std::weak_ptr<Noeud> parent_;

// public:
//     explicit Noeud(std::string nom)
//         : nom_(std::move(nom))
//     {
//         std::cout
//             << "Construction de "
//             << nom_
//             << '\n';
//     }

//     ~Noeud()
//     {
//         std::cout
//             << "Destruction de "
//             << nom_
//             << '\n';
//     }

//     void ajouterEnfant(
//         const std::shared_ptr<Noeud>& enfant)
//     {
//         enfants_.push_back(enfant);

//         enfant->parent_ = shared_from_this();
//     }

//     void afficher() const
//     {
//         std::cout << nom_;

//         if (auto p = parent_.lock()) {
//             std::cout
//                 << " (parent : "
//                 << p->nom_
//                 << ")";
//         } else {
//             std::cout << " (racine)";
//         }

//         std::cout << '\n';

//         for (const auto& enfant : enfants_) {
//             enfant->afficher();
//         }
//     }
// };

// int main()
// {
//     {
//         auto racine =
//             std::make_shared<Noeud>("Racine");

//         auto a =
//             std::make_shared<Noeud>("A");

//         auto b =
//             std::make_shared<Noeud>("B");

//         auto c =
//             std::make_shared<Noeud>("C");

//         racine->ajouterEnfant(a);
//         racine->ajouterEnfant(b);

//         a->ajouterEnfant(c);

//         racine->afficher();
//     }

//     std::cout << "Fin du programme\n";
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C. Exercice C — Remplacer les new/delete

// ancien code
// class Objet {
// public:
//     void travailler()
//     {
//         std::cout << "Travail\n";
//     }
// };

// void ancienCode()
// {
//     Objet* objet = new Objet();

//     objet->travailler();

//     delete objet;
// }

// // c+ moderne
// void nouveauCode()
// {
//     auto objet = std::make_unique<Objet>();

//     objet->travailler();
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D. fuites memoires, double delete, dereference null pointer
// void fonctionBuguee() {
//     int* a = new int(5);
//     int* b = new int[10];
//     int* c = nullptr;

//     *c = 42;               // Bug 1
//     delete b;              // Bug 2
//     delete a;
//     delete a;              // Bug 3

//     Ressource* r = new Ressource();
//     if (!r->initialiser()) return;  // Bug 4

//     r->utiliser();
//     delete r;
}

// version modene corrige

// #include <array>
// #include <memory>

// void fonctionCorrigee()
// {
//     int a = 5;

//     std::array<int, 10> b{};

//     int c = 42;

//     auto r =
//         std::make_unique<Ressource>();

//     if (!r->initialiser()) {
//         return;
//     }

//     r->utiliser();
// }

// int main()
// {
//     fonctionBuguee();
//     // fonctionCorrigee();
// }