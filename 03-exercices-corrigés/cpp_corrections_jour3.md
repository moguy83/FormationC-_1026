# ✅ Corrections — Jour 3 : Templates, Exceptions & Intégration
**Formation C++ · 5 jours · Corrigé des exercices**

---

## ⚡ Niveau 1 — Échauffement

### 1.1 Templates de fonctions

**1.1.a — Fonctions génériques de base**

```cpp
template<typename T>
T maximum(T a, T b) { return a > b ? a : b; }

template<typename T>
T clamp(T val, T min, T max) { return val < min ? min : (val > max ? max : val); }

template<typename T>
void echanger(T& a, T& b) { T temp = std::move(a); a = std::move(b); b = std::move(temp); }

template<typename Container>
typename Container::value_type somme(const Container& c) {
    typename Container::value_type acc{};
    for (const auto& e : c) acc += e;
    return acc;
}
```

**1.1.b — Template variadique**

```cpp
void afficher(const std::string&) {}   // cas de base

template<typename First, typename... Rest>
void afficher(const std::string& sep, First&& f, Rest&&... rest) {
    std::cout << std::forward<First>(f);
    if constexpr (sizeof...(rest) > 0) {
        std::cout << sep;
        afficher(sep, std::forward<Rest>(rest)...);
    }
}

template<typename T>
T maximum(T premier) { return premier; }

template<typename T, typename... Args>
T maximum(T premier, Args... reste) {
    T maxReste = maximum(reste...);
    return premier > maxReste ? premier : maxReste;
}
```

---

### 1.2 Templates de classes

**1.2.a — Resultat<T, E>**

```cpp
template<typename T, typename E = std::string>
class Resultat {
    bool succes;
    std::optional<T> val;
    std::optional<E> err;
    Resultat(bool s, std::optional<T> v, std::optional<E> e) : succes(s), val(std::move(v)), err(std::move(e)) {}

public:
    static Resultat succesDe(T val) { return Resultat(true, std::move(val), std::nullopt); }
    static Resultat echecDe(E err)  { return Resultat(false, std::nullopt, std::move(err)); }

    bool estSucces() const { return succes; }
    const T& valeur() const { if (!succes) throw std::logic_error("Pas de valeur"); return *val; }
    const E& erreur() const { if (succes) throw std::logic_error("Pas d'erreur"); return *err; }
    T valeurOu(T defaut) const { return succes ? *val : defaut; }

    template<typename Fn>
    auto etPuis(Fn fn) const -> decltype(fn(std::declval<T>())) {
        if (!succes) return decltype(fn(std::declval<T>()))::echecDe(*err);
        return fn(*val);
    }
};

Resultat<int> diviser(int a, int b) {
    if (b == 0) return Resultat<int>::echecDe("Division par zéro");
    return Resultat<int>::succesDe(a / b);
}
```
*(Note : les méthodes statiques ont été renommées `succesDe`/`echecDe` pour éviter le conflit de nom avec le membre booléen `succes` — un piège classique à souligner en correction.)*

---

### 1.3 Exceptions

**1.3.a — Rethrowing et chaîne d'exceptions**

Réponses :
1. Dans `niveau1()`, le `catch (const std::exception&)` capture le `std::logic_error` lancé par `niveau2()` (`logic_error` hérite de `exception`).
2. `throw;` sans argument **relance l'exception courante telle quelle** (même type dynamique, sans tranchage/slicing ni copie) — à utiliser uniquement dans un bloc `catch`.
3. Version chaînée :

```cpp
void niveau2bis() {
    try { niveau3(); }
    catch (...) {
        std::throw_with_nested(std::logic_error("Erreur réseau"));
    }
}

void afficherChaine(const std::exception& e, int niveau = 0) {
    std::cerr << std::string(niveau * 2, ' ') << e.what() << "\n";
    try { std::rethrow_if_nested(e); }
    catch (const std::exception& nested) { afficherChaine(nested, niveau + 1); }
    catch (...) {}
}
```

**1.3.b — noexcept et performances**

1. `noexcept` permet à `std::vector` d'utiliser le **move constructor** lors d'une réallocation plutôt qu'une copie (garantie de sécurité forte sinon impossible si le move peut lancer) — gain énorme sur de gros objets.
2. Swap générique :

```cpp
template<typename T>
void swap_generique(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                          std::is_nothrow_move_assignable_v<T>) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}
```
3. `static_assert(noexcept(swap_v2(a, b)));` compile car `std::string::swap` est marqué `noexcept`.

---

### 1.4 CMake de base

**1.4 — Projet CMake multi-cibles**

