# ✅ Corrections — Jour 2 : Mémoire, STL & Design Patterns
**Formation C++ · 5 jours · Corrigé des exercices**

---

## ⚡ Niveau 1 — Échauffement

### 1.1 Stack vs Heap

**1.1.a — Identifier la mémoire**

| Ligne | Stack / Heap | Justification |
|---|---|---|
| `int a = 5;` | Stack | Variable locale automatique |
| `int* p = new int(5);` | Heap (le pointeur `p` est sur la Stack, la donnée pointée sur le Heap) | `new` alloue dynamiquement |
| `std::string s = "hello";` | Stack (l'objet), Heap possible pour le buffer interne (SSO peut l'éviter) | Petites chaînes optimisées (SSO) |
| `auto v = std::make_unique<std::vector<int>>(100);` | Heap (le `vector` et son buffer), Stack pour `v` lui-même | `make_unique` alloue |
| `static double pi = 3.14159;` | Zone statique (ni Stack ni Heap) | Durée de vie = programme |
| `std::array<char, 256> buffer;` | Stack | `array` est toujours en place, taille fixe |
| `void* raw = malloc(1024);` | Heap | Allocation C brute |
| `std::shared_ptr<int> sp = std::make_shared<int>(42);` | Heap (bloc contrôle + valeur), Stack pour `sp` | Bloc de contrôle alloué une fois |

**Bonus** : allouer 10 millions d'`int` (~40 Mo) sur la Stack provoque un **stack overflow** (la Stack fait typiquement 1-8 Mo) → crash immédiat. Il faut utiliser le Heap (`std::vector`, `new[]`).

**1.1.b — Fuites mémoire**

```cpp
void fonctionCorrigee() {
    auto tab = std::make_unique<int[]>(100);          // Fix : plus de fuite si return anticipé
    auto msg = std::make_unique<std::string>("hello");

    if (tab[0] == 0) {
        return;   // Bug 1 corrigé : les unique_ptr se libèrent automatiquement
    }

    for (int i = 0; i < 100; i++) tab[i] = i;
    // Bug 2 corrigé : plus de delete manuel nécessaire
}

class Gestionnaire {
    std::vector<int> donnees;   // Bug 3 corrigé : Rule of Zero, plus de destructeur à écrire
public:
    Gestionnaire() : donnees(50) {}

    void reset() {
        donnees.assign(50, 0);  // Bug 4 corrigé : vector gère l'échec d'allocation par exception (bad_alloc),
                                 // pas de fuite ni de pointeur invalide en cas d'échec
    }
};
```

---

### 1.2 Pointeurs et références

**1.2.a — Tableau de pointeurs**

```cpp
int main() {
    int* ptrs[5];
    for (int i = 0; i < 5; ++i) ptrs[i] = new int(i * 10);

    for (int i = 0; i < 5; ++i)
        std::cout << "Valeur=" << *ptrs[i] << " adresse=" << ptrs[i]
                  << " adresse du pointeur=" << &ptrs[i] << "\n";

    for (int i = 0; i < 5; ++i) delete ptrs[i];   // libération correcte
}
```
```
Schéma :
ptrs[0] --> [ 0]   (heap)
ptrs[1] --> [10]
ptrs[2] --> [20]
...
```
*(Meilleure pratique moderne : `std::vector<std::unique_ptr<int>>` évite tout `delete` manuel.)*

**1.2.b — Pointeurs de fonctions**

```cpp
using OperationBinaire = int(*)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int div_(int a, int b) { return b != 0 ? a / b : 0; }

int main() {
    OperationBinaire ops[4] = {add, sub, mul, div_};
    std::cout << ops[0](3, 4) << "\n";   // 7

    std::vector<std::function<int(int, int)>> opsF = {add, sub, mul, div_};
    std::cout << opsF[2](3, 4) << "\n";  // 12
}
```
Réponse : `std::function` est **plus flexible** (accepte lambdas avec capture, foncteurs, méthodes liées) mais a un léger surcoût (indirection + éventuelle allocation heap pour les grosses captures). Le pointeur de fonction brut est **plus performant** (appel direct) mais limité aux fonctions sans état/capture.

