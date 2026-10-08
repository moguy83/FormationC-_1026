# 📘 Exercices C++ — Jour 2 : Mémoire, STL & Design Patterns
**Formation C++ · 5 jours · Niveau débutant–intermédiaire**

---

## 📋 Sommaire

- [Objectifs du jour](#objectifs)
- [⚡ Niveau 1 — Échauffement](#niveau-1) *(~30 min)*
  - [1.1 Stack vs Heap — modèle mémoire](#11-stack-vs-heap)
  - [1.2 Pointeurs et références](#12-pointeurs-et-références)
  - [1.3 unique_ptr et shared_ptr](#13-smart-pointers)
  - [1.4 STL — vector et algorithmes](#14-vector-et-algorithmes)
  - [1.5 STL — map et set](#15-map-et-set)
- [🔥 Niveau 2 — Pratique](#niveau-2) *(~60 min)*
  - [2.1 RAII et gestion des ressources](#21-raii)
  - [2.2 Move semantics](#22-move-semantics)
  - [2.3 STL avancé — algorithmes et lambdas](#23-stl-avancé)
  - [2.4 Pattern Singleton et Factory](#24-singleton-et-factory)
  - [2.5 Pattern Observer et Strategy](#25-observer-et-strategy)
- [🚀 Niveau 3 — Défi](#niveau-3) *(~90 min)*
  - [3.1 Allocateur custom](#31-allocateur-custom)
  - [3.2 Conteneur générique](#32-conteneur-générique)
  - [3.3 Système de plugins avec Factory](#33-plugins-factory)
- [🏆 Mini-projet du Jour 2](#mini-projet)
- [💡 Indices et solutions partielles](#indices)
- [✅ Auto-évaluation](#auto-évaluation)

---

## 🎯 Objectifs du jour {#objectifs}

À l'issue de ces exercices, vous serez capable de :
- Distinguer et gérer la mémoire Stack et Heap de façon sûre
- Utiliser `unique_ptr`, `shared_ptr`, `weak_ptr` en lieu et place de `new`/`delete`
- Appliquer le principe RAII pour toute acquisition de ressource
- Exploiter les conteneurs et algorithmes STL efficacement
- Implémenter les patterns Singleton, Factory, Observer et Strategy

---

## ⚡ Niveau 1 — Échauffement {#niveau-1}

---

### 1.1 Stack vs Heap {#11-stack-vs-heap}

**Exercice 1.1.a — Identifier la mémoire**

Pour chaque ligne, indiquez si la variable est allouée sur la **Stack** ou le **Heap**, et justifiez :

```cpp
int a = 5;
int* p = new int(5);
std::string s = "hello";
auto v = std::make_unique<std::vector<int>>(100);
static double pi = 3.14159;
std::array<char, 256> buffer;
void* raw = malloc(1024);
std::shared_ptr<int> sp = std::make_shared<int>(42);
```

**Question bonus** : que se passe-t-il si vous allouez un tableau de 10 millions d'entiers sur la Stack ?

---

**Exercice 1.1.b — Identifier les fuites mémoire**

Identifiez et corrigez les fuites mémoire dans ce code :

```cpp
void fonctionBuguee() {
    int* tab = new int[100];
    std::string* msg = new std::string("hello");

    if (tab[0] == 0) {
        return;  // Bug 1
    }

    for (int i = 0; i < 100; i++) {
        tab[i] = i;
    }

    delete msg;
    delete tab;  // Bug 2
}

class Gestionnaire {
    int* donnees;
public:
    Gestionnaire() : donnees(new int[50]) {}
    // Bug 3 : que manque-t-il ?

    void reset() {
        delete[] donnees;
        donnees = new int[50];  // Bug 4 : que se passe-t-il si new échoue ?
    }
};
```

---

### 1.2 Pointeurs et références {#12-pointeurs-et-références}

**Exercice 1.2.a — Tableau de pointeurs**

```cpp
// Créer un tableau de 5 pointeurs vers des entiers
// Initialiser chaque pointeur avec une valeur différente
// Afficher les valeurs, les adresses, et les adresses des pointeurs
// Libérer correctement toute la mémoire
```

Dessinez (schéma ASCII) la représentation en mémoire de votre tableau.

---

**Exercice 1.2.b — Pointeurs de fonctions**

```cpp
// Déclarer un type alias pour une fonction : int(int, int)
using OperationBinaire = int(*)(int, int);

// Créer un tableau de 4 fonctions : add, sub, mul, div
// Les appeler dynamiquement selon le choix de l'utilisateur
// Puis refaire avec std::function<int(int,int)>

// Laquelle est plus flexible ? Plus performante ? Pourquoi ?
```

---

### 1.3 Smart Pointers {#13-smart-pointers}

**Exercice 1.3.a — Convertir du code**

Convertissez ce code C++98 en C++ moderne (sans `new`/`delete`) :

```cpp
// À convertir
class Noeud {
public:
    int valeur;
    Noeud* gauche;
    Noeud* droite;
    Noeud(int v) : valeur(v), gauche(nullptr), droite(nullptr) {}
    ~Noeud() { delete gauche; delete droite; }
};

Noeud* creerArbre() {
    Noeud* racine = new Noeud(1);
    racine->gauche = new Noeud(2);
    racine->droite = new Noeud(3);
    racine->gauche->gauche = new Noeud(4);
    return racine;
}
// (gestion des delete oubliée dans l'original)
```

---

**Exercice 1.3.b — Cycles avec weak_ptr**

Implémentez un graphe dirigé sans cycle de propriété :

```cpp
struct Sommet {
    std::string nom;
    std::vector<std::weak_ptr<Sommet>> voisins;  // pas de cycle !

    void ajouterVoisin(std::shared_ptr<Sommet> v) {
        voisins.push_back(v);
    }

    void afficherVoisins() const;
};

// Créer un graphe : A→B→C→A (cycle)
// Vérifier qu'il n'y a pas de fuite mémoire (Valgrind)
```

---

**Exercice 1.3.c — unique_ptr dans un vecteur**

```cpp
// Stocker une hiérarchie de formes dans un vecteur de unique_ptr
std::vector<std::unique_ptr<Forme>> formes;
formes.push_back(std::make_unique<Cercle>(5.0));
formes.push_back(std::make_unique<Rectangle>(4.0, 3.0));

// Essayez de copier le vecteur → que se passe-t-il ?
// Déplacez-le avec std::move → que se passe-t-il ?
// Accédez à un élément par index et appelez une méthode virtuelle
// Supprimez le deuxième élément
// Itérez et affichez l'aire de toutes les formes
```

---

### 1.4 vector et algorithmes {#14-vector-et-algorithmes}

**Exercice 1.4.a — Pipeline STL**

Partez d'un `std::vector<int>` de 20 valeurs aléatoires entre 1 et 100.

Effectuez dans l'ordre :
1. Supprimer les doublons (sans trier au préalable — pensez à `std::unordered_set`)
2. Garder seulement les valeurs paires
3. Multiplier chaque valeur par 3
4. Trier dans l'ordre décroissant
5. Garder les 5 premières valeurs
6. Calculer leur somme et leur produit

Faites-le en **une seule chaîne** avec les algorithmes STL + lambdas.

---

**Exercice 1.4.b — Performances vector**

Mesurez (avec `std::chrono::high_resolution_clock`) le temps d'exécution pour insérer 1 million d'entiers :

| Méthode | Temps mesuré |
|---|---|
| `push_back` sans `reserve` | |
| `push_back` avec `reserve(1000000)` | |
| Assignation directe `v[i] = i` (après `resize`) | |
| `std::iota` sur un vecteur pré-alloué | |

Expliquez les différences observées.

---

### 1.5 map et set {#15-map-et-set}

**Exercice 1.5.a — Fréquences de mots**

Lisez un fichier texte et comptez la fréquence de chaque mot (insensible à la casse, ignorer la ponctuation). Affichez :
- Les 10 mots les plus fréquents
- Les mots uniques (fréquence = 1)
- Le nombre total de mots distincts

Comparez `std::map<string, int>` vs `std::unordered_map<string, int>` en termes de performances.

---

**Exercice 1.5.b — Annuaire inversé**

```cpp
// Annuaire classique : nom → numéro
std::map<std::string, std::string> annuaire;

// Construire l'annuaire inversé : numéro → nom
// Gérer les cas où un numéro correspond à plusieurs noms
// Rechercher "qui a le numéro 06.12.34.56.78 ?"
```

---

## 🔥 Niveau 2 — Pratique {#niveau-2}

---

### 2.1 RAII {#21-raii}

**Exercice 2.1.a — Chronomètre RAII**

```cpp
// Classe qui mesure automatiquement le temps d'exécution d'un bloc
class Chrono {
    std::string nom;
    std::chrono::high_resolution_clock::time_point debut;
public:
    explicit Chrono(std::string nom);
    ~Chrono();  // affiche la durée au moment de la destruction
};

// Utilisation
{
    Chrono c("Tri de 1M d'entiers");
    trierUnMillionEntiers();
}  // ← affiche automatiquement "Tri de 1M d'entiers : 45 ms"
```

Ajoutez la possibilité de "mettre en pause" et "reprendre" le chronomètre.

---

**Exercice 2.1.b — ScopedLock maison**

Sans utiliser `std::lock_guard`, implémentez :

```cpp
class ScopedLock {
    std::mutex& mtx;
public:
    explicit ScopedLock(std::mutex& m);
    ~ScopedLock();
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
};
```

Testez avec 3 threads qui incrémentent un compteur partagé 100 000 fois chacun.

---

**Exercice 2.1.c — Gestionnaire de fichier RAII**

```cpp
class FichierTemp {
    std::filesystem::path chemin;
    std::ofstream stream;

public:
    // Crée un fichier temporaire avec un nom unique
    FichierTemp(const std::string& prefixe = "tmp");

    // Supprime le fichier à la destruction
    ~FichierTemp();

    std::ostream& getStream();
    std::string getChemin() const;

    // Non-copiable, déplaçable
};
```

---

### 2.2 Move Semantics {#22-move-semantics}

**Exercice 2.2.a — Buffer déplaçable**

```cpp
class Buffer {
    std::unique_ptr<uint8_t[]> data;
    size_t taille;

public:
    explicit Buffer(size_t n);
    Buffer(const Buffer&);             // copie profonde
    Buffer(Buffer&&) noexcept;         // move
    Buffer& operator=(const Buffer&);
    Buffer& operator=(Buffer&&) noexcept;

    void remplir(uint8_t valeur);
    size_t getTaille() const;
    uint8_t& operator[](size_t i);
    const uint8_t& operator[](size_t i) const;
};
```

Mesurez la différence de performance entre copier et déplacer un Buffer de 100 Mo.

---

**Exercice 2.2.b — Rule of Five vs Rule of Zero**

Implémentez deux versions d'une même classe :

```cpp
// Version 1 : Rule of Five (gestion manuelle)
class MatriceV1 {
    double* data;
    int lignes, colonnes;
    // Implémenter les 5 membres spéciaux
};

// Version 2 : Rule of Zero (déléguer à la STL)
class MatriceV2 {
    std::vector<double> data;
    int lignes, colonnes;
    // Aucun membre spécial nécessaire !
};
```

Vérifiez avec Valgrind que les deux versions ont zéro fuite.

---

### 2.3 STL avancé {#23-stl-avancé}

**Exercice 2.3.a — Transformer une collection de structures**

```cpp
struct Produit {
    std::string ref;
    std::string nom;
    double prix;
    int stock;
    std::string categorie;
};

std::vector<Produit> catalogue = { /* 20 produits */ };
```

Implémentez, **uniquement avec des algorithmes STL et des lambdas**, les opérations suivantes :
1. Filtrer les produits en stock (`stock > 0`) et disponibles (<50€)
2. Trier par prix croissant, puis par nom alphabétique si égalité
3. Grouper par catégorie dans une `map<string, vector<Produit>>`
4. Calculer le prix moyen de chaque catégorie
5. Trouver le produit le plus cher en stock
6. Appliquer une remise de 10% aux produits > 100€

---

**Exercice 2.3.b — Algorithme personnalisé**

Implémentez ces algorithmes génériques à la façon STL (avec des itérateurs) :

```cpp
// Retourne un itérateur vers le 2e maximum
template<typename It>
It second_max(It first, It last);

// Partitionner en N sous-groupes selon un prédicat multi-niveaux
template<typename It, typename KeyFn>
std::map<..., std::vector<...>> groupBy(It first, It last, KeyFn fn);

// zip : combiner deux conteneurs élément par élément
template<typename It1, typename It2, typename OutIt, typename Fn>
void zip_transform(It1 f1, It1 l1, It2 f2, OutIt out, Fn fn);
```

---

### 2.4 Singleton et Factory {#24-singleton-et-factory}

**Exercice 2.4.a — Logger Singleton**

```cpp
class Logger {
public:
    enum class Niveau { DEBUG, INFO, WARN, ERROR };

    static Logger& getInstance();

    void setNiveauMinimum(Niveau n);
    void setFichier(const std::string& path);
    void log(Niveau n, const std::string& msg);

    // Macros utiles
    // LOG_DEBUG("message") → Logger::getInstance().log(DEBUG, "message")

private:
    Logger() = default;
    // Thread-safe avec std::call_once
    static std::once_flag flag;
    static std::unique_ptr<Logger> instance;
};
```

---

**Exercice 2.4.b — Factory de parseurs**

```cpp
// Interface commune
class IParser {
public:
    virtual nlohmann::json parse(const std::string& contenu) = 0;
    virtual std::string format() const = 0;
    virtual ~IParser() = default;
};

class ParserJSON : public IParser { /* ... */ };
class ParserCSV  : public IParser { /* ... */ };
class ParserTOML : public IParser { /* ... */ };
class ParserXML  : public IParser { /* ... */ };

// Factory avec enregistrement dynamique
class ParserFactory {
    using Createur = std::function<std::unique_ptr<IParser>()>;
    std::unordered_map<std::string, Createur> createurs;

public:
    void enregistrer(const std::string& format, Createur fn);
    std::unique_ptr<IParser> creer(const std::string& format) const;
    std::vector<std::string> formatsDisponibles() const;
};
```

---

### 2.5 Observer et Strategy {#25-observer-et-strategy}

**Exercice 2.5.a — Système d'alertes météo**

```cpp
struct DonneesMeteo {
    double temperature;   // °C
    double humidite;      // %
    double vent;          // km/h
    double pression;      // hPa
};

class StationMeteo {
    DonneesMeteo donnees;
    std::vector<std::function<void(const DonneesMeteo&)>> observateurs;

public:
    void abonner(std::function<void(const DonneesMeteo&)> fn);
    void mettreAJour(DonneesMeteo nouvelles);
};

// Observateurs à implémenter :
// - AfficheNumerique : affiche les valeurs
// - AlerteCaleur : alerte si temp > 35°C
// - AlerteGel : alerte si temp < 0°C  
// - Historique : stocke les 100 dernières mesures
// - StatsCalculator : min, max, moyenne glissante sur 24h
```

---

**Exercice 2.5.b — Stratégie de tri configurable**

```cpp
struct Employe {
    std::string nom;
    std::string prenom;
    std::string departement;
    double salaire;
    int anciennete;  // années
};

class GestionnaireEmployes {
    std::vector<Employe> employes;
    std::function<bool(const Employe&, const Employe&)> comparateur;

public:
    void setStrategie(const std::string& critere);
    // criteres: "nom", "salaire_asc", "salaire_desc", 
    //            "anciennete", "departement_puis_salaire"
    
    void trier();
    void afficher() const;
};
```

---

## 🚀 Niveau 3 — Défi {#niveau-3}

---

### 3.1 Allocateur custom {#31-allocateur-custom}

**Exercice 3.1 — Pool d'allocation**

Implémentez un allocateur de mémoire par pool (arène) compatible STL :

```cpp
template<typename T>
class PoolAllocateur {
    struct Bloc { alignas(T) char data[sizeof(T)]; };
    std::vector<Bloc> pool;
    std::stack<T*> libres;

public:
    using value_type = T;

    explicit PoolAllocateur(size_t capacite);

    T* allocate(size_t n);
    void deallocate(T* p, size_t n);

    template<typename U>
    struct rebind { using other = PoolAllocateur<U>; };
};

// Utilisation
std::vector<int, PoolAllocateur<int>> v(PoolAllocateur<int>(1000));
std::list<std::string, PoolAllocateur<std::string>> l(
    PoolAllocateur<std::string>(100));
```

Benchmarkez contre l'allocateur par défaut pour 1 million d'allocations de `int`.

---

### 3.2 Conteneur générique {#32-conteneur-générique}

**Exercice 3.2 — Liste doublement chaînée générique**

Implémentez `ListeChainee<T>` compatible STL :

```cpp
template<typename T>
class ListeChainee {
    struct Noeud {
        T valeur;
        std::unique_ptr<Noeud> suivant;
        Noeud* precedent;
    };
    std::unique_ptr<Noeud> tete;
    Noeud* queue;
    size_t count;

public:
    // Construction/destruction
    ListeChainee();
    ListeChainee(std::initializer_list<T> init);
    ListeChainee(const ListeChainee&);
    ListeChainee(ListeChainee&&) noexcept;

    // Modification
    void push_front(const T&);
    void push_back(const T&);
    void pop_front();
    void pop_back();
    void insert(size_t pos, const T& val);
    void erase(size_t pos);

    // Accès
    T& front();
    T& back();
    size_t size() const;
    bool empty() const;

    // Itérateur bidirectionnel (requis pour for each)
    class iterator { /* ... */ };
    iterator begin();
    iterator end();

    // Algorithmes propres
    void sort();
    void reverse();
    void unique();  // supprime les doublons consécutifs
};
```

Vérifiez la compatibilité avec `std::sort`, `std::for_each`, `std::copy`.

---

### 3.3 Système de plugins avec Factory {#33-plugins-factory}

**Exercice 3.3 — Plugin Registry**

```cpp
// Macro d'enregistrement automatique
#define REGISTER_PLUGIN(nom, classe) \
    static bool _reg_##classe = \
        PluginRegistry::getInstance().enregistrer(nom, \
            []() { return std::make_unique<classe>(); });

class IPlugin {
public:
    virtual std::string nom() const = 0;
    virtual std::string version() const = 0;
    virtual void initialiser(const nlohmann::json& config) = 0;
    virtual void executer(const std::string& input, std::ostream& out) = 0;
    virtual ~IPlugin() = default;
};

// Plugins concrets à implémenter :
// - PluginMajuscules : convertit en MAJUSCULES
// - PluginCompteurMots : compte les mots
// - PluginInverse : inverse la chaîne
// - PluginBase64 : encode en Base64 (algo simplifié)

class PluginRegistry { /* Singleton + Factory */ };

// Application
class Application {
    std::vector<std::unique_ptr<IPlugin>> plugins_actifs;
public:
    void chargerPlugin(const std::string& nom, const nlohmann::json& config);
    void executer(const std::string& input);
};
```

---

## 🏆 Mini-projet du Jour 2 : Gestionnaire de tâches {#mini-projet}

**Durée estimée** : 2h30–3h

**Description** : Todo list en ligne de commande avec persistance et filtres.

### Fonctionnalités

```cpp
enum class Priorite { Basse, Normale, Haute, Critique };
enum class Statut   { ATFaire, EnCours, Terminee, Annulee };

struct Tache {
    int id;
    std::string titre;
    std::string description;
    Priorite priorite;
    Statut statut;
    std::string dateCreation;
    std::string dateEcheance;
    std::vector<std::string> tags;
};
```

### Architecture

```cpp
class DepotTaches {              // Repository pattern
    std::vector<Tache> taches;
    std::string fichierJSON;

public:
    void ajouter(Tache t);
    void modifier(int id, const Tache& modif);
    void supprimer(int id);
    std::optional<Tache> trouver(int id) const;

    // Filtres (retournent des vues, pas des copies)
    std::vector<std::reference_wrapper<const Tache>>
        filtrerParStatut(Statut s) const;
    std::vector<std::reference_wrapper<const Tache>>
        filtrerParPriorite(Priorite p) const;
    std::vector<std::reference_wrapper<const Tache>>
        filtrerParTag(const std::string& tag) const;
    std::vector<std::reference_wrapper<const Tache>>
        rechercherPartiel(const std::string& terme) const;

    // Statistiques
    std::map<Statut, int> statistiques() const;
    std::map<std::string, int> repartitionParTag() const;
};

class NotificateurEcheance {    // Observer
    void verifier(const Tache& t);
};
```

### Contraintes

- Persistance en JSON avec `nlohmann_json` ou CSV fait maison
- Observer pattern : notifier quand une tâche passe en statut "En cours"
- Strategy pattern : différents algorithmes de tri (priorité, date, titre)
- Zéro `new`/`delete` — smart pointers partout
- 15 tests unitaires minimum

---

## 💡 Indices et solutions partielles {#indices}

<details>
<summary><strong>1.3.a — Convertir le code avec unique_ptr</strong></summary>

```cpp
struct Noeud {
    int valeur;
    std::unique_ptr<Noeud> gauche;
    std::unique_ptr<Noeud> droite;
    explicit Noeud(int v) : valeur(v) {}
    // Plus besoin de destructeur : unique_ptr gère tout !
};

std::unique_ptr<Noeud> creerArbre() {
    auto racine = std::make_unique<Noeud>(1);
    racine->gauche = std::make_unique<Noeud>(2);
    racine->droite = std::make_unique<Noeud>(3);
    racine->gauche->gauche = std::make_unique<Noeud>(4);
    return racine;  // move implicite
}
```
</details>

<details>
<summary><strong>2.3.a — Grouper par catégorie avec STL</strong></summary>

```cpp
// Grouper des produits par catégorie
std::map<std::string, std::vector<Produit>> parCategorie;
for (const auto& p : catalogue) {
    parCategorie[p.categorie].push_back(p);
}

// Ou avec std::for_each + lambda
std::for_each(catalogue.begin(), catalogue.end(),
    [&parCategorie](const Produit& p) {
        parCategorie[p.categorie].push_back(p);
    });
```
</details>

<details>
<summary><strong>2.4.a — Singleton thread-safe avec call_once</strong></summary>

```cpp
std::once_flag Logger::flag;
std::unique_ptr<Logger> Logger::instance;

Logger& Logger::getInstance() {
    std::call_once(flag, []() {
        instance = std::unique_ptr<Logger>(new Logger());
    });
    return *instance;
}
```
</details>

<details>
<summary><strong>2.5.a — Observer avec std::function</strong></summary>

```cpp
void StationMeteo::abonner(std::function<void(const DonneesMeteo&)> fn) {
    observateurs.push_back(std::move(fn));
}

void StationMeteo::mettreAJour(DonneesMeteo nouvelles) {
    donnees = nouvelles;
    for (const auto& obs : observateurs) {
        obs(donnees);
    }
}

// Abonner une lambda
station.abonner([](const DonneesMeteo& d) {
    if (d.temperature > 35.0)
        std::cout << "ALERTE CHALEUR : " << d.temperature << "°C\n";
});
```
</details>

---

## ✅ Auto-évaluation {#auto-évaluation}

### Mémoire
- [ ] Je comprends la différence Stack vs Heap et les implications
- [ ] Je sais identifier et corriger une fuite mémoire
- [ ] J'utilise `unique_ptr` par défaut et `shared_ptr` si partage nécessaire
- [ ] Je n'utilise plus jamais `new`/`delete` dans du code de production
- [ ] Je sais appliquer le principe RAII pour les fichiers, mutex, connexions

### STL
- [ ] Je sais choisir le bon conteneur selon le cas d'usage
- [ ] Je sais chaîner les algorithmes STL avec des lambdas
- [ ] Je comprends la complexité de `vector`, `map`, `unordered_map`
- [ ] Je sais utiliser `std::sort` avec un comparateur custom
- [ ] Je comprends l'idiome erase-remove

### Design Patterns
- [ ] Je sais implémenter un Singleton thread-safe
- [ ] Je sais concevoir une Factory extensible
- [ ] Je sais implémenter l'Observer avec `std::function`
- [ ] Je sais encapsuler une stratégie variable dans un objet

### Score indicatif
- **12-14 cases** : Excellent — prêt pour le Jour 3
- **8-11 cases** : Bien — consolider les points non cochés
- **< 8 cases** : Reprendre les exercices Niveau 1-2