```cmake
cmake_minimum_required(VERSION 3.20)
project(monprojet VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_library(monlib STATIC src/calcul.cpp src/utils.cpp)
target_include_directories(monlib PUBLIC include)
target_compile_features(monlib PUBLIC cxx_std_17)

add_executable(monapp src/main.cpp)
target_link_libraries(monapp PRIVATE monlib)

include(FetchContent)
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0)
FetchContent_MakeAvailable(Catch2)

enable_testing()
add_subdirectory(tests)
```

```json
// CMakePresets.json
{
  "version": 3,
  "configurePresets": [
    { "name": "debug",   "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug" },   "binaryDir": "${sourceDir}/build/debug" },
    { "name": "release", "cacheVariables": { "CMAKE_BUILD_TYPE": "Release" }, "binaryDir": "${sourceDir}/build/release" }
  ]
}
```

```cmake
# tests/CMakeLists.txt
add_executable(unit_tests test_calcul.cpp test_utils.cpp)
target_link_libraries(unit_tests PRIVATE monlib Catch2::Catch2WithMain)
add_test(NAME unit_tests COMMAND unit_tests)
```

---

### 1.5 GitHub Actions

**1.5 — Workflow CI minimal**

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
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: sudo apt-get update && sudo apt-get install -y cmake g++ gcovr

      - name: Configure
        run: cmake --preset debug -DENABLE_COVERAGE=ON

      - name: Build
        run: cmake --build build/debug --parallel

      - name: Test
        run: ctest --test-dir build/debug --output-on-failure

      - name: Coverage
        run: |
          gcovr --root . --fail-under-line 70 --print-summary
```

---

## 🔥 Niveau 2 — Pratique

### 2.1 SFINAE et enable_if

```cpp
template<typename T>
std::string toString(T val) {
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<std::decay_t<T>, const char*>)
        return std::string(val);
    else if constexpr (std::is_arithmetic_v<T>)
        return std::to_string(val);
    else
        static_assert(std::is_arithmetic_v<T>, "toString ne supporte pas ce type");
}

template<typename Container>
auto somme(const Container& c)
    -> std::enable_if_t<std::is_arithmetic_v<typename Container::value_type>,
                         typename Container::value_type> {
    typename Container::value_type acc{};
    for (const auto& e : c) acc += e;
    return acc;
}

template<typename T>
bool estEgal(T a, T b) {
    if constexpr (std::is_floating_point_v<T>) return std::abs(a - b) < 1e-9;
    else return a == b;
}
```

### 2.2 Concepts C++20

```cpp
template<typename T>
concept Sérialisable = requires(T obj, std::string s) {
    { obj.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(s) } -> std::same_as<T>;
};

template<typename T>
concept Numérique = std::integral<T> || std::floating_point<T>;

template<Triable C>
void trierEtAfficher(C& conteneur) {
    std::sort(conteneur.begin(), conteneur.end());
    for (const auto& e : conteneur) std::cout << e << " ";
}

template<Numérique T>
T racineCarree(T x) { return static_cast<T>(std::sqrt(static_cast<double>(x))); }

void sauvegarder(const Sérialisable auto& obj) {
    std::ofstream out("data.txt");
    out << obj.serialize();
}
```

### 2.3 Hiérarchie d'exceptions

```cpp
class ExceptionBancaire : public std::exception {
protected:
    std::string code_, message_;
    mutable std::string cache_;
public:
    ExceptionBancaire(std::string code, std::string msg) : code_(std::move(code)), message_(std::move(msg)) {}
    const char* what() const noexcept override {
        cache_ = "[" + code_ + "] " + message_;
        return cache_.c_str();
    }
    const std::string& code() const { return code_; }
    friend std::ostream& operator<<(std::ostream& os, const ExceptionBancaire& e) { return os << e.what(); }
};

class ErreurValidation : public ExceptionBancaire {
public: using ExceptionBancaire::ExceptionBancaire;
};
class ChampObligatoire : public ErreurValidation {
public: explicit ChampObligatoire(const std::string& champ)
    : ErreurValidation("BNK-001", "Champ obligatoire manquant : " + champ) {}
};
class FormatInvalide : public ErreurValidation {
public: FormatInvalide(const std::string& champ, const std::string& valeur, const std::string& formatAttendu)
    : ErreurValidation("BNK-002", champ + "='" + valeur + "' ne respecte pas le format " + formatAttendu) {}
};