---

### 1.3 Smart Pointers

**1.3.a — Convertir du code**

```cpp
struct Noeud {
    int valeur;
    std::unique_ptr<Noeud> gauche, droite;
    explicit Noeud(int v) : valeur(v) {}
};

std::unique_ptr<Noeud> creerArbre() {
    auto racine = std::make_unique<Noeud>(1);
    racine->gauche = std::make_unique<Noeud>(2);
    racine->droite = std::make_unique<Noeud>(3);
    racine->gauche->gauche = std::make_unique<Noeud>(4);
    return racine;
}
```

**1.3.b — Cycles avec weak_ptr**

```cpp
struct Sommet {
    std::string nom;
    std::vector<std::weak_ptr<Sommet>> voisins;
    void ajouterVoisin(std::shared_ptr<Sommet> v) { voisins.push_back(v); }
    void afficherVoisins() const {
        for (auto& w : voisins)
            if (auto sp = w.lock()) std::cout << nom << " -> " << sp->nom << "\n";
    }
};

int main() {
    auto a = std::make_shared<Sommet>(Sommet{"A", {}});
    auto b = std::make_shared<Sommet>(Sommet{"B", {}});
    auto c = std::make_shared<Sommet>(Sommet{"C", {}});
    a->ajouterVoisin(b); b->ajouterVoisin(c); c->ajouterVoisin(a);  // cycle A→B→C→A, sans fuite
    a->afficherVoisins();
}   // Tous les shared_ptr sont libérés proprement (compteur atteint 0), pas de fuite Valgrind
```

**1.3.c — unique_ptr dans un vecteur**

```cpp
std::vector<std::unique_ptr<Forme>> formes;
formes.push_back(std::make_unique<Cercle>(5.0));
formes.push_back(std::make_unique<Rectangle>(4.0, 3.0));

// formes2 = formes;         // ❌ ERREUR DE COMPILATION : unique_ptr non copiable
auto formes2 = std::move(formes); // ✅ OK : transfert, `formes` devient vide

std::cout << formes2[0]->aire() << "\n";     // accès + appel virtuel
formes2.erase(formes2.begin() + 1);          // suppression du 2e élément
for (const auto& f : formes2) std::cout << f->aire() << "\n";
```

---

### 1.4 vector et algorithmes

**1.4.a — Pipeline STL**

```cpp
#include <unordered_set>
#include <algorithm>
#include <numeric>

int main() {
    std::vector<int> v = /* 20 valeurs aléatoires 1-100 */ {};

    std::unordered_set<int> uniques(v.begin(), v.end());
    std::vector<int> sansDoublons(uniques.begin(), uniques.end());

    std::vector<int> pairs;
    std::copy_if(sansDoublons.begin(), sansDoublons.end(), std::back_inserter(pairs),
                 [](int x) { return x % 2 == 0; });

    std::transform(pairs.begin(), pairs.end(), pairs.begin(), [](int x) { return x * 3; });

    std::sort(pairs.begin(), pairs.end(), std::greater<int>());

    if (pairs.size() > 5) pairs.resize(5);

    int somme = std::accumulate(pairs.begin(), pairs.end(), 0);
    long long produit = std::accumulate(pairs.begin(), pairs.end(), 1LL, std::multiplies<>());

    std::cout << "Somme=" << somme << " Produit=" << produit << "\n";
}
```

**1.4.b — Performances vector**

