// A. Gestion d'un stock d'articles

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using Stock = std::unordered_map<std::string, int>;

void ajouterArticle(
    Stock& stock,
    const std::string& nom,
    int quantite)
{
    if (quantite <= 0) {
        std::cout << "Quantite invalide\n";
        return;
    }

    stock[nom] += quantite;
}

bool retirerArticle(
    Stock& stock,
    const std::string& nom,
    int quantite)
{
    if (quantite <= 0) {
        return false;
    }

    auto it = stock.find(nom);

    if (it == stock.end()) {
        std::cout << "Article introuvable : "
                  << nom << '\n';
        return false;
    }

    if (it->second < quantite) {
        std::cout << "Stock insuffisant pour "
                  << nom << '\n';
        return false;
    }

    it->second -= quantite;

    if (it->second == 0) {
        std::cout << "RUPTURE : "
                  << nom << '\n';
    }

    return true;
}

void afficherStock(const Stock& stock)
{
    std::cout << "\n--- STOCK ---\n";

    for (const auto& [nom, quantite] : stock) {
        std::cout << nom
                  << " : "
                  << quantite
                  << '\n';
    }
}

void afficherTrieParNom(const Stock& stock)
{
    std::vector<std::pair<std::string, int>> articles(
        stock.begin(),
        stock.end()
    );

    std::sort(
        articles.begin(),
        articles.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        }
    );

    std::cout << "\n--- TRI PAR NOM ---\n";

    for (const auto& [nom, quantite] : articles) {
        std::cout << nom
                  << " : "
                  << quantite
                  << '\n';
    }
}

void afficherTrieParQuantite(const Stock& stock)
{
    std::vector<std::pair<std::string, int>> articles(
        stock.begin(),
        stock.end()
    );

    std::sort(
        articles.begin(),
        articles.end(),
        [](const auto& a, const auto& b) {
            if (a.second == b.second) {
                return a.first < b.first;
            }

            return a.second > b.second;
        }
    );

    std::cout << "\n--- TRI PAR QUANTITE ---\n";

    for (const auto& [nom, quantite] : articles) {
        std::cout << nom
                  << " : "
                  << quantite
                  << '\n';
    }
}

int main()
{
    Stock stock;

    ajouterArticle(stock, "Clavier", 10);
    ajouterArticle(stock, "Souris", 25);
    ajouterArticle(stock, "Ecran", 5);
    ajouterArticle(stock, "Casque", 12);

    ajouterArticle(stock, "Clavier", 3);

    afficherStock(stock);

    retirerArticle(stock, "Ecran", 3);
    retirerArticle(stock, "Casque", 12);
    retirerArticle(stock, "Souris", 100);

    afficherTrieParNom(stock);
    afficherTrieParQuantite(stock);

    return 0;
}

// B. Analyse de texte

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

std::string nettoyerMot(const std::string& mot)
{
    std::string resultat;

    for (unsigned char c : mot) {
        if (std::isalnum(c)) {
            resultat += static_cast<char>(
                std::tolower(c)
            );
        }
    }

    return resultat;
}

int main()
{
    std::ifstream fichier("texte.txt");

    if (!fichier) {
        std::cerr
            << "Impossible d'ouvrir texte.txt\n";
        return 1;
    }

    std::map<std::string, int> frequences;

    std::string mot;

    while (fichier >> mot) {
        mot = nettoyerMot(mot);

        if (!mot.empty()) {
            ++frequences[mot];
        }
    }

    std::vector<std::pair<std::string, int>> classement(
        frequences.begin(),
        frequences.end()
    );

    std::sort(
        classement.begin(),
        classement.end(),
        [](const auto& a, const auto& b) {
            if (a.second == b.second) {
                return a.first < b.first;
            }

            return a.second > b.second;
        }
    );

    std::cout << "--- TOP 10 ---\n";

    std::size_t limite =
        std::min<std::size_t>(
            10,
            classement.size()
        );

    for (std::size_t i = 0; i < limite; ++i) {
        std::cout
            << classement[i].first
            << " : "
            << classement[i].second
            << '\n';
    }

    return 0;
}

// C. Gestion de tickets

#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Ticket
{
    int priorite;
    std::string description;
    unsigned long ordre;
};

struct ComparateurTicket
{
    bool operator()(
        const Ticket& a,
        const Ticket& b
    ) const
    {
        if (a.priorite == b.priorite) {
            return a.ordre > b.ordre;
        }

        return a.priorite < b.priorite;
    }
};

int main()
{
    std::priority_queue<
        Ticket,
        std::vector<Ticket>,
        ComparateurTicket
    > tickets;

    unsigned long prochainOrdre = 0;

    tickets.push({
        2,
        "Probleme imprimante",
        prochainOrdre++
    });

    tickets.push({
        5,
        "Serveur indisponible",
        prochainOrdre++
    });

    tickets.push({
        3,
        "Mot de passe bloque",
        prochainOrdre++
    });

    tickets.push({
        5,
        "Base de donnees inaccessible",
        prochainOrdre++
    });

    while (!tickets.empty()) {

        const Ticket& ticket = tickets.top();

        std::cout
            << "[Priorite "
            << ticket.priorite
            << "] "
            << ticket.description
            << '\n';

        tickets.pop();
    }

    return 0;
}

// D. Analyse de données

#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

