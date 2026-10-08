# 📘 Exercices C++ — Jour 3 : Templates, Exceptions & Intégration
**Formation C++ · 5 jours · Niveau débutant–intermédiaire**

---

## 📋 Sommaire

- [Objectifs du jour](#objectifs)
- [⚡ Niveau 1 — Échauffement](#niveau-1) *(~30 min)*
  - [1.1 Templates de fonctions](#11-templates-fonctions)
  - [1.2 Templates de classes](#12-templates-classes)
  - [1.3 Exceptions — try/catch/throw](#13-exceptions)
  - [1.4 CMake — configuration de base](#14-cmake)
  - [1.5 GitHub Actions — CI minimal](#15-github-actions)
- [🔥 Niveau 2 — Pratique](#niveau-2) *(~60 min)*
  - [2.1 Templates avancés et SFINAE](#21-sfinae)
  - [2.2 Concepts C++20](#22-concepts)
  - [2.3 Hiérarchie d'exceptions](#23-hiérarchie-exceptions)
  - [2.4 Organisation multifichiers](#24-multifichiers)
  - [2.5 Tests d'intégration](#25-tests-intégration)
- [🚀 Niveau 3 — Défi](#niveau-3) *(~90 min)*
  - [3.1 Bibliothèque générique de conteneurs](#31-bibliothèque-générique)
  - [3.2 Pipeline de traitement typé](#32-pipeline-typé)
  - [3.3 CI/CD complet sur un projet réel](#33-cicd)
- [🏆 Mini-projet du Jour 3](#mini-projet)
- [💡 Indices et solutions partielles](#indices)
- [✅ Auto-évaluation](#auto-évaluation)

---

## 🎯 Objectifs du jour {#objectifs}

À l'issue de ces exercices, vous serez capable de :
- Écrire des templates de fonctions et de classes génériques
- Utiliser SFINAE et les concepts C++20 pour contraindre les templates
- Concevoir une hiérarchie d'exceptions métier
- Structurer un projet CMake multi-modules professionnel
- Configurer un pipeline CI/CD GitHub Actions complet

---

## ⚡ Niveau 1 — Échauffement {#niveau-1}

---

### 1.1 Templates de fonctions {#11-templates-fonctions}

**Exercice 1.1.a — Fonctions génériques de base**

Implémentez en template les fonctions suivantes (elles doivent fonctionner avec tout type comparable) :

```cpp
template<typename T>
T maximum(T a, T b);

template<typename T>
T clamp(T val, T min, T max);  // borne val dans [min, max]

template<typename T>
void echanger(T& a, T& b);  // sans std::swap

template<typename Container>
typename Container::value_type somme(const Container& c);
```

Testez avec : `int`, `double`, `std::string`, `std::vector<double>`.

---

**Exercice 1.1.b — Template variadique**

```cpp
// Afficher n'importe quel nombre d'arguments, séparés par un délimiteur
template<typename... Args>
void afficher(const std::string& separateur, Args&&... args);

// afficher(", ", 1, 2.5, "hello", true)
// → "1, 2.5, hello, true"

// Calculer le maximum de n arguments
template<typename T, typename... Args>
T maximum(T premier, Args... reste);

// maximum(3, 1, 4, 1, 5, 9, 2, 6) → 9
```

---

### 1.2 Templates de classes {#12-templates-classes}

**Exercice 1.2.a — Résultat typé**

Implémentez `Resultat<T, E>` similaire à `std::expected` :

```cpp
template<typename T, typename E = std::string>
class Resultat {
public:
    static Resultat succes(T val);
    static Resultat echec(E err);

    bool estSucces() const;
    const T& valeur() const;  // throw si échec
    const E& erreur() const;  // throw si succès
    T valeurOu(T defaut) const;

    // Chaînage (monadic)
    template<typename Fn>
    auto etPuis(Fn fn) const;  // Fn: T → Resultat<U, E>
};

// Usage
Resultat<int> diviser(int a, int b) {
    if (b == 0) return Resultat<int>::echec("Division par zéro");
    return Resultat<int>::succes(a / b);
}

auto r = diviser(10, 2)
    .etPuis([](int v){ return diviser(v, 0); });
// r.estSucces() == false
```

---

### 1.3 Exceptions {#13-exceptions}

**Exercice 1.3.a — Rethrowing et chaîne d'exceptions**

```cpp
// Analyser ce code et répondre aux questions :
void niveau3() { throw std::runtime_error("Erreur disque"); }

void niveau2() {
    try { niveau3(); }
    catch (const std::runtime_error& e) {
        throw std::logic_error(
            std::string("Erreur réseau: ") + e.what());
    }
}

void niveau1() {
    try { niveau2(); }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        throw;  // que fait ce throw ?
    }
}
```

1. Quel type d'exception est capturé dans `niveau1()` ?
2. Que fait `throw;` sans argument ?
3. Ré-écrivez avec C++11 `std::nested_exception` pour chaîner les exceptions.

---

**Exercice 1.3.b — noexcept et performances**

```cpp
// Comparer deux versions de swap :
void swap_v1(std::string& a, std::string& b) {
    std::string temp = a;
    a = b;
    b = temp;
}

void swap_v2(std::string& a, std::string& b) noexcept {
    a.swap(b);
}
```

1. Pourquoi `noexcept` améliore-t-il les performances dans `std::vector` ?
2. Écrivez un template `swap_generique` avec `noexcept` conditionnel.
3. Vérifiez avec `static_assert(noexcept(swap_v2(...)))`.

---

### 1.4 CMake de base {#14-cmake}

**Exercice 1.4 — Projet CMake multi-cibles**

Créez la structure suivante et le `CMakeLists.txt` correspondant :

```
projet/
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   └── monlib/
│       ├── calcul.hpp
│       └── utils.hpp
├── src/
│   ├── calcul.cpp
│   ├── utils.cpp
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt
    ├── test_calcul.cpp
    └── test_utils.cpp
```

Le `CMakeLists.txt` doit :
- Définir deux cibles : `monlib` (bibliothèque statique) et `monapp` (exécutable)
- Lier `Catch2` pour les tests via `FetchContent`
- Exiger C++17 minimum
- Activer `-Wall -Wextra -Wpedantic`
- Fournir un preset `debug` et un preset `release`

---

### 1.5 GitHub Actions {#15-github-actions}

**Exercice 1.5 — Workflow CI minimal**

Écrivez le fichier `.github/workflows/ci.yml` qui :
1. Se déclenche sur push et pull_request vers `main`
2. Compile avec g++ sur Ubuntu-latest
3. Exécute les tests avec `ctest`
4. Génère un rapport de couverture avec `gcovr`
5. Échoue si la couverture est < 70%

```yaml
name: CI

on:
  push:
    branches: [main]
  pull_request:

jobs:
  build-test:
    runs-on: ubuntu-latest
    steps:
    # À compléter...
```

---

## 🔥 Niveau 2 — Pratique {#niveau-2}

---

### 2.1 SFINAE et enable_if {#21-sfinae}

**Exercice 2.1 — Fonctions conditionnelles selon le type**

```cpp
// Implémenter ces fonctions qui n'existent que pour certains types

// toString : fonctionne pour les types arithmétiques ET std::string
template<typename T>
std::string toString(T val);
// toString(42) → "42", toString(3.14) → "3.14", toString("hello") → "hello"
// Mais toString(std::vector<int>{}) → ERREUR DE COMPILATION

// somme : seulement pour les types numériques
template<typename Container>
auto somme(const Container& c)
    -> /* activer seulement si value_type est arithmétique */;

// estEgal : comparaison flottante avec epsilon pour les doubles
template<typename T>
bool estEgal(T a, T b);
// Pour les entiers : a == b
// Pour les flottants : |a - b| < epsilon
// Pour les autres : a == b si opérateur== existe, erreur sinon
```

---

### 2.2 Concepts C++20 {#22-concepts}

**Exercice 2.2 — Définir et utiliser des concepts**

```cpp
#include <concepts>

// Définir le concept "Triable" : un conteneur dont les éléments
// peuvent être comparés avec < et qui a begin/end
template<typename T>
concept Triable = requires(T& c) {
    c.begin();
    c.end();
    { *c.begin() < *c.begin() } -> std::convertible_to<bool>;
};

// Définir "Sérialisable" : a une méthode serialize() → string
// et une méthode statique deserialize(string) → T
template<typename T>
concept Sérialisable = /* à compléter */;

// Définir "Numérique" : int, float, double, etc.
template<typename T>
concept Numérique = /* à compléter */;

// Utiliser ces concepts
template<Triable C>
void trierEtAfficher(C& conteneur);

template<Numérique T>
T racineCarree(T x);

void sauvegarder(const Sérialisable auto& obj);
```

---

### 2.3 Hiérarchie d'exceptions {#23-hiérarchie-exceptions}

**Exercice 2.3 — Exceptions pour une API bancaire**

Concevez et implémentez cette hiérarchie :

```
ExceptionBancaire (base, hérite de std::exception)
├── ErreurValidation
│   ├── ChampObligatoire(champ: string)
│   ├── FormatInvalide(champ: string, valeur: string, format_attendu: string)
│   └── ValeurHorsPlage(champ: string, valeur, min, max)
├── ErreurMetier
│   ├── SoldeInsuffisant(solde: double, montant: double)
│   ├── CompteInexistant(numero: string)
│   ├── CompteBloque(raison: string)
│   └── LimiteDépassee(type: string, limite: double, demande: double)
└── ErreurTechnique
    ├── ErreurConnexionBDD(message: string)
    ├── ErreurTimeout(dureeMs: int)
    └── ErreurSystème(codeErreur: int, description: string)
```

Chaque exception doit :
- Avoir un constructeur significatif
- Surcharger `what()` pour retourner un message clair
- Avoir un code d'erreur unique (ex: "BNK-001")
- Être loggable (surcharger `operator<<`)

---

### 2.4 Organisation multifichiers {#24-multifichiers}

**Exercice 2.4 — Refactorer un projet monolithique**

Prenez ce fichier unique de 300 lignes et décomposez-le en modules :

```cpp
// AVANT : tout_en_un.cpp (300 lignes)
// Contient : Logger, Config, BDD, Service, main()

// APRÈS : structure à créer
// include/logger.hpp + src/logger.cpp
// include/config.hpp + src/config.cpp
// include/bdd.hpp    + src/bdd.cpp
// include/service.hpp + src/service.cpp
// src/main.cpp
// tests/test_service.cpp
```

Règles à respecter :
- Chaque header doit avoir `#pragma once`
- Pas de `using namespace std;` dans les headers
- Les headers n'incluent que ce dont ils ont **besoin** (forward declarations quand possible)
- Le CMakeLists.txt doit refléter la structure

---

### 2.5 Tests d'intégration {#25-tests-intégration}

**Exercice 2.5 — Tester avec une vraie BDD**

```cpp
// Utiliser SQLite en mémoire (:memory:) pour les tests d'intégration

class TestIntegrationRepository {
    sqlite3* db;

    void setUp() {
        sqlite3_open(":memory:", &db);
        // Créer les tables
        // Insérer les données de test
    }

    void tearDown() {
        sqlite3_close(db);
    }
};

// Test à écrire :
// 1. Insérer 5 livres → vérifier count = 5
// 2. Rechercher par auteur → vérifier les résultats
// 3. Emprunter un livre → vérifier qu'il est marqué non-disponible
// 4. Emprunter un livre déjà emprunté → vérifier l'exception
// 5. Retourner un livre → vérifier qu'il redevient disponible
```

---

## 🚀 Niveau 3 — Défi {#niveau-3}

---

### 3.1 Bibliothèque générique de conteneurs {#31-bibliothèque-générique}

**Exercice 3.1 — Implémentation de `RingBuffer<T, N>`**

```cpp
// Buffer circulaire de taille fixe, temps O(1) pour toutes les opérations
template<typename T, size_t N>
class RingBuffer {
    std::array<T, N> data;
    size_t head{0}, tail{0};
    size_t count{0};

public:
    void push(T val);          // throw si plein
    T pop();                   // throw si vide
    const T& front() const;
    bool empty() const;
    bool full() const;
    size_t size() const;

    // Itérateur circulaire
    class iterator { /* ... */ };
    iterator begin();
    iterator end();

    // Statistiques glissantes (seulement si T est numérique)
    template<typename U = T>
    std::enable_if_t<std::is_arithmetic_v<U>, U> moyenne() const;
};

// Tests : buffer de 5 éléments
// push 1,2,3,4,5 → pop 1 → push 6 → contenu = {2,3,4,5,6}
// Vérifier que l'itérateur parcourt dans le bon ordre
```

---

### 3.2 Pipeline de traitement typé {#32-pipeline-typé}

**Exercice 3.2 — Pipeline builder avec templates**

```cpp
// Pipeline de transformation de données typé à la compilation
template<typename Input, typename Output>
class Etape {
public:
    virtual Output traiter(const Input& in) = 0;
};

template<typename T>
class Pipeline {
    // Liste de fonctions de transformation
    std::vector<std::function<T(const T&)>> etapes;

public:
    Pipeline& puis(std::function<T(const T&)> fn) {
        etapes.push_back(std::move(fn));
        return *this;
    }

    T executer(const T& input) const {
        T courant = input;
        for (const auto& etape : etapes)
            courant = etape(courant);
        return courant;
    }
};

// Usage : pipeline de traitement de texte
auto resultat = Pipeline<std::string>{}
    .puis([](const std::string& s) { /* trim */ return s; })
    .puis([](const std::string& s) { /* lowercase */ return s; })
    .puis([](const std::string& s) { /* remove punctuation */ return s; })
    .executer("  Hello, World!  ");
// → "hello world"
```

Étendez le pipeline pour supporter des **transformations de type** (`Pipeline<Input, Output>`).

---

### 3.3 CI/CD complet {#33-cicd}

**Exercice 3.3 — Pipeline de qualité complet**

Créez un fichier `.github/workflows/qualite.yml` qui :

1. **Build matrix** : Ubuntu + macOS, g++ + clang++, Debug + Release
2. **Tests** : `ctest --output-on-failure`
3. **Couverture** : gcovr, upload sur Codecov, badge dans README
4. **Analyse statique** : cppcheck + clang-tidy, échec si warning
5. **Formatage** : clang-format --dry-run, échec si modifié
6. **Sanitizers** : ASan + UBSan en mode Debug
7. **Package** : CPack pour créer un .deb sur merge dans main
8. **Documentation** : Doxygen, publiée sur GitHub Pages

Chaque étape doit avoir un timeout et des artefacts en cas d'échec.

---

## 🏆 Mini-projet du Jour 3 : Bibliothèque générique de sérialisation {#mini-projet}

**Durée estimée** : 3h

**Description** : Concevoir une bibliothèque de sérialisation/désérialisation générique, multi-format.

```cpp
// Interface
template<typename T>
concept Sérialisable = requires(T obj) {
    { obj.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(std::string{}) } -> std::same_as<T>;
};

// Formats supportés
class FormatJSON { /* ... */ };
class FormatCSV  { /* ... */ };
class FormatBin  { /* ... */ };  // binaire simplifié

// Sérialiseur générique
template<typename Format>
class Serialiseur {
public:
    template<Sérialisable T>
    std::string serialiser(const T& obj) const;

    template<Sérialisable T>
    T deserialiser(const std::string& data) const;

    template<Sérialisable T>
    void sauvegarder(const T& obj, const std::filesystem::path& p) const;

    template<Sérialisable T>
    T charger(const std::filesystem::path& p) const;
};

// Macro helper pour générer serialize/deserialize
#define SERIALISABLE(Classe, ...) /* macro qui génère les méthodes */
```

Implémentez pour les classes `Livre`, `Membre`, `Emprunt` du projet bibliothèque.

**Livrables** :
- CMakeLists.txt avec `FetchContent` pour nlohmann_json
- Tests : 20 cas couvrant JSON, CSV et cas d'erreur
- CI/CD : pipeline complet qui valide la couverture > 80%

---

## 💡 Indices et solutions partielles {#indices}

<details>
<summary><strong>1.1.b — Template variadique (hint récursion)</strong></summary>

```cpp
// Cas de base (0 arguments)
void afficher(const std::string&) {}

// Cas récursif
template<typename First, typename... Rest>
void afficher(const std::string& sep, First&& f, Rest&&... rest) {
    std::cout << std::forward<First>(f);
    if constexpr (sizeof...(rest) > 0) {
        std::cout << sep;
        afficher(sep, std::forward<Rest>(rest)...);
    }
}
```
</details>

<details>
<summary><strong>2.2 — Concept Sérialisable</strong></summary>

```cpp
template<typename T>
concept Sérialisable = requires(T obj, std::string s) {
    { obj.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(s) } -> std::same_as<T>;
    std::is_default_constructible_v<T>;
};
```
</details>

<details>
<summary><strong>2.3 — Exception avec code et message</strong></summary>

```cpp
class ExceptionBancaire : public std::exception {
    std::string code_;
    std::string message_;
    mutable std::string cache_;  // pour what()

public:
    ExceptionBancaire(std::string code, std::string msg)
        : code_(std::move(code)), message_(std::move(msg)) {}

    const char* what() const noexcept override {
        cache_ = "[" + code_ + "] " + message_;
        return cache_.c_str();
    }

    const std::string& code() const { return code_; }
};
```
</details>

---

## ✅ Auto-évaluation {#auto-évaluation}

### Templates
- [ ] Je sais écrire un template de fonction avec un ou plusieurs paramètres de type
- [ ] Je sais écrire un template de classe générique
- [ ] Je comprends la spécialisation partielle et totale
- [ ] Je sais utiliser `if constexpr` pour le code conditionnel à la compilation
- [ ] Je sais définir un concept C++20 avec `requires`

### Exceptions
- [ ] Je sais créer une hiérarchie d'exceptions métier
- [ ] Je comprends la garantie forte vs basique vs nothrow
- [ ] Je sais utiliser `noexcept` correctement
- [ ] Je sais propager et relancer des exceptions (`throw;`)

### Intégration
- [ ] Je sais structurer un projet CMake multi-modules
- [ ] Je sais ajouter une dépendance externe avec `FetchContent`
- [ ] J'ai configuré un pipeline CI/CD GitHub Actions fonctionnel
- [ ] Je sais générer un rapport de couverture de code

### Score indicatif
- **12-14 cases** : Excellent — prêt pour le Jour 4
- **8-11 cases** : Bien — revoir CMake et concepts C++20
- **< 8 cases** : Reprendre les templates de base (exercices 1.1 et 1.2)