```cpp
#include <chrono>

template<typename Fn>
long long mesurer(Fn fn) {
    auto d = std::chrono::high_resolution_clock::now();
    fn();
    auto f = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(f - d).count();
}

int main() {
    const int N = 1'000'000;

    auto t1 = mesurer([&]{ std::vector<int> v; for (int i = 0; i < N; ++i) v.push_back(i); });
    auto t2 = mesurer([&]{ std::vector<int> v; v.reserve(N); for (int i = 0; i < N; ++i) v.push_back(i); });
    auto t3 = mesurer([&]{ std::vector<int> v(N); for (int i = 0; i < N; ++i) v[i] = i; });
    auto t4 = mesurer([&]{ std::vector<int> v(N); std::iota(v.begin(), v.end(), 0); });

    std::cout << t1 << " " << t2 << " " << t3 << " " << t4 << " ms\n";
}
```
Explication : sans `reserve`, `push_back` provoque des réallocations successives (copie de tout le contenu à chaque doublement de capacité) → le plus lent. Avec `reserve`, une seule allocation. L'assignation directe après `resize` et `std::iota` évitent complètement la logique de croissance du vector et sont les plus rapides.

---

### 1.5 map et set

**1.5.a — Fréquences de mots**

```cpp
#include <unordered_map>
#include <sstream>
#include <cctype>

int main() {
    std::ifstream in("texte.txt");
    std::unordered_map<std::string, int> freq;
    std::string mot;
    while (in >> mot) {
        std::string nettoye;
        for (char c : mot) if (std::isalpha(static_cast<unsigned char>(c)))
            nettoye += static_cast<char>(std::tolower(c));
        if (!nettoye.empty()) freq[nettoye]++;
    }

    std::vector<std::pair<std::string, int>> tries(freq.begin(), freq.end());
    std::sort(tries.begin(), tries.end(), [](auto& a, auto& b) { return a.second > b.second; });

    std::cout << "Top 10 :\n";
    for (size_t i = 0; i < std::min<size_t>(10, tries.size()); ++i)
        std::cout << tries[i].first << " : " << tries[i].second << "\n";

    int uniques = std::count_if(freq.begin(), freq.end(), [](auto& p) { return p.second == 1; });
    std::cout << "Mots uniques : " << uniques << "\n";
    std::cout << "Mots distincts : " << freq.size() << "\n";
}
```
`unordered_map` est en moyenne O(1) par accès contre O(log n) pour `map` — plus rapide pour ce cas d'usage où l'ordre n'importe pas.

**1.5.b — Annuaire inversé**

```cpp
std::map<std::string, std::string> annuaire = { {"Alice", "0612345678"}, {"Bob", "0612345678"} };

std::multimap<std::string, std::string> inverse;
for (const auto& [nom, num] : annuaire) inverse.insert({num, nom});

auto range = inverse.equal_range("0612345678");
for (auto it = range.first; it != range.second; ++it) std::cout << it->second << " ";
```

---

## 🔥 Niveau 2 — Pratique

### 2.1 RAII

**2.1.a — Chronomètre RAII**

```cpp
#include <chrono>

class Chrono {
    std::string nom;
    std::chrono::high_resolution_clock::time_point debut;
    std::chrono::high_resolution_clock::duration accumule{};
    bool enPause = false;
public:
    explicit Chrono(std::string n) : nom(std::move(n)), debut(std::chrono::high_resolution_clock::now()) {}

    void pause() {
        if (!enPause) { accumule += std::chrono::high_resolution_clock::now() - debut; enPause = true; }
    }
    void reprendre() {
        if (enPause) { debut = std::chrono::high_resolution_clock::now(); enPause = false; }
    }

    ~Chrono() {
        if (!enPause) accumule += std::chrono::high_resolution_clock::now() - debut;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(accumule).count();
        std::cout << nom << " : " << ms << " ms\n";
    }
};
```

**2.1.b — ScopedLock maison**

```cpp
class ScopedLock {
    std::mutex& mtx;
public:
    explicit ScopedLock(std::mutex& m) : mtx(m) { mtx.lock(); }
    ~ScopedLock() { mtx.unlock(); }
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
};

// Test
std::mutex mtx;
int compteur = 0;
void travail() { for (int i = 0; i < 100000; ++i) { ScopedLock lock(mtx); ++compteur; } }

int main() {
    std::thread t1(travail), t2(travail), t3(travail);
    t1.join(); t2.join(); t3.join();
    std::cout << compteur << "\n";  // 300000 garanti
}
```

**2.1.c — Gestionnaire de fichier RAII**

