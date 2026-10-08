# ✅ Corrections — Jour 5 : Projet de Synthèse & Consolidation
**Formation C++ · 5 jours · Corrigé des exercices**

---

## ⚡ Révision express

### R.1 Quiz C++ — 20 questions (réponses)

**Mémoire et RAII**
1. `unique_ptr` = propriété exclusive, non copiable, coût quasi nul ; `shared_ptr` = propriété partagée avec compteur atomique. Utiliser `unique_ptr` par défaut, `shared_ptr` seulement si plusieurs propriétaires réels sont nécessaires.
2. RAII (*Resource Acquisition Is Initialization*) : lier la durée de vie d'une ressource à celle d'un objet — acquisition dans le constructeur, libération dans le destructeur (appelé même en cas d'exception). Cela garantit qu'aucune ressource n'est oubliée.
3. Oublier `delete[]` après `new int[10]` provoque une **fuite mémoire** : la mémoire allouée n'est jamais rendue au système tant que le programme tourne.
4. `std::move(v)` transtype `v` en rvalue reference, permettant au récepteur de "voler" ses ressources internes (O(1)). Après le move, `v` est dans un état valide mais non spécifié (généralement vide pour un `vector`).
5. `map::find()` est O(log n) (arbre équilibré), `unordered_map::find()` est O(1) en moyenne (table de hachage), O(n) au pire (collisions).
6. `vector` est préféré à `list` car il offre une bien meilleure localité cache (mémoire contiguë), ce qui compense largement son coût d'insertion au milieu dans la majorité des cas réels.
7. Idiome erase-remove : `v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; }), v.end());` — supprime les éléments pairs.
8. `std::sort` (introsort) n'est pas stable — l'ordre relatif des éléments égaux n'est pas garanti ; `std::stable_sort` le préserve, au prix d'un léger surcoût.

**Templates et Generics**
9. `template<typename T>` et `template<class T>` sont strictement équivalents en syntaxe C++ (aucune différence sémantique) ; `typename` est la convention moderne recommandée.
10. SFINAE (*Substitution Failure Is Not An Error*) : quand la substitution d'un paramètre de template échoue, le compilateur retire silencieusement cette surcharge du jeu de candidats plutôt que de lever une erreur. Exemple : `std::enable_if_t<std::is_integral_v<T>>` pour restreindre un template aux types entiers.
11. `template<typename T> concept Numerique = std::integral<T> || std::floating_point<T>;`

**Concurrence**
12. Un data race survient quand deux threads accèdent à la même donnée en même temps, sans synchronisation, et qu'au moins un accès est une écriture → comportement indéfini. Détectable avec ThreadSanitizer (`-fsanitize=thread`).
13. `lock_guard` : verrouillage simple, RAII pur, pas de déverrouillage manuel possible. `unique_lock` : plus flexible (déverrouillage/reverrouillage manuel, lock différé, compatible `condition_variable::wait`).
14. `std::atomic<int>` est préférable pour de simples opérations arithmétiques sur un scalaire (moins de surcoût qu'un mutex, pas de blocage de thread).
15. `std::async(std::launch::deferred, fn)` ne lance PAS de thread : `fn` n'est exécutée que lors du premier appel à `.get()` ou `.wait()` sur le `future` (exécution paresseuse, synchrone).

**Sécurité**
16. `strcpy` ne vérifie pas la taille du buffer destination → buffer overflow possible. Alternative moderne : `std::string`.
17. Une injection SQL consiste à insérer du code SQL dans une entrée utilisateur non échappée, concaténée directement dans une requête, modifiant sa logique. Protection : requêtes préparées avec paramètres liés.
18. `-fsanitize=address` détecte les erreurs mémoire (overflow, use-after-free) ; `-fsanitize=thread` détecte les data races entre threads. Ils ne sont généralement pas combinables dans le même binaire.

**Architecture**
19. CRTP (*Curiously Recurring Template Pattern*) : une classe de base template paramétrée par sa classe dérivée elle-même (`class Derived : public Base<Derived>`), utilisée pour du polymorphisme statique sans coût virtuel (ex. `clone()` typé sans cast).
20. La composition ("a-un") assemble des objets indépendants ; l'héritage ("est-un") établit une relation de sous-typage. Préférer la composition par défaut (plus flexible, plus faiblement couplée) et réserver l'héritage aux vraies relations polymorphiques.

---

### R.2 Code Review — trouver les bugs

**Code 1 : GestionnaireFichiers — problèmes identifiés**

1. `fopen` peut retourner `nullptr` (fichier inexistant, ex. "inexistant.txt") — jamais vérifié → crash à l'usage.
2. Descripteurs de fichiers bruts (`FILE*`) sans RAII : si une exception survient entre l'ouverture et `fermerTous()`, fuite de descripteur.
3. `lire()` ne vérifie pas que `index` est valide (`fichiers[index]` hors bornes = comportement indéfini).
4. `buffer[256]` : `fread` peut ne remplir qu'une partie du buffer ; `std::string(buffer)` lit jusqu'au premier `\0`, potentiellement des données non initialisées si le fichier est plus petit que 256 octets et que `buffer` n'est pas terminé par zéro.
5. Le destructeur est vide : si `fermerTous()` n'est jamais appelé (cas illustré dans l'utilisation), les fichiers restent ouverts jusqu'à la fin du programme — pas de RAII.
6. Ouvrir `/etc/passwd` en lecture directe sans contrôle d'accès applicatif est une mauvaise pratique de sécurité (accès à un fichier système sensible).

**Version corrigée (RAII avec `std::ifstream`)** :
```cpp
class GestionnaireFichiers {
    std::vector<std::ifstream> fichiers;
public:
    bool ouvrirFichier(const std::string& path) {
        std::ifstream f(path);
        if (!f.is_open()) return false;
        fichiers.push_back(std::move(f));
        return true;
    }
    std::string lire(size_t index) {
        if (index >= fichiers.size()) throw std::out_of_range("Index invalide");
        std::string ligne;
        std::getline(fichiers[index], ligne);
        return ligne;
    }
    // Pas besoin de fermerTous() ni de destructeur : ifstream se ferme automatiquement (RAII)
};
```

**Code 2 : Cache — problèmes de concurrence identifiés**

1. `nettoyeur.detach()` : le thread détaché continue à tourner même après la destruction de `Cache` → accès à `this` invalide (use-after-free potentiel) une fois l'objet détruit. Il faudrait joindre le thread dans le destructeur avec un flag d'arrêt.
2. `get()` utilise `.at(cle)` qui lève une exception si la clé n'existe pas — pas nécessairement un bug, mais l'appelant doit être prêt à l'attraper (sinon crash par exception non gérée).
3. `set()` **n'a pas de lock** alors que `get()`/`contains()`/le thread de nettoyage en ont un → data race garantie sur `data` dès qu'un thread appelle `set()` pendant qu'un autre lit.
4. Pattern **TOCTOU** (Time-Of-Check to Time-Of-Use) : entre `contains("user")` et `get("user")`, un autre thread peut avoir supprimé/vidé la clé (le nettoyeur fait `data.clear()` toutes les 60s) → `get()` peut lever une exception malgré le `contains()` précédent réussi.

**Correction** : ajouter le lock manquant dans `set()`, et remplacer le pattern check-then-act par une méthode atomique unique `tryGet(cle)` retournant un `std::optional<std::string>` sous un seul verrou.

---

### R.3 Comparatif de solutions

```cpp
// Version 1 : boucles traditionnelles
std::map<std::string, int> compterV1(const std::vector<std::string>& textes) {
    std::map<std::string, int> freq;
    for (const auto& texte : textes) {
        std::istringstream iss(texte);
        std::string mot;
        while (iss >> mot) freq[mot]++;
    }
    return freq;
}

// Version 2 : algorithmes STL
std::map<std::string, int> compterV2(const std::vector<std::string>& textes) {
    std::map<std::string, int> freq;
    std::for_each(textes.begin(), textes.end(), [&freq](const std::string& texte) {
        std::istringstream iss(texte);
        std::for_each(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
                      [&freq](const std::string& mot) { freq[mot]++; });
    });
    return freq;
}

// Version 3 : Ranges C++20
std::map<std::string, int> compterV3(const std::vector<std::string>& textes) {
    std::map<std::string, int> freq;
    for (const auto& texte : textes)
        for (const auto& motView : std::views::split(texte, ' '))
            freq[std::string(motView.begin(), motView.end())]++;
    return freq;
}
```
Comparatif : **V1** est la plus lisible pour un débutant et la plus rapide à écrire ; **V2** apporte peu de gain de lisibilité pour un surcoût de complexité (itérateurs de flux) ; **V3** est la plus expressive/déclarative une fois les Ranges maîtrisés, mais nécessite un compilateur récent (C++20). Les trois ont la même complexité O(nombre total de mots).

---

## 🔥 Exercices de consolidation

### C.1 Refactoring guidé — version modernisée

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>

struct Personne { std::string nom; int age; };
using ListePersonnes = std::vector<Personne>;

void ajouterPersonne(ListePersonnes& liste, const std::string& nom, int age) {
    if (nom.empty() || age < 0 || age > 150) return;
    liste.push_back({nom, age});
}

std::optional<Personne> trouverPlusVieux(const ListePersonnes& liste) {
    if (liste.empty()) return std::nullopt;
    return *std::max_element(liste.begin(), liste.end(),
                              [](const Personne& a, const Personne& b) { return a.age < b.age; });
}

void afficher(const ListePersonnes& liste) {
    for (const auto& p : liste) std::cout << p.nom << " : " << p.age << "\n";
}

int main() {
    ListePersonnes liste;
    ajouterPersonne(liste, "Alice", 30);
    ajouterPersonne(liste, "Bob", 25);
    ajouterPersonne(liste, "Charlie", 35);

    std::sort(liste.begin(), liste.end(),
              [](const Personne& a, const Personne& b) { return a.age < b.age; });
    afficher(liste);

    if (auto plusVieux = trouverPlusVieux(liste))
        std::cout << "Plus vieux : " << plusVieux->nom << "\n";
    // Pas de new/delete : le vector se libère automatiquement.
}
```
Checklist respectée : `typedef`→`using`, aucun pointeur brut, RAII complet, lambdas au lieu de fonctions de comparaison nommées, range-based for possible dans `afficher`, `struct Personne` nommée au lieu de `std::pair`, brace-init, `std::optional` pour `trouverPlusVieux`.

### C.2 Debugging avancé

Bugs identifiés (segfault) :
1. `setRacine` jamais appelée dans `main()` → `racine == nullptr`, puis `trouver(racine, ...)` déréférence un pointeur nul dès le premier appel.
2. Pointeurs bruts (`Noeud*`) sans propriété claire → fuite mémoire garantie (aucun `delete` nulle part) et destructions manuelles fragiles.
3. `ajouter()` ne vérifie jamais que `trouver()` a retourné un pointeur non nul avant de faire `p->ajouterEnfant(...)`.

**Version corrigée avec `unique_ptr` :**
```cpp
class Noeud {
public:
    std::string valeur;
    std::vector<std::unique_ptr<Noeud>> enfants;
    explicit Noeud(std::string v) : valeur(std::move(v)) {}

    void ajouterEnfant(std::unique_ptr<Noeud> enfant) { enfants.push_back(std::move(enfant)); }

    void afficher(int profondeur = 0) const {
        std::cout << std::string(profondeur * 2, ' ') << valeur << "\n";
        for (const auto& e : enfants) e->afficher(profondeur + 1);
    }
};

class Arbre {
    std::unique_ptr<Noeud> racine;
public:
    void setRacine(const std::string& val) { racine = std::make_unique<Noeud>(val); }

    void ajouter(const std::string& parent, const std::string& enfant) {
        Noeud* p = trouver(racine.get(), parent);
        if (!p) throw std::runtime_error("Parent introuvable : " + parent);
        p->ajouterEnfant(std::make_unique<Noeud>(enfant));
    }

    Noeud* trouver(Noeud* noeud, const std::string& val) {
        if (!noeud) return nullptr;
        if (noeud->valeur == val) return noeud;
        for (auto& e : noeud->enfants) if (Noeud* res = trouver(e.get(), val)) return res;
        return nullptr;
    }

    void afficher() const { if (racine) racine->afficher(); }
};

int main() {
    Arbre arbre;
    arbre.setRacine("Racine");         // fix : appel oublié dans l'original
    arbre.ajouter("Racine", "Enfant1");
    arbre.ajouter("Racine", "Enfant2");
    arbre.ajouter("Enfant1", "Petit-enfant1");
    arbre.afficher();
    // Aucune libération manuelle nécessaire : unique_ptr gère tout (RAII).
}
```

### C.3 Optimisation mesurée — méthode

1. Baseline avec Google Benchmark sur `trouverParId`, `parDepartement`, etc. (O(n) sur 50 000 employés → quelques dizaines de µs par appel).
2. `perf record -g ./app && perf report` révèle que les recherches linéaires (`std::find_if`) dominent le temps CPU.
3. Ajout d'un `std::unordered_map<int, size_t>` (id → index) pour `trouverParId` → O(1), speedup ≥ 100× validé par Benchmark.
4. Ajout d'un `std::unordered_multimap<std::string, size_t>` (département → indices) pour `parDepartement` → O(k), speedup ≥ 10×.
5. Coût mémoire supplémentaire documenté : ~quelques Mo pour 50 000 entrées, largement justifié à 10 000 appels/seconde.

### C.4 Tests et couverture

```cpp
TEST_CASE("Validateur — email", "[validation]") {
    Validateur v;
    auto [entree, attendu] = GENERATE(table<std::string, bool>({
        {"test@example.com", true},
        {"invalide", false},
        {"a@b.c", true},
        {"", false},
        {"@manque-local.com", false}
    }));
    REQUIRE(v.validerEmail(entree).valide == attendu);
}

TEST_CASE("Validateur — mot de passe", "[validation]") {
    Validateur v;
    REQUIRE(v.validerMotDePasse("Abc123!@").valide);
    REQUIRE_FALSE(v.validerMotDePasse("court1!").valide);       // trop court
    REQUIRE_FALSE(v.validerMotDePasse("abcdefgh1!").valide);    // pas de majuscule
    REQUIRE_FALSE(v.validerMotDePasse("ABCDEFGH1!").valide);    // pas de minuscule
    REQUIRE_FALSE(v.validerMotDePasse("Abcdefgh!").valide);     // pas de chiffre
    REQUIRE_FALSE(v.validerMotDePasse("Abcdefg12").valide);     // pas de caractère spécial
}

TEST_CASE("Validateur — date de naissance", "[validation]") {
    Validateur v;
    REQUIRE(v.validerDateNaissance("15/06/1990").valide);
    REQUIRE_FALSE(v.validerDateNaissance("31/02/2000").valide);  // jour invalide
    REQUIRE_FALSE(v.validerDateNaissance("15/06/2200").valide);  // âge > 120
}
```
*(Compléter symétriquement pour `validerTelephone` et `validerIBAN` avec cas valides/invalides/limites jusqu'à couvrir ≥ 90% des lignes et branches, vérifié via `gcovr --html-details coverage.html`.)*

---

## 🏆 Projet de synthèse — LibraryManager (guide de correction)

Le projet étant ouvert (architecture Ports & Adapters), la correction porte sur les **points de contrôle attendus** plutôt que sur un unique code source :

### Domaine

```cpp
struct ISBN {
    std::string valeur;
    ISBN(std::string v) : valeur(std::move(v)) {
        // Validation checksum ISBN-13 simplifiée
        if (valeur.size() != 13) throw std::invalid_argument("ISBN invalide");
    }
    bool operator==(const ISBN& o) const { return valeur == o.valeur; }
};

struct Livre {
    ISBN isbn; std::string titre, auteur; int anneePublication;
    std::string genre; bool disponible = true;
    std::optional<std::string> description; std::vector<std::string> tags;
};
```

### Ports (interfaces)

```cpp
class ILivreRepository {
public:
    virtual void ajouter(const Livre&) = 0;
    virtual std::optional<Livre> trouverParISBN(const ISBN&) const = 0;
    virtual std::vector<Livre> rechercher(const std::string& terme) const = 0;
    virtual void mettreAJour(const Livre&) = 0;
    virtual ~ILivreRepository() = default;
};
```

### Use case type

```cpp
class EmprunterLivre {
    ILivreRepository& livres; IMembreRepository& membres; IEmpruntRepository& emprunts;
public:
    EmprunterLivre(ILivreRepository& l, IMembreRepository& m, IEmpruntRepository& e)
        : livres(l), membres(m), emprunts(e) {}

    Emprunt executer(const ISBN& isbn, int membreId) {
        auto livre = livres.trouverParISBN(isbn);
        if (!livre) throw LivreIntrouvable(isbn.valeur);
        if (!livre->disponible) throw LivreDejaEmprunte(isbn.valeur);

        auto membre = membres.trouver(membreId);
        if (!membre) throw MembreIntrouvable(membreId);
        if (membre->nbEmpruntsActifs >= Membre::MAX_EMPRUNTS) throw LimiteEmpruntsAtteinte(membreId);

        livre->disponible = false;
        livres.mettreAJour(*livre);

        Emprunt e{ /* id */ 0, isbn, membreId,
                   std::chrono::system_clock::now(),
                   std::chrono::system_clock::now() + std::chrono::hours(24 * 14), std::nullopt };
        emprunts.ajouter(e);
        return e;
    }
};
```

### Grille d'évaluation appliquée (extraits des points de contrôle)

- **Compilation propre** : `cmake --build . 2>&1 | grep -i warning` doit être vide avec `-Wall -Wextra`.
- **Tests** : ≥ 30 tests répartis entre domaine (Livre, Membre, Emprunt), use cases (emprunter, retourner, rechercher) et persistance (JSON repository).
- **Mémoire** : `valgrind --leak-check=full ./tests` → 0 fuite (garanti par le tout-`unique_ptr`/RAII du domaine).
- **Architecture** : chaque `usecase` ne dépend que d'interfaces (`ports/`), jamais d'une implémentation concrète (`adapters/`) → respecte l'inversion de dépendance (SOLID - D).
- **Pénalité de retard** (`Emprunt::calculerPenalite`) : `0.20 * jours_de_retard` si `dateRetourReelle` (ou la date du jour si non retourné) dépasse `dateRetourPrevue`.

---

## 🎓 Bonus — pistes de correction rapides

- **Corroutines** : `std::generator` (C++23) ou une implémentation maison avec `co_yield`/`co_return` pour la pagination — vérifier que chaque `co_yield` suspend bien l'exécution sans bloquer le thread appelant.
- **REST API** : `cpp-httplib` suffit pour un CRUD simple ; penser à sérialiser/désérialiser via `nlohmann::json` et à valider les entrées (cf. Jour 4, InputValidator) avant insertion.
- **CRTP** : vérifier que chaque classe dérivée implémente bien `getId()`/`serialize()` utilisés par `Entite<Derived>::operator==`/`toJSON()`, sinon erreur de compilation (SFINAE implicite via `static_cast`).

---

## ✅ Grille d'auto-évaluation — repères de correction

Un score ≥ 45/60 après ce Jour 5 traduit une bonne maîtrise de l'ensemble des 5 jours de formation. Les points les plus fréquemment sous-évalués par les stagiaires sont : l'architecture hexagonale (Ports & Adapters) et la rigueur des tests d'intégration — à retravailler en priorité si le score du bloc "Jour 5 — Synthèse" est inférieur à 8/12.

---

## ✅ Points clés à retenir de la formation (synthèse 5 jours)

1. **RAII partout** : ressources = objets, jamais de gestion manuelle.
2. **Zéro `new`/`delete` nu** : `unique_ptr` par défaut, `shared_ptr` si partage réel prouvé nécessaire.
3. **STL et algorithmes** avant boucles manuelles : plus sûr, souvent plus rapide, toujours plus lisible.
4. **Templates/concepts** pour généraliser sans dupliquer, avec des messages d'erreur clairs (C++20).
5. **Exceptions** hiérarchisées et documentées, jamais lancées depuis un destructeur.
6. **Sécurité** : valider et sanitiser toute entrée externe, activer les sanitizers en développement.
7. **Concurrence** : `atomic` pour le simple, `mutex`/`condition_variable` pour le complexe, toujours mesurer avant d'optimiser.
8. **Architecture** : séparer domaine / ports / adapters pour un code testable et évolutif.