int main()
{
    constexpr std::size_t NB_DONNEES = 10'000;
    constexpr std::size_t NB_PLUS_GRANDS = 100;

    std::mt19937 generateur(
        std::random_device{}()
    );

    std::uniform_int_distribution<int>
        distribution(1, 100'000);

    std::vector<int> donnees;

    donnees.reserve(NB_DONNEES);

    for (std::size_t i = 0;
         i < NB_DONNEES;
         ++i) {

        donnees.push_back(
            distribution(generateur)
        );
    }

    // 1. Garder uniquement les nombres pairs

    donnees.erase(
        std::remove_if(
            donnees.begin(),
            donnees.end(),
            [](int valeur) {
                return valeur % 2 != 0;
            }
        ),
        donnees.end()
    );

    // 2. Garder les 100 plus grands

    const std::size_t nb =
        std::min(
            NB_PLUS_GRANDS,
            donnees.size()
        );

    std::partial_sort(
        donnees.begin(),
        donnees.begin() + nb,
        donnees.end(),
        std::greater<int>{}
    );

    donnees.resize(nb);

    // 3. Calculer la moyenne

    long long somme =
        std::accumulate(
            donnees.begin(),
            donnees.end(),
            0LL
        );

    double moyenne =
        donnees.empty()
            ? 0.0
            : static_cast<double>(somme)
                / donnees.size();

    std::cout
        << "Moyenne des "
        << donnees.size()
        << " plus grands nombres pairs : "
        << moyenne
        << "\n\n";

    // 4. Afficher les 10 premiers

    std::cout << "Top 10 :\n";

    const std::size_t nbAffiches =
        std::min<std::size_t>(
            10,
            donnees.size()
        );

    std::for_each(
        donnees.begin(),
        donnees.begin() + nbAffiches,
        [](int valeur) {
            std::cout << valeur << '\n';
        }
    );

    return 0;
}

//E. Comparaison des conteneurs

#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <set>
#include <unordered_set>
#include <vector>

using Horloge =
    std::chrono::high_resolution_clock;

template<typename Fonction>
double mesurerMs(Fonction&& fonction)
{
    auto debut = Horloge::now();

    fonction();

    auto fin = Horloge::now();

    return std::chrono::duration<double, std::milli>(
        fin - debut
    ).count();
}

int main()
{
    constexpr int NB_INSERTIONS = 1'000'000;
    constexpr int NB_RECHERCHES = 10'000;

    std::vector<int> valeurs(NB_INSERTIONS);

    std::iota(
        valeurs.begin(),
        valeurs.end(),
        0
    );

    std::mt19937 generateur(
        std::random_device{}()
    );

    std::shuffle(
        valeurs.begin(),
        valeurs.end(),
        generateur
    );

    std::vector<int> recherches;

    recherches.reserve(NB_RECHERCHES);

    std::uniform_int_distribution<int>
        distribution(
            0,
            NB_INSERTIONS - 1
        );

    for (int i = 0;
         i < NB_RECHERCHES;
         ++i) {

        recherches.push_back(
            distribution(generateur)
        );
    }

    std::vector<int> v;
    std::list<int> l;
    std::deque<int> d;
    std::set<int> s;
    std::unordered_set<int> us;

    double tempsVector = mesurerMs([&]() {
        for (int x : valeurs) {
            v.push_back(x);
        }
    });

    double tempsList = mesurerMs([&]() {
        for (int x : valeurs) {
            l.push_back(x);
        }
    });

    double tempsDeque = mesurerMs([&]() {
        for (int x : valeurs) {
            d.push_back(x);
        }
    });

    double tempsSet = mesurerMs([&]() {
        for (int x : valeurs) {
            s.insert(x);
        }
    });

    double tempsUnorderedSet = mesurerMs([&]() {
        for (int x : valeurs) {
            us.insert(x);
        }
    });

    std::cout << "--- INSERTION ---\n";

    std::cout
        << "vector        : "
        << tempsVector << " ms\n";

    std::cout
        << "list          : "
        << tempsList << " ms\n";

    std::cout
        << "deque         : "
        << tempsDeque << " ms\n";

    std::cout
        << "set           : "
        << tempsSet << " ms\n";

    std::cout
        << "unordered_set : "
        << tempsUnorderedSet << " ms\n";

    std::size_t trouves = 0;

    double rechercheVector = mesurerMs([&]() {
        for (int x : recherches) {
            if (
                std::find(
                    v.begin(),
                    v.end(),
                    x
                ) != v.end()
            ) {
                ++trouves;
            }
        }
    });

    double rechercheList = mesurerMs([&]() {
        for (int x : recherches) {
            if (
                std::find(
                    l.begin(),
                    l.end(),
                    x
                ) != l.end()
            ) {
                ++trouves;
            }
        }
    });

    double rechercheDeque = mesurerMs([&]() {
        for (int x : recherches) {
            if (
                std::find(
                    d.begin(),
                    d.end(),
                    x
                ) != d.end()
            ) {
                ++trouves;
            }
        }
    });

    double rechercheSet = mesurerMs([&]() {
        for (int x : recherches) {
            if (s.find(x) != s.end()) {
                ++trouves;
            }
        }
    });

    double rechercheUnorderedSet =
        mesurerMs([&]() {
            for (int x : recherches) {
                if (us.find(x) != us.end()) {
                    ++trouves;
                }
            }
        });

    std::cout << "\n--- RECHERCHE ---\n";

    std::cout
        << "vector        : "
        << rechercheVector << " ms\n";

    std::cout
        << "list          : "
        << rechercheList << " ms\n";

    std::cout
        << "deque         : "
        << rechercheDeque << " ms\n";

    std::cout
        << "set           : "
        << rechercheSet << " ms\n";

    std::cout
        << "unordered_set : "
        << rechercheUnorderedSet
        << " ms\n";

    std::cout
        << "\nTrouves : "
        << trouves
        << '\n';

    return 0;
}