```cpp
#include <filesystem>
#include <fstream>
#include <random>

class FichierTemp {
    std::filesystem::path chemin;
    std::ofstream stream;
public:
    explicit FichierTemp(const std::string& prefixe = "tmp") {
        std::random_device rd;
        chemin = std::filesystem::temp_directory_path() /
                 (prefixe + "_" + std::to_string(rd()) + ".tmp");
        stream.open(chemin);
    }
    ~FichierTemp() { stream.close(); std::filesystem::remove(chemin); }

    FichierTemp(const FichierTemp&) = delete;
    FichierTemp& operator=(const FichierTemp&) = delete;
    FichierTemp(FichierTemp&&) noexcept = default;
    FichierTemp& operator=(FichierTemp&&) noexcept = default;

    std::ostream& getStream() { return stream; }
    std::string getChemin() const { return chemin.string(); }
};
```

---

### 2.2 Move Semantics

**2.2.a — Buffer déplaçable**

```cpp
class Buffer {
    std::unique_ptr<uint8_t[]> data;
    size_t taille;
public:
    explicit Buffer(size_t n) : data(std::make_unique<uint8_t[]>(n)), taille(n) {}

    Buffer(const Buffer& o) : data(std::make_unique<uint8_t[]>(o.taille)), taille(o.taille) {
        std::copy(o.data.get(), o.data.get() + taille, data.get());
    }
    Buffer(Buffer&& o) noexcept : data(std::move(o.data)), taille(o.taille) { o.taille = 0; }

    Buffer& operator=(const Buffer& o) {
        if (this != &o) { data = std::make_unique<uint8_t[]>(o.taille); taille = o.taille;
                           std::copy(o.data.get(), o.data.get() + taille, data.get()); }
        return *this;
    }
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) { data = std::move(o.data); taille = o.taille; o.taille = 0; }
        return *this;
    }

    void remplir(uint8_t v) { std::fill(data.get(), data.get() + taille, v); }
    size_t getTaille() const { return taille; }
    uint8_t& operator[](size_t i) { return data[i]; }
    const uint8_t& operator[](size_t i) const { return data[i]; }
};
```
Sur un Buffer de 100 Mo : la copie est O(n) (~des dizaines de ms) alors que le move est O(1) (juste un échange de pointeurs, quelques nanosecondes).

**2.2.b — Rule of Five vs Rule of Zero**

```cpp
class MatriceV1 {
    double* data; int lignes, colonnes;
public:
    MatriceV1(int l, int c) : data(new double[l * c]()), lignes(l), colonnes(c) {}
    ~MatriceV1() { delete[] data; }
    MatriceV1(const MatriceV1& o) : data(new double[o.lignes * o.colonnes]), lignes(o.lignes), colonnes(o.colonnes) {
        std::copy(o.data, o.data + lignes * colonnes, data);
    }
    MatriceV1(MatriceV1&& o) noexcept : data(o.data), lignes(o.lignes), colonnes(o.colonnes) {
        o.data = nullptr;
    }
    MatriceV1& operator=(const MatriceV1& o) {
        if (this != &o) { delete[] data; lignes = o.lignes; colonnes = o.colonnes;
                           data = new double[lignes * colonnes];
                           std::copy(o.data, o.data + lignes * colonnes, data); }
        return *this;
    }
    MatriceV1& operator=(MatriceV1&& o) noexcept {
        if (this != &o) { delete[] data; data = o.data; lignes = o.lignes; colonnes = o.colonnes; o.data = nullptr; }
        return *this;
    }
};

class MatriceV2 {
    std::vector<double> data; int lignes, colonnes;
public:
    MatriceV2(int l, int c) : data(l * c, 0.0), lignes(l), colonnes(c) {}
    // Rien d'autre à écrire : le compilateur génère copie/move/destructeur corrects.
};
```
Les deux versions sont sans fuite sous Valgrind ; `MatriceV2` est nettement moins risquée à maintenir (Rule of Zero recommandée par défaut).

---

### 2.3 STL avancé