class ErreurMetier : public ExceptionBancaire {
public: using ExceptionBancaire::ExceptionBancaire;
};
class SoldeInsuffisant : public ErreurMetier {
public: SoldeInsuffisant(double solde, double montant)
    : ErreurMetier("BNK-010", "Solde " + std::to_string(solde) + " insuffisant pour " + std::to_string(montant)) {}
};
class CompteInexistant : public ErreurMetier {
public: explicit CompteInexistant(const std::string& numero)
    : ErreurMetier("BNK-011", "Compte inexistant : " + numero) {}
};

class ErreurTechnique : public ExceptionBancaire {
public: using ExceptionBancaire::ExceptionBancaire;
};
class ErreurConnexionBDD : public ErreurTechnique {
public: explicit ErreurConnexionBDD(const std::string& msg) : ErreurTechnique("BNK-020", msg) {}
};
class ErreurTimeout : public ErreurTechnique {
public: explicit ErreurTimeout(int dureeMs) : ErreurTechnique("BNK-021", "Timeout après " + std::to_string(dureeMs) + "ms") {}
};
```
*(Les autres sous-classes — `ValeurHorsPlage`, `CompteBloque`, `LimiteDépassee`, `ErreurSystème` — suivent exactement le même patron : constructeur significatif, code unique, message clair via `what()`.)*

### 2.4 Organisation multifichiers

Squelette attendu (extrait pour `logger`) :

```cpp
// include/logger.hpp
#pragma once
#include <string>

class Logger {
public:
    void log(const std::string& msg);
};
```
```cpp
// src/logger.cpp
#include "logger.hpp"
#include <iostream>
void Logger::log(const std::string& msg) { std::cout << msg << "\n"; }
```
```cpp
// include/service.hpp
#pragma once
class Logger;   // forward declaration : évite d'inclure logger.hpp si seul un pointeur/référence est utilisé
class Config;

