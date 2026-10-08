# 📘 Exercices C++ — Jour 5 : Projet de Synthèse & Consolidation
**Formation C++ · 5 jours · Niveau débutant–intermédiaire**

---

## 📋 Sommaire

- [Objectifs du jour](#objectifs)
- [⚡ Révision express](#révision) *(~30 min)*
  - [R.1 Quiz C++ — 20 questions](#r1-quiz)
  - [R.2 Code review — trouver les bugs](#r2-code-review)
  - [R.3 Comparatif de solutions](#r3-comparatif)
- [🔥 Exercices de consolidation](#consolidation) *(~90 min)*
  - [C.1 Refactoring guidé](#c1-refactoring)
  - [C.2 Debugging avancé](#c2-debugging)
  - [C.3 Optimisation mesurée](#c3-optimisation)
  - [C.4 Tests et couverture](#c4-tests)
- [🏆 Projet de synthèse — LibraryManager](#projet-synthese)
  - [Cahier des charges complet](#cahier-des-charges)
  - [Architecture recommandée](#architecture)
  - [Planning de développement](#planning)
  - [Critères de soutenance](#critères)
- [🎓 Exercices bonus — Pour aller plus loin](#bonus)
- [💡 Aide à la conception](#aide-conception)
- [✅ Grille d'auto-évaluation finale](#auto-évaluation)

---

## 🎯 Objectifs du jour {#objectifs}

Le Jour 5 est centré sur l'intégration de tous les concepts. Vous allez :
- Consolider les apprentissages des 4 premiers jours par la pratique
- Concevoir et implémenter un projet complet en autonomie
- Appliquer les bonnes pratiques de développement professionnel
- Préparer et présenter votre travail devant le groupe

---

## ⚡ Révision express {#révision}

---

### R.1 Quiz C++ — 20 questions {#r1-quiz}

Répondez sans consulter vos notes. Temps : 15 minutes.

**Mémoire et RAII**

1. Quelle est la différence entre `unique_ptr` et `shared_ptr` ? Quand utiliser l'un plutôt que l'autre ?
2. Que signifie RAII et pourquoi est-ce important ?
3. Que se passe-t-il si vous oubliez `delete[]` après `new int[10]` ?
4. Que fait `std::move(v)` sur un `std::vector` ? Que contient `v` après ?

**STL**

5. Quelle est la complexité de `std::map::find()` vs `std::unordered_map::find()` ?
6. Pourquoi `std::vector` est-il généralement préféré à `std::list` ?
7. Qu'est-ce que l'idiome erase-remove ? Écrivez l'exemple pour supprimer les pairs d'un vector.
8. Quelle est la différence entre `std::sort` et `std::stable_sort` ?

**Templates et Generics**

9. Quelle est la différence entre `template<typename T>` et `template<class T>` ?
10. Qu'est-ce que SFINAE ? Donnez un exemple d'utilisation.
11. Comment définir un concept C++20 qui accepte uniquement les types numériques ?

**Concurrence**

12. Qu'est-ce qu'un data race ? Comment le détecter ?
13. Quelle est la différence entre `lock_guard` et `unique_lock` ?
14. Quand utiliser `std::atomic<int>` plutôt qu'un `mutex` ?
15. Que fait `std::async(std::launch::deferred, fn)` ?

**Sécurité**

16. Pourquoi `strcpy` est-il dangereux ? Quelle est l'alternative en C++ moderne ?
17. Comment fonctionne une attaque SQL injection ? Comment s'en protéger ?
18. Quand utiliser `-fsanitize=address` vs `-fsanitize=thread` ?

**Architecture**

19. Décrivez le pattern CRTP en une phrase et donnez un cas d'usage.
20. Quelle est la différence entre composition et héritage ? Quand préférer l'un à l'autre ?

---

### R.2 Code Review — trouver les bugs {#r2-code-review}

**Code 1 : Classe de gestion de ressources**

Identifiez tous les problèmes (au moins 6) :

```cpp
class GestionnaireFichiers {
    std::vector<FILE*> fichiers;

public:
    void ouvrirFichier(const std::string& path) {
        FILE* f = fopen(path.c_str(), "r");
        fichiers.push_back(f);
    }

    std::string lire(int index) {
        char buffer[256];
        fread(buffer, 1, 256, fichiers[index]);
        return std::string(buffer);
    }

    void fermerTous() {
        for (FILE* f : fichiers) fclose(f);
    }

    ~GestionnaireFichiers() { /* vide */ }
};

// Utilisation
GestionnaireFichiers g;
g.ouvrirFichier("/etc/passwd");
g.ouvrirFichier("inexistant.txt");
std::cout << g.lire(0) << g.lire(1);
// Pas d'appel à fermerTous()
```

---

**Code 2 : Concurrence**

Identifiez les problèmes de threading (au moins 4) :

```cpp
class Cache {
    std::map<std::string, std::string> data;
    std::mutex mtx;
    std::thread nettoyeur;

public:
    Cache() {
        nettoyeur = std::thread([this]() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(60));
                std::lock_guard<std::mutex> lock(mtx);
                data.clear();
            }
        });
        nettoyeur.detach();  // Problem ?
    }

    std::string get(const std::string& cle) {
        std::lock_guard<std::mutex> lock(mtx);
        return data.at(cle);  // Problem ?
    }

    void set(const std::string& cle, std::string val) {
        // Pas de lock ici ?
        data[cle] = val;
    }

    bool contains(const std::string& cle) {
        std::lock_guard<std::mutex> lock(mtx);
        return data.count(cle) > 0;
    }

    // TOCTOU bug :
    if (cache.contains("user")) {
        auto val = cache.get("user");  // data race possible ?
    }
};
```

---

### R.3 Comparatif de solutions {#r3-comparatif}

**Exercice** : Implémenter la même fonctionnalité de 3 façons différentes et comparer.

**Fonctionnalité** : Transformer une collection de `std::string` en une map fréquence de mots.

```cpp
std::vector<std::string> textes = { "le chat", "le chien", "le chat chat" };
// Résultat attendu :
// { "le": 3, "chat": 3, "chien": 1 }
```

Implémentez les 3 versions et comparez leur lisibilité, performance et maintenabilité :

```cpp
// Version 1 : Boucles for traditionnelles
std::map<std::string, int> compterV1(const std::vector<std::string>& textes);

// Version 2 : Algorithmes STL
std::map<std::string, int> compterV2(const std::vector<std::string>& textes);

// Version 3 : Ranges C++20
std::map<std::string, int> compterV3(const std::vector<std::string>& textes);
```

---

## 🔥 Exercices de consolidation {#consolidation}

---

### C.1 Refactoring guidé {#c1-refactoring}

**Modernisez ce code C++98 en C++17 :**

```cpp
// AVANT : code C++98 (200 lignes) — à refactorer

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

typedef std::pair<std::string, int> PaireNomAge;
typedef std::vector<PaireNomAge> ListePersonnes;

bool compareParAge(const PaireNomAge& a, const PaireNomAge& b) {
    return a.second < b.second;
}

bool compareParNom(const PaireNomAge& a, const PaireNomAge& b) {
    return a.first < b.first;
}

ListePersonnes* creerListe() {
    return new ListePersonnes();
}

void ajouterPersonne(ListePersonnes* liste,
                     const std::string& nom, int age) {
    if (liste == NULL) return;
    if (nom.empty()) return;
    if (age < 0 || age > 150) return;
    liste->push_back(std::make_pair(nom, age));
}

PaireNomAge* trouverPlusVieux(ListePersonnes* liste) {
    if (liste == NULL || liste->empty()) return NULL;
    ListePersonnes::iterator it = liste->begin();
    PaireNomAge* plusVieux = &(*it);
    for (; it != liste->end(); ++it) {
        if (it->second > plusVieux->second)
            plusVieux = &(*it);
    }
    return plusVieux;
}

void afficher(const ListePersonnes* liste) {
    if (liste == NULL) return;
    for (size_t i = 0; i < liste->size(); ++i) {
        std::cout << (*liste)[i].first << " : "
                  << (*liste)[i].second << "\n";
    }
}

int main() {
    ListePersonnes* liste = creerListe();
    ajouterPersonne(liste, "Alice", 30);
    ajouterPersonne(liste, "Bob", 25);
    ajouterPersonne(liste, "Charlie", 35);

    std::sort(liste->begin(), liste->end(), compareParAge);
    afficher(liste);

    PaireNomAge* plusVieux = trouverPlusVieux(liste);
    if (plusVieux)
        std::cout << "Plus vieux : " << plusVieux->first << "\n";

    delete liste;
}
```

**Checklist de modernisation** :
- [ ] `typedef` → `using`
- [ ] `NULL` → `nullptr`
- [ ] Raw pointers → smart pointers ou valeurs
- [ ] `new`/`delete` → automatique (RAII)
- [ ] Fonctions de comparaison → lambdas
- [ ] Boucles for → range-based for
- [ ] `std::pair` → `struct` nommée ou `auto`
- [ ] `std::make_pair` → brace initialization `{nom, age}`
- [ ] `std::optional` pour les valeurs manquantes

---

### C.2 Debugging avancé {#c2-debugging}

**Exercice de débogage systématique**

Le programme suivant crashe avec "Segmentation fault". Trouvez et corrigez tous les bugs :

```cpp
#include <vector>
#include <string>
#include <memory>
#include <iostream>

class Noeud {
public:
    std::string valeur;
    std::vector<Noeud*> enfants;

    Noeud(const std::string& v) : valeur(v) {}

    void ajouterEnfant(Noeud* enfant) {
        enfants.push_back(enfant);
    }

    void afficher(int profondeur = 0) {
        std::cout << std::string(profondeur * 2, ' ')
                  << valeur << "\n";
        for (Noeud* enfant : enfants)
            enfant->afficher(profondeur + 1);
    }
};

class Arbre {
    Noeud* racine;

public:
    Arbre() : racine(nullptr) {}

    void setRacine(const std::string& val) {
        racine = new Noeud(val);
    }

    void ajouter(const std::string& parent, const std::string& enfant) {
        Noeud* p = trouver(racine, parent);
        p->ajouterEnfant(new Noeud(enfant));
    }

    Noeud* trouver(Noeud* noeud, const std::string& val) {
        if (noeud->valeur == val) return noeud;
        for (Noeud* enfant : noeud->enfants) {
            Noeud* res = trouver(enfant, val);
            if (res) return res;
        }
        return nullptr;
    }

    void afficher() { racine->afficher(); }
};

int main() {
    Arbre arbre;
    // setRacine oublié !
    arbre.ajouter("Racine", "Enfant1");
    arbre.ajouter("Racine", "Enfant2");
    arbre.ajouter("Enfant1", "Petit-enfant1");
    arbre.afficher();
    // Pas de libération mémoire
}
```

Processus recommandé :
1. Compiler avec `-g -fsanitize=address`
2. Lire le rapport ASan
3. Corriger les problèmes un par un
4. Refactoriser avec `unique_ptr`

---

### C.3 Optimisation mesurée {#c3-optimisation}

**Exercice : cycle mesurer → optimiser → valider**

```cpp
// Fonction à optimiser : recherche par nom dans une liste d'employés
struct Employe {
    int id;
    std::string prenom;
    std::string nom;
    std::string departement;
    double salaire;
    std::string email;
};

class RHSysteme {
    std::vector<Employe> employes;  // 50 000 employés

public:
    // Ces fonctions sont appelées 10 000 fois par seconde
    std::optional<Employe> trouverParId(int id) const;
    std::vector<Employe> parDepartement(const std::string& dept) const;
    std::vector<Employe> parTrancheSalaire(double min, double max) const;
    std::vector<std::string> emailsParDepartement(const std::string& dept) const;
};
```

**Processus** :
1. Mesurer les performances initiales avec Google Benchmark
2. Profiler avec `perf record` + `perf report` (ou `callgrind`)
3. Identifier les 2 opérations les plus lentes
4. Proposer une structure de données optimisée (index)
5. Implémenter et mesurer le speedup
6. Documenter le trade-off mémoire/temps

**Critères de succès** :
- `trouverParId` : de O(n) à O(1) avec speedup ≥ 100×
- `parDepartement` : speedup ≥ 10× avec index
- Mémoire supplémentaire documentée et justifiée

---

### C.4 Tests et couverture {#c4-tests}

**Exercice : atteindre 90% de couverture**

Partez de cette classe et écrivez les tests nécessaires pour atteindre 90%+ de couverture :

```cpp
class Validateur {
public:
    struct Resultat {
        bool valide;
        std::string erreur;
        std::string valeurNormalisee;
    };

    Resultat validerEmail(const std::string& email) const;
    Resultat validerMotDePasse(const std::string& mdp) const;
    // Règles MDP : 8+ chars, 1 maj, 1 min, 1 chiffre, 1 spécial

    Resultat validerTelephone(const std::string& tel) const;
    // Accepte : 0612345678, 06 12 34 56 78, +33612345678, (33)612345678

    Resultat validerDateNaissance(const std::string& date) const;
    // Format : DD/MM/YYYY, âge entre 0 et 120 ans

    Resultat validerIBAN(const std::string& iban) const;
    // Validation du checksum selon l'algorithme MOD97
};
```

**Contraintes** :
- Utiliser Catch2 avec GENERATE pour les tests paramétrés
- Couvrir obligatoirement : cas valides, cas invalides, cas limites
- Générer le rapport avec `gcovr --html-details coverage.html`
- Atteindre ≥ 90% de couverture de lignes ET de branches

---

## 🏆 Projet de synthèse — LibraryManager {#projet-synthese}

---

### Cahier des charges complet {#cahier-des-charges}

**Système de gestion de bibliothèque numérique**

#### Entités métier

```cpp
// Livre
struct ISBN { std::string valeur; };  // Value Object validé

struct Livre {
    ISBN isbn;
    std::string titre;
    std::string auteur;
    int anneePublication;
    std::string genre;
    bool disponible;
    std::optional<std::string> description;
    std::vector<std::string> tags;
};

// Membre
struct Membre {
    int id;
    std::string prenom, nom;
    std::string email;
    std::chrono::system_clock::time_point dateInscription;
    int nbEmpruntsActifs;
    static constexpr int MAX_EMPRUNTS = 5;
};

// Emprunt
struct Emprunt {
    int id;
    ISBN isbn;
    int membreId;
    std::chrono::system_clock::time_point dateEmprunt;
    std::chrono::system_clock::time_point dateRetourPrevue;   // +14 jours
    std::optional<std::chrono::system_clock::time_point> dateRetourReelle;

    bool estEnRetard() const;
    double calculerPenalite() const;  // 0.20€/jour de retard
};
```

#### Fonctionnalités obligatoires (MVP)

| Fonctionnalité | Priorité | Complexité |
|---|---|---|
| Ajouter/modifier/supprimer un livre | P1 | Faible |
| Rechercher livres (titre, auteur, genre, tag) | P1 | Moyenne |
| Inscrire/modifier un membre | P1 | Faible |
| Emprunter un livre | P1 | Moyenne |
| Retourner un livre | P1 | Faible |
| Voir les emprunts en cours | P1 | Faible |
| Voir les livres en retard | P1 | Faible |
| Persistance JSON | P1 | Moyenne |

#### Fonctionnalités optionnelles (bonus)

| Fonctionnalité | Points bonus |
|---|---|
| Persistance SQLite | +10 pts |
| Recherche full-text (index inversé) | +8 pts |
| Cache LRU thread-safe | +6 pts |
| API REST (cpp-httplib) | +10 pts |
| Interface graphique (ncurses) | +8 pts |
| Export PDF du relevé d'emprunts | +5 pts |

---

### Architecture recommandée {#architecture}

```
LibraryManager/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── Doxyfile
├── .github/workflows/ci.yml
├── .clang-format
├── .clang-tidy
│
├── include/library/
│   ├── domain/
│   │   ├── livre.hpp          ← Entités et Value Objects
│   │   ├── membre.hpp
│   │   └── emprunt.hpp
│   ├── ports/
│   │   ├── i_livre_repo.hpp   ← Interfaces Repository
│   │   ├── i_membre_repo.hpp
│   │   └── i_emprunt_repo.hpp
│   ├── usecases/
│   │   ├── emprunter_livre.hpp
│   │   ├── retourner_livre.hpp
│   │   └── rechercher_livres.hpp
│   └── exceptions.hpp
│
├── src/
│   ├── domain/                ← Implémentation des entités
│   ├── adapters/
│   │   ├── json_repository/   ← Persistance JSON
│   │   └── sqlite_repository/ ← Persistance SQLite (optionnel)
│   ├── usecases/              ← Logique métier
│   └── main.cpp
│
└── tests/
    ├── unit/
    │   ├── test_livre.cpp
    │   ├── test_emprunt.cpp
    │   └── test_usecases.cpp
    └── integration/
        └── test_repository.cpp
```

---

### Planning de développement {#planning}

| Phase | Durée | Livrables |
|---|---|---|
| **Phase 0** : Setup | 20 min | Projet CMake, git init, CI de base |
| **Phase 1** : Domaine | 40 min | Entités, Value Objects, Exceptions |
| **Phase 2** : Interfaces | 20 min | Interfaces Repository (ports) |
| **Phase 3** : Use Cases | 40 min | Emprunter, Retourner, Rechercher |
| **Phase 4** : Persistance | 45 min | JSON Repository |
| **Phase 5** : CLI | 30 min | Interface ligne de commande |
| **Phase 6** : Tests | 45 min | 30+ tests, couverture 80% |
| **Phase 7** : Qualité | 20 min | clang-tidy, Valgrind, Doxygen |
| **Phase 8** : Soutenance | 10 min | Demo + présentation |

---

### Critères de soutenance {#critères}

#### Évaluation technique (70 points)

| Critère | Vérification | Points |
|---|---|---|
| **Compilation** | `cmake --build` sans warning `-Wall -Wextra` | 10 |
| **Tests** | `ctest` 100% vert, ≥ 30 tests | 15 |
| **Couverture** | gcovr ≥ 80% lignes | 10 |
| **Mémoire** | Valgrind 0 fuite, 0 erreur | 10 |
| **Sanitizers** | ASan + UBSan 0 erreur | 5 |
| **Statique** | clang-tidy 0 warning | 5 |
| **Architecture** | Ports & Adapters, SOLID | 15 |

#### Qualité logicielle (20 points)

| Critère | Description | Points |
|---|---|---|
| **Lisibilité** | Code auto-documenté, nommage clair | 8 |
| **Documentation** | Doxygen complet + README | 7 |
| **CI/CD** | Pipeline GitHub Actions fonctionnel | 5 |

#### Présentation (10 points)

| Critère | Description | Points |
|---|---|---|
| **Démo** | Toutes les fonctionnalités démontrées | 5 |
| **Explication** | Justification des choix techniques | 5 |

---

## 🎓 Exercices bonus — Pour aller plus loin {#bonus}

---

**Bonus 1 : Corroutines C++20**

```cpp
// Implémenter un générateur lazy de livres paginés
std::generator<std::vector<Livre>> paginateLivres(
    const ILivreRepository& repo, size_t pageSize) {

    size_t offset = 0;
    while (true) {
        auto page = repo.chargerPage(offset, pageSize);
        if (page.empty()) co_return;
        co_yield page;
        offset += pageSize;
    }
}

// Utilisation
for (const auto& page : paginateLivres(repo, 20)) {
    afficherPage(page);
    if (utilisateurQuitte()) break;
}
```

---

**Bonus 2 : REST API avec cpp-httplib**

```cpp
#include <httplib.h>

class LibraryAPI {
    httplib::Server srv;
    Catalogue& catalogue;

public:
    LibraryAPI(Catalogue& cat) : catalogue(cat) {
        // GET /api/livres
        srv.Get("/api/livres", [this](const httplib::Request& req,
                                      httplib::Response& res) {
            auto livres = catalogue.tousLivres();
            res.set_content(serialiserLivres(livres), "application/json");
        });

        // POST /api/livres
        srv.Post("/api/livres", /* ... */);

        // POST /api/emprunts
        srv.Post("/api/emprunts", /* ... */);
    }

    void lancer(int port = 8080) { srv.listen("0.0.0.0", port); }
};
```

---

**Bonus 3 : CRTP pour les entités**

```cpp
// Base CRTP pour les entités persistables
template<typename Derived>
class Entite {
public:
    // Méthode clone typée sans cast
    Derived clone() const {
        return *static_cast<const Derived*>(this);
    }

    // Comparaison par ID (si Derived a un getid())
    bool operator==(const Derived& o) const {
        return static_cast<const Derived*>(this)->getId()
               == o.getId();
    }

    // Sérialisation automatique si Derived implémente serialize()
    std::string toJSON() const {
        return static_cast<const Derived*>(this)->serialize();
    }
};

class Livre : public Entite<Livre> {
    // ...
};
```

---

## 💡 Aide à la conception {#aide-conception}

### Checklist avant de coder

Avant d'écrire la première ligne de code :

- [ ] **Comprendre le domaine** : lire le cahier des charges en entier
- [ ] **Identifier les entités** : quels objets du monde réel modéliser ?
- [ ] **Identifier les opérations** : que doit faire le système ?
- [ ] **Identifier les interfaces** : quelles dépendances abstraire ?
- [ ] **Dessiner le diagramme de classes** (même rapidement à la main)
- [ ] **Définir les exceptions** : quels cas d'erreur traiter ?
- [ ] **Penser aux tests** : comment vais-je tester cette classe ?

### Template de classe bien conçue

```cpp
// Checklist pour chaque classe
class MaClasse {
    // ✅ Attributs privés avec noms significatifs
    // ✅ Constructeur avec validation (throw si invalide)
    // ✅ Rule of Zero OU Rule of Five (jamais entre-deux)
    // ✅ Méthodes const sur les méthodes de lecture
    // ✅ Paramètres par const& pour les types complexes
    // ✅ [[nodiscard]] sur les méthodes qui retournent une valeur importante
    // ✅ noexcept sur les opérations qui ne peuvent pas échouer
    // ✅ Destructeur virtual si utilisé comme classe de base
};
```

### Erreurs de débutants à éviter

| Erreur | Correction |
|---|---|
| `using namespace std;` dans un header | Toujours utiliser `std::` dans les headers |
| Retourner un pointeur vers une variable locale | Retourner par valeur ou utiliser `std::optional` |
| Boucle sans `const auto&` sur un vecteur d'objets | `for (const auto& elem : v)` |
| Comparer deux `double` avec `==` | Utiliser `std::abs(a-b) < epsilon` |
| Oublier `override` sur les méthodes virtuelles | Toujours ajouter `override` |
| Passer un `std::string` par valeur en paramètre | Passer par `const std::string&` |
| Utiliser `int` pour des tailles/indices | Utiliser `size_t` ou `int64_t` |
| Ignorer la valeur de retour d'une fonction | Ajouter `[[nodiscard]]` et gérer le retour |

---

## ✅ Grille d'auto-évaluation finale {#auto-évaluation}

### Compétences techniques — Bilan formation

Évaluez-vous honnêtement (1 = à retravailler, 3 = maîtrisé) :

#### Jour 1 — Fondamentaux
| Compétence | 1 | 2 | 3 |
|---|---|---|---|
| Types, variables, auto, constexpr | ☐ | ☐ | ☐ |
| Fonctions, surcharge, références | ☐ | ☐ | ☐ |
| Classes, encapsulation, constructeurs | ☐ | ☐ | ☐ |
| Héritage, polymorphisme, virtual | ☐ | ☐ | ☐ |

#### Jour 2 — Mémoire & STL
| Compétence | 1 | 2 | 3 |
|---|---|---|---|
| RAII, smart pointers (unique/shared/weak) | ☐ | ☐ | ☐ |
| Move semantics, std::move | ☐ | ☐ | ☐ |
| Conteneurs STL (vector, map, set, unordered_*) | ☐ | ☐ | ☐ |
| Algorithmes STL + lambdas | ☐ | ☐ | ☐ |
| Design Patterns (Factory, Observer, Strategy) | ☐ | ☐ | ☐ |

#### Jour 3 — Templates & Intégration
| Compétence | 1 | 2 | 3 |
|---|---|---|---|
| Templates de fonctions et classes | ☐ | ☐ | ☐ |
| Concepts C++20 | ☐ | ☐ | ☐ |
| Exceptions et garanties d'exception | ☐ | ☐ | ☐ |
| CMake multi-modules | ☐ | ☐ | ☐ |
| Pipeline CI/CD GitHub Actions | ☐ | ☐ | ☐ |

#### Jour 4 — Moderne & Sécurité
| Compétence | 1 | 2 | 3 |
|---|---|---|---|
| Lambdas et closures | ☐ | ☐ | ☐ |
| std::optional, variant, any | ☐ | ☐ | ☐ |
| Sécurité mémoire (buffer overflow, UAF) | ☐ | ☐ | ☐ |
| Multithreading (thread, mutex, atomic) | ☐ | ☐ | ☐ |
| Async / Future / Promise | ☐ | ☐ | ☐ |

#### Jour 5 — Synthèse
| Compétence | 1 | 2 | 3 |
|---|---|---|---|
| Architecture hexagonale | ☐ | ☐ | ☐ |
| Projet complet compilable et testé | ☐ | ☐ | ☐ |
| Code review constructive | ☐ | ☐ | ☐ |
| Documentation et lisibilité | ☐ | ☐ | ☐ |

### Interprétation du score

Comptez vos scores (1=1pt, 2=2pts, 3=3pts) :

| Score | Niveau | Plan d'action |
|---|---|---|
| **≥ 55 pts** | Expert | Former les autres, contribuer à l'open source |
| **45-54 pts** | Avancé | Formation C++ avancée dans 3 mois |
| **30-44 pts** | Intermédiaire | Pratique quotidienne 3-6 mois |
| **< 30 pts** | Débutant | Revoir les jours 1-2, projets simples |

### Mon plan d'action personnel

```
Les 3 compétences que je veux consolider en priorité :
1. _______________________________________________
2. _______________________________________________
3. _______________________________________________

Ressource principale que je vais utiliser :
☐ Effective Modern C++ (Meyers)
☐ C++ Concurrency in Action (Williams)
☐ cppreference.com quotidiennement
☐ Exercices LeetCode/HackerRank en C++
☐ Projet personnel : ________________________

Objectif dans 30 jours :
_______________________________________________

Objectif dans 90 jours :
_______________________________________________
```