**2.3.a — Transformer une collection**

```cpp
struct Produit { std::string ref, nom; double prix; int stock; std::string categorie; };

std::vector<Produit> disponibles;
std::copy_if(catalogue.begin(), catalogue.end(), std::back_inserter(disponibles),
             [](const Produit& p) { return p.stock > 0 && p.prix < 50; });

std::sort(disponibles.begin(), disponibles.end(), [](const Produit& a, const Produit& b) {
    if (a.prix != b.prix) return a.prix < b.prix;
    return a.nom < b.nom;
});

std::map<std::string, std::vector<Produit>> parCategorie;
for (const auto& p : catalogue) parCategorie[p.categorie].push_back(p);

std::map<std::string, double> prixMoyen;
for (const auto& [cat, produits] : parCategorie) {
    double somme = std::accumulate(produits.begin(), produits.end(), 0.0,
                                    [](double acc, const Produit& p) { return acc + p.prix; });
    prixMoyen[cat] = somme / produits.size();
}

auto plusCher = std::max_element(catalogue.begin(), catalogue.end(), [](const Produit& a, const Produit& b) {
    if (a.stock <= 0) return true;
    if (b.stock <= 0) return false;
    return a.prix < b.prix;
});

std::for_each(catalogue.begin(), catalogue.end(), [](Produit& p) {
    if (p.prix > 100) p.prix *= 0.9;
});
```

**2.3.b — Algorithmes génériques**

```cpp
template<typename It>
It second_max(It first, It last) {
    It max1 = first, max2 = last;
    for (It it = first; it != last; ++it) {
        if (*it > *max1) { max2 = max1; max1 = it; }
        else if (max2 == last || *it > *max2) max2 = it;
    }
    return max2;
}

template<typename It, typename KeyFn>
auto groupBy(It first, It last, KeyFn fn) {
    std::map<decltype(fn(*first)), std::vector<typename std::iterator_traits<It>::value_type>> res;
    for (It it = first; it != last; ++it) res[fn(*it)].push_back(*it);
    return res;
}

template<typename It1, typename It2, typename OutIt, typename Fn>
void zip_transform(It1 f1, It1 l1, It2 f2, OutIt out, Fn fn) {
    for (; f1 != l1; ++f1, ++f2, ++out) *out = fn(*f1, *f2);
}
```

---

### 2.4 Singleton et Factory

**2.4.a — Logger Singleton**

```cpp
class Logger {
public:
    enum class Niveau { DEBUG, INFO, WARN, ERROR };

    static Logger& getInstance() {
        static Logger instance;   // Meyers' Singleton — thread-safe depuis C++11
        return instance;
    }

    void setNiveauMinimum(Niveau n) { niveauMin = n; }
    void setFichier(const std::string& path) { fichier.open(path, std::ios::app); }

    void log(Niveau n, const std::string& msg) {
        if (n < niveauMin) return;
        std::lock_guard<std::mutex> lock(mtx);
        (fichier.is_open() ? fichier : std::cout) << "[" << static_cast<int>(n) << "] " << msg << "\n";
    }

private:
    Logger() = default;
    Niveau niveauMin = Niveau::DEBUG;
    std::ofstream fichier;
    std::mutex mtx;
};
#define LOG_DEBUG(msg) Logger::getInstance().log(Logger::Niveau::DEBUG, msg)
```
*(La construction locale-statique de `getInstance()` est thread-safe garantie par le standard depuis C++11 — plus simple que `call_once` manuel.)*

**2.4.b — Factory de parseurs**

```cpp
class ParserFactory {
    using Createur = std::function<std::unique_ptr<IParser>()>;
    std::unordered_map<std::string, Createur> createurs;
public:
    void enregistrer(const std::string& format, Createur fn) { createurs[format] = std::move(fn); }

    std::unique_ptr<IParser> creer(const std::string& format) const {
        auto it = createurs.find(format);
        if (it == createurs.end()) throw std::invalid_argument("Format inconnu : " + format);
        return it->second();
    }

    std::vector<std::string> formatsDisponibles() const {
        std::vector<std::string> v;
        for (auto& [nom, fn] : createurs) v.push_back(nom);
        return v;
    }
};

// Enregistrement
ParserFactory factory;
factory.enregistrer("json", []{ return std::make_unique<ParserJSON>(); });
factory.enregistrer("csv",  []{ return std::make_unique<ParserCSV>(); });
```