class Service {
    Logger& logger;
    Config& config;
public:
    Service(Logger& l, Config& c);
    void executer();
};
```
```cmake
add_library(core STATIC src/logger.cpp src/config.cpp src/bdd.cpp src/service.cpp)
target_include_directories(core PUBLIC include)
add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE core)
```

### 2.5 Tests d'intégration

```cpp
TEST_CASE("Repository livre — intégration SQLite mémoire") {
    sqlite3* db;
    sqlite3_open(":memory:", &db);
    sqlite3_exec(db, "CREATE TABLE livres(id INTEGER PRIMARY KEY, titre TEXT, auteur TEXT, dispo INTEGER)", nullptr, nullptr, nullptr);

    for (int i = 0; i < 5; ++i) {
        std::string sql = "INSERT INTO livres(titre, auteur, dispo) VALUES('L" + std::to_string(i) + "', 'A', 1)";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM livres", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    REQUIRE(sqlite3_column_int(stmt, 0) == 5);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}
```
Les tests 2 à 5 (recherche par auteur, emprunt, double-emprunt, retour) suivent le même patron : `setUp()` en `:memory:`, exécution SQL, assertions sur le résultat, `tearDown()` fermant la connexion.

---

## 🚀 Niveau 3 — Défi

### 3.1 RingBuffer<T, N>

```cpp
template<typename T, size_t N>
class RingBuffer {
    std::array<T, N> data;
    size_t head = 0, tail = 0, count = 0;
public:
    void push(T val) {
        if (full()) throw std::overflow_error("RingBuffer plein");
        data[tail] = std::move(val);
        tail = (tail + 1) % N;
        ++count;
    }
    T pop() {
        if (empty()) throw std::underflow_error("RingBuffer vide");
        T val = std::move(data[head]);
        head = (head + 1) % N;
        --count;
        return val;
    }
    const T& front() const { return data[head]; }
    bool empty() const { return count == 0; }
    bool full() const { return count == N; }
    size_t size() const { return count; }

    class iterator {
        const RingBuffer* rb; size_t idx, restant;
    public:
        iterator(const RingBuffer* b, size_t i, size_t r) : rb(b), idx(i), restant(r) {}
        const T& operator*() const { return rb->data[idx]; }
        iterator& operator++() { idx = (idx + 1) % N; --restant; return *this; }
        bool operator!=(const iterator& o) const { return restant != o.restant; }
    };
    iterator begin() const { return iterator(this, head, count); }
    iterator end() const { return iterator(this, tail, 0); }

    template<typename U = T>
    std::enable_if_t<std::is_arithmetic_v<U>, U> moyenne() const {
        U somme{};
        for (const auto& v : *this) somme += v;
        return count ? somme / static_cast<U>(count) : U{};
    }
};
```

### 3.2 Pipeline de traitement typé

```cpp
template<typename Input, typename Output>
class PipelineTyped {
    std::function<Output(const Input&)> transformation;
public:
    explicit PipelineTyped(std::function<Output(const Input&)> fn) : transformation(std::move(fn)) {}

    template<typename NextOutput>
    PipelineTyped<Input, NextOutput> puis(std::function<NextOutput(const Output&)> fn) const {
        auto capture = transformation;
        return PipelineTyped<Input, NextOutput>([capture, fn](const Input& in) {
            return fn(capture(in));
        });
    }

    Output executer(const Input& in) const { return transformation(in); }
};

// Usage
auto pipeline = PipelineTyped<std::string, std::string>([](const std::string& s){ return s; })
    .puis<int>([](const std::string& s) -> int { return static_cast<int>(s.size()); })
    .puis<std::string>([](const int& n) { return "Longueur: " + std::to_string(n); });
```

### 3.3 CI/CD complet

```yaml
name: Qualité

on: [push, pull_request]

jobs:
  build-matrix:
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        compiler: [g++, clang++]
        build_type: [Debug, Release]
    runs-on: ${{ matrix.os }}
    timeout-minutes: 20
    steps:
      - uses: actions/checkout@v4
      - name: Configure & Build
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DCMAKE_CXX_COMPILER=${{ matrix.compiler }}
          cmake --build build --parallel
      - name: Test
        run: ctest --test-dir build --output-on-failure
      - name: Sanitizers (Debug only)
        if: matrix.build_type == 'Debug'
        run: cmake --build build --target sanitized_tests
      - uses: actions/upload-artifact@v4
        if: failure()
        with:
          name: logs-${{ matrix.os }}-${{ matrix.compiler }}
          path: build/Testing

  static-analysis:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: cppcheck --enable=all --error-exitcode=1 src/
      - run: clang-tidy src/*.cpp -- -std=c++20

  format-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: clang-format --dry-run --Werror src/*.cpp include/**/*.hpp

  coverage:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: |
          cmake -B build -DENABLE_COVERAGE=ON
          cmake --build build
          ctest --test-dir build
          gcovr --root . --xml coverage.xml
      - uses: codecov/codecov-action@v4
        with: { files: coverage.xml }

  package:
    needs: [build-matrix, static-analysis]
    if: github.ref == 'refs/heads/main'
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: cmake -B build && cmake --build build && cd build && cpack -G DEB

  docs:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: doxygen Doxyfile
      - uses: peaceiris/actions-gh-pages@v3
        with: { github_token: "${{ secrets.GITHUB_TOKEN }}", publish_dir: ./docs/html }
```

---

## 🏆 Mini-projet du Jour 3 : Bibliothèque de sérialisation (architecture de correction)

```cpp
template<typename T>
concept Sérialisable = requires(T obj) {
    { obj.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(std::string{}) } -> std::same_as<T>;
};

class FormatJSON {
public:
    template<Sérialisable T>
    static std::string encoder(const T& obj) { return obj.serialize(); }  // délègue au format JSON de l'objet
};

template<typename Format>
class Serialiseur {
public:
    template<Sérialisable T>
    std::string serialiser(const T& obj) const { return Format::encoder(obj); }

    template<Sérialisable T>
    T deserialiser(const std::string& data) const { return T::deserialize(data); }

    template<Sérialisable T>
    void sauvegarder(const T& obj, const std::filesystem::path& p) const {
        std::ofstream out(p);
        out << serialiser(obj);
    }

    template<Sérialisable T>
    T charger(const std::filesystem::path& p) const {
        std::ifstream in(p);
        std::string contenu((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        return deserialiser<T>(contenu);
    }
};

// Exemple d'implémentation pour Livre
struct Livre {
    std::string isbn, titre, auteur;
    std::string serialize() const {
        return R"({"isbn":")" + isbn + R"(","titre":")" + titre + R"(","auteur":")" + auteur + "\"}";
    }
    static Livre deserialize(const std::string& data) {
        // parsing JSON simplifié (utiliser nlohmann::json en pratique)
        Livre l;
        // ... extraction des champs ...
        return l;
    }
};
```

---

## ✅ Points clés à retenir du Jour 3

- `if constexpr` remplace élégamment SFINAE pour du code conditionnel à la compilation, plus lisible.
- Les concepts C++20 donnent des messages d'erreur bien plus clairs qu'`enable_if`.
- Une hiérarchie d'exceptions métier doit toujours avoir un code d'erreur stable, exploitable en log/monitoring.
- `#pragma once` + forward declarations réduisent les temps de compilation et les dépendances circulaires.
- Un pipeline CI/CD complet combine build matrix, tests, couverture, analyse statique et packaging.