---

### 2.5 Observer et Strategy

**2.5.a — Alertes météo**

```cpp
void afficheNumerique(const DonneesMeteo& d) {
    std::cout << "T=" << d.temperature << "°C H=" << d.humidite << "% V=" << d.vent << "km/h\n";
}
void alerteChaleur(const DonneesMeteo& d) { if (d.temperature > 35.0) std::cout << "ALERTE CHALEUR\n"; }
void alerteGel(const DonneesMeteo& d)     { if (d.temperature < 0.0) std::cout << "ALERTE GEL\n"; }

class Historique {
    std::deque<DonneesMeteo> mesures;
public:
    void operator()(const DonneesMeteo& d) {
        mesures.push_back(d);
        if (mesures.size() > 100) mesures.pop_front();
    }
};

// station.abonner(afficheNumerique); station.abonner(alerteChaleur); ...
```

**2.5.b — Stratégie de tri**

```cpp
void GestionnaireEmployes::setStrategie(const std::string& critere) {
    if (critere == "nom") comparateur = [](auto& a, auto& b) { return a.nom < b.nom; };
    else if (critere == "salaire_asc") comparateur = [](auto& a, auto& b) { return a.salaire < b.salaire; };
    else if (critere == "salaire_desc") comparateur = [](auto& a, auto& b) { return a.salaire > b.salaire; };
    else if (critere == "anciennete") comparateur = [](auto& a, auto& b) { return a.anciennete > b.anciennete; };
    else if (critere == "departement_puis_salaire")
        comparateur = [](auto& a, auto& b) {
            if (a.departement != b.departement) return a.departement < b.departement;
            return a.salaire > b.salaire;
        };
}
void GestionnaireEmployes::trier() { std::sort(employes.begin(), employes.end(), comparateur); }
```

---

## 🚀 Niveau 3 — Défi

### 3.1 Pool d'allocation (esquisse)

```cpp
template<typename T>
class PoolAllocateur {
    struct Bloc { alignas(T) char data[sizeof(T)]; };
    std::shared_ptr<std::vector<Bloc>> pool;
    std::shared_ptr<std::stack<T*>> libres;
public:
    using value_type = T;

    explicit PoolAllocateur(size_t capacite)
        : pool(std::make_shared<std::vector<Bloc>>(capacite)),
          libres(std::make_shared<std::stack<T*>>()) {
        for (auto& b : *pool) libres->push(reinterpret_cast<T*>(&b));
    }

    T* allocate(size_t n) {
        if (n != 1 || libres->empty()) throw std::bad_alloc();
        T* p = libres->top(); libres->pop();
        return p;
    }
    void deallocate(T* p, size_t) { libres->push(p); }

    template<typename U> struct rebind { using other = PoolAllocateur<U>; };
};
```
*(Un allocateur de pool évite l'appel système `malloc`/`free` répété : gain typique de 3-8× sur de petites allocations fréquentes de même taille, mesurable avec Google Benchmark.)*

### 3.2 Liste doublement chaînée générique (points clés)

```cpp
template<typename T>
class ListeChainee {
    struct Noeud {
        T valeur;
        std::unique_ptr<Noeud> suivant;
        Noeud* precedent = nullptr;
        explicit Noeud(T v) : valeur(std::move(v)) {}
    };
    std::unique_ptr<Noeud> tete;
    Noeud* queue = nullptr;
    size_t count = 0;

public:
    ListeChainee() = default;

    void push_back(const T& val) {
        auto n = std::make_unique<Noeud>(val);
        n->precedent = queue;
        Noeud* raw = n.get();
        if (queue) queue->suivant = std::move(n); else tete = std::move(n);
        queue = raw;
        ++count;
    }

    void push_front(const T& val) {
        auto n = std::make_unique<Noeud>(val);
        n->suivant = std::move(tete);
        if (n->suivant) n->suivant->precedent = n.get();
        tete = std::move(n);
        if (!queue) queue = tete.get();
        ++count;
    }

    T& front() { return tete->valeur; }
    T& back() { return queue->valeur; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    class iterator {
        Noeud* n;
    public:
        explicit iterator(Noeud* p) : n(p) {}
        T& operator*() { return n->valeur; }
        iterator& operator++() { n = n->suivant.get(); return *this; }
        bool operator!=(const iterator& o) const { return n != o.n; }
    };
    iterator begin() { return iterator(tete.get()); }
    iterator end() { return iterator(nullptr); }
};
```
Cette structure est compatible avec `std::for_each` et `std::copy` grâce à son itérateur minimal (à étendre en bidirectionnel avec `operator--` pour une compatibilité complète avec `std::sort`, qui exige un itérateur à accès aléatoire — non satisfait ici nativement, d'où un `sort()` interne dédié).

### 3.3 Plugin Registry (points clés)

```cpp
class PluginRegistry {
    using Createur = std::function<std::unique_ptr<IPlugin>()>;
    std::unordered_map<std::string, Createur> createurs;
    PluginRegistry() = default;
public:
    static PluginRegistry& getInstance() { static PluginRegistry inst; return inst; }
    bool enregistrer(const std::string& nom, Createur fn) {
        createurs[nom] = std::move(fn);
        return true;
    }
    std::unique_ptr<IPlugin> creer(const std::string& nom) const {
        auto it = createurs.find(nom);
        return it != createurs.end() ? it->second() : nullptr;
    }
};

class PluginMajuscules : public IPlugin {
public:
    std::string nom() const override { return "majuscules"; }
    std::string version() const override { return "1.0"; }
    void initialiser(const nlohmann::json&) override {}
    void executer(const std::string& in, std::ostream& out) override {
        std::string s = in;
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        out << s;
    }
};
REGISTER_PLUGIN("majuscules", PluginMajuscules);
```

---

## 🏆 Mini-projet du Jour 2 : Gestionnaire de tâches — Architecture de correction

```cpp
class DepotTaches {
    std::vector<Tache> taches;
    std::string fichierJSON;
    std::vector<std::function<void(const Tache&)>> observateurs;
public:
    void ajouter(Tache t) {
        taches.push_back(std::move(t));
        for (auto& obs : observateurs) if (taches.back().statut == Statut::EnCours) obs(taches.back());
    }

    std::optional<Tache> trouver(int id) const {
        auto it = std::find_if(taches.begin(), taches.end(), [id](const Tache& t) { return t.id == id; });
        return it != taches.end() ? std::optional<Tache>(*it) : std::nullopt;
    }

    std::vector<std::reference_wrapper<const Tache>> filtrerParStatut(Statut s) const {
        std::vector<std::reference_wrapper<const Tache>> res;
        for (const auto& t : taches) if (t.statut == s) res.push_back(t);
        return res;
    }

    std::map<Statut, int> statistiques() const {
        std::map<Statut, int> stats;
        for (const auto& t : taches) stats[t.statut]++;
        return stats;
    }
};
```
Le Strategy pattern pour le tri suit le même modèle que 2.5.b (un `std::function<bool(const Tache&, const Tache&)>` configurable selon "priorité", "date", "titre").

---

## ✅ Points clés à retenir du Jour 2

- Zéro `new`/`delete` nu dans le code applicatif : `unique_ptr` par défaut, `shared_ptr` seulement en cas de partage réel.
- `weak_ptr` casse les cycles de référence entre `shared_ptr`.
- `reserve()` avant une série de `push_back` évite les réallocations coûteuses.
- Le Meyers' Singleton (variable statique locale) est la manière la plus simple et thread-safe d'implémenter un singleton en C++11+.
- `std::function` + lambda remplace élégamment Strategy et Observer sans hiérarchie de classes lourde.
