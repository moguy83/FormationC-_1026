# 📘 Exercices C++ — Jour 4 : C++ Moderne, Sécurité & Performance
**Formation C++ · 5 jours · Niveau débutant–intermédiaire**

---

## 📋 Sommaire

- [Objectifs du jour](#objectifs)
- [⚡ Niveau 1 — Échauffement](#niveau-1) *(~30 min)*
  - [1.1 Lambdas et closures](#11-lambdas)
  - [1.2 auto, structured bindings, if constexpr](#12-syntaxe-moderne)
  - [1.3 std::optional, variant, any](#13-types-modernes)
  - [1.4 Sécurité mémoire — détecter les bugs](#14-sécurité)
  - [1.5 std::thread et mutex — premiers pas](#15-threading-bases)
- [🔥 Niveau 2 — Pratique](#niveau-2) *(~60 min)*
  - [2.1 Lambdas avancées et std::function](#21-lambdas-avancées)
  - [2.2 Ranges C++20](#22-ranges)
  - [2.3 Validation et sanitisation des entrées](#23-validation)
  - [2.4 Synchronisation — mutex, condition_variable](#24-synchronisation)
  - [2.5 Futures et async](#25-futures)
- [🚀 Niveau 3 — Défi](#niveau-3) *(~90 min)*
  - [3.1 ThreadPool complet](#31-threadpool)
  - [3.2 Auditer et corriger un code non sécurisé](#32-audit-sécurité)
  - [3.3 Benchmark et optimisation](#33-benchmark)
- [🏆 Mini-projet du Jour 4](#mini-projet)
- [💡 Indices et solutions partielles](#indices)
- [✅ Auto-évaluation](#auto-évaluation)

---

## 🎯 Objectifs du jour {#objectifs}

À l'issue de ces exercices, vous serez capable de :
- Écrire des lambdas complexes avec captures et perfect forwarding
- Exploiter les fonctionnalités modernes C++17/20 (ranges, structured bindings)
- Identifier et corriger les vulnérabilités de sécurité mémoire courantes
- Implémenter des programmes multithreadés corrects (sans data race)
- Utiliser `std::async`, `std::future`, `std::promise` pour le code asynchrone
- Mesurer et optimiser les performances avec les bons outils

---

## ⚡ Niveau 1 — Échauffement {#niveau-1}

---

### 1.1 Lambdas et closures {#11-lambdas}

**Exercice 1.1.a — Comprendre les captures**

Pour chaque lambda, indiquez ce qui sera affiché et pourquoi :

```cpp
int x = 10;

// Lambda 1 : capture par valeur
auto l1 = [x]() { std::cout << x << "\n"; };
x = 20;
l1();  // Affiche ?

// Lambda 2 : capture par référence
auto l2 = [&x]() { std::cout << x << "\n"; };
x = 30;
l2();  // Affiche ?

// Lambda 3 : mutable
auto l3 = [x]() mutable {
    x++;
    std::cout << x << "\n";
};
l3();  l3();  l3();  // Affiche ? Et x vaut ?

// Lambda 4 : capture par valeur tout
int a = 1, b = 2;
auto l4 = [=]() { std::cout << a + b << "\n"; };
a = 10; b = 20;
l4();  // Affiche ?

// Lambda 5 : init capture C++14
auto ptr = std::make_unique<int>(42);
auto l5 = [p = std::move(ptr)]() { std::cout << *p << "\n"; };
// Peut-on appeler l5 deux fois ? ptr est-il encore valide ?
```

---

**Exercice 1.1.b — Lambdas comme callbacks**

```cpp
// Implémenter un système d'événements avec lambdas
class Bouton {
    std::string label;
    std::vector<std::function<void()>> onClic;
    std::vector<std::function<void(std::string)>> onSurvol;

public:
    explicit Bouton(std::string l) : label(std::move(l)) {}

    void ajouterClic(std::function<void()> fn)           { onClic.push_back(fn); }
    void ajouterSurvol(std::function<void(std::string)> fn) { onSurvol.push_back(fn); }

    void simulerClic()              { for (auto& f : onClic) f(); }
    void simulerSurvol(std::string msg) { for (auto& f : onSurvol) f(msg); }
};

// Test :
int clics = 0;
Bouton btn("OK");
btn.ajouterClic([&clics]() { clics++; std::cout << "Clic!\n"; });
btn.ajouterClic([]() { std::cout << "Deuxième handler\n"; });
btn.ajouterSurvol([](const std::string& m) { std::cout << "Survol: " << m << "\n"; });

btn.simulerClic();    // Affiche ?
btn.simulerClic();    // clics vaut ?
btn.simulerSurvol("Enregistrer le document");
```

---

### 1.2 Syntaxe moderne {#12-syntaxe-moderne}

**Exercice 1.2.a — Structured bindings**

```cpp
// Réécrire avec des structured bindings

// Avant :
std::map<std::string, int> scores = {{"Alice",90}, {"Bob",85}};
for (auto it = scores.begin(); it != scores.end(); ++it) {
    std::cout << it->first << " : " << it->second << "\n";
}

// Après : (à compléter)

// Aussi avec std::tuple :
auto getDimensions() { return std::make_tuple(1920, 1080, 32); }

// Avant :
auto dims = getDimensions();
int w = std::get<0>(dims);
int h = std::get<1>(dims);
int bpp = std::get<2>(dims);

// Après : (à compléter avec structured binding)

// Avec std::minmax_element :
std::vector<int> v = {5, 2, 8, 1, 9, 3};
auto [min_it, max_it] = std::minmax_element(v.begin(), v.end());
// Afficher min et max
```

---

**Exercice 1.2.b — if constexpr en pratique**

```cpp
// Implémenter une fonction toString générique
template<typename T>
std::string toString(const T& val) {
    if constexpr (std::is_same_v<T, bool>) {
        return val ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val;
    } else if constexpr (requires { val.begin(); val.end(); }) {
        // Container → afficher entre []
        std::string result = "[";
        bool first = true;
        for (const auto& elem : val) {
            if (!first) result += ", ";
            result += toString(elem);  // récursif !
            first = false;
        }
        return result + "]";
    } else {
        return "?";
    }
}

// Tests :
// toString(true) → "true"
// toString(42) → "42"
// toString(std::vector<int>{1,2,3}) → "[1, 2, 3]"
// toString(std::string("hello")) → "hello"
```

---

### 1.3 Types modernes {#13-types-modernes}

**Exercice 1.3.a — std::optional**

```cpp
// Refactorer pour utiliser optional plutôt que des pointeurs/sentinelles

// AVANT (patterns à éliminer)
int* trouverMax(const std::vector<int>& v) {
    if (v.empty()) return nullptr;
    // ...
}

int chercherIndex(const std::string& s, char c) {
    // retourne -1 si non trouvé (magic number !)
}

// APRÈS avec optional
std::optional<int> trouverMax(const std::vector<int>& v);
std::optional<size_t> chercherIndex(const std::string& s, char c);

// Utiliser aussi :
// - value_or() pour les valeurs par défaut
// - has_value() pour la vérification
// - transform() et and_then() (C++23 pour les chainages)
```

---

**Exercice 1.3.b — std::variant et visitor**

```cpp
// Système de configuration typée
using ValeurConfig = std::variant<int, double, bool, std::string,
                                   std::vector<int>>;

class Config {
    std::map<std::string, ValeurConfig> parametres;

public:
    template<typename T>
    void set(const std::string& cle, T val) {
        parametres[cle] = std::move(val);
    }

    template<typename T>
    std::optional<T> get(const std::string& cle) const;

    // Afficher toutes les valeurs avec std::visit
    void afficher() const;

    // Sérialiser en JSON (chaque type a son format)
    std::string toJSON() const;
};

// Tests :
Config cfg;
cfg.set("port", 8080);
cfg.set("debug", true);
cfg.set("nom", std::string("MonApp"));
cfg.set("ports", std::vector<int>{80, 443, 8080});

cfg.afficher();
// port = 8080 (int)
// debug = true (bool)
// nom = "MonApp" (string)
// ports = [80, 443, 8080] (vector)
```

---

### 1.4 Sécurité mémoire {#14-sécurité}

**Exercice 1.4.a — Identifier les vulnérabilités**

Pour chaque code, identifiez la vulnérabilité, son type, et proposez la correction :

```cpp
// Code 1
void copierNom(const char* source) {
    char dest[32];
    strcpy(dest, source);  // Vulnérabilité ?
}

// Code 2
int tab[10];
for (int i = 0; i <= 10; i++)  // Vulnérabilité ?
    tab[i] = i;

// Code 3
int* creer() { int local = 42; return &local; }  // Vulnérabilité ?

// Code 4
int* p = new int(5);
delete p;
std::cout << *p;  // Vulnérabilité ?

// Code 5
int a = INT_MAX;
int b = a + 1;  // Vulnérabilité ?
std::cout << b;

// Code 6
std::string query = "SELECT * FROM users WHERE name='" + nom + "'";
executerSQL(query);  // Vulnérabilité ?
```

---

### 1.5 Threading — bases {#15-threading-bases}

**Exercice 1.5.a — Premier thread**

```cpp
// 1. Créer un thread qui affiche les nombres de 1 à 10
// 2. Créer un thread qui calcule la somme de 1 à 1000000
// 3. Récupérer le résultat du thread (utiliser une variable partagée)
// 4. Mesurer le temps d'exécution de chaque thread

// Sans regarder la réponse, que se passe-t-il si on oublie t.join() ?
// Que se passe-t-il si on appelle t.join() deux fois ?
```

---

**Exercice 1.5.b — Data race**

```cpp
// Ce code a un problème — lequel ?
int compteur = 0;

void incrementer(int n) {
    for (int i = 0; i < n; i++) compteur++;
}

int main() {
    std::thread t1(incrementer, 100000);
    std::thread t2(incrementer, 100000);
    t1.join(); t2.join();
    std::cout << compteur << "\n";  // Toujours 200000 ?
}

// Corriger en utilisant :
// Version 1 : std::mutex + lock_guard
// Version 2 : std::atomic<int>
// Comparer les performances des deux versions
```

---

## 🔥 Niveau 2 — Pratique {#niveau-2}

---

### 2.1 Lambdas avancées {#21-lambdas-avancées}

**Exercice 2.1 — Monade de transformation**

Implémentez `Transformable<T>` : un wrapper qui permet de chaîner des transformations avec des lambdas.

```cpp
template<typename T>
class Transformable {
    T valeur;

public:
    explicit Transformable(T v) : valeur(std::move(v)) {}

    // Appliquer une transformation
    template<typename Fn>
    auto map(Fn fn) const -> Transformable<decltype(fn(valeur))> {
        return Transformable<decltype(fn(valeur))>(fn(valeur));
    }

    // Appliquer si condition vraie
    Transformable filter(std::function<bool(const T&)> pred,
                          T defaut = T{}) const;

    // Exécuter un side-effect sans changer la valeur
    Transformable& tap(std::function<void(const T&)> fn);

    const T& get() const { return valeur; }
};

// Usage attendu :
auto resultat = Transformable<std::string>("  HELLO, WORLD!  ")
    .map([](std::string s) { /* trim */ return s; })
    .map([](std::string s) { std::transform(s.begin(), s.end(), s.begin(), ::tolower); return s; })
    .tap([](const std::string& s) { std::cout << "Debug: " << s << "\n"; })
    .map([](std::string s) { std::replace(s.begin(), s.end(), ' ', '_'); return s; })
    .get();
// → "hello,_world!"
```

---

### 2.2 Ranges C++20 {#22-ranges}

**Exercice 2.2 — Pipeline de données avec Ranges**

```cpp
#include <ranges>

struct LogEntry {
    std::string timestamp;
    std::string niveau;
    std::string message;
    std::string source;
};

std::vector<LogEntry> logs = chargerLogs("app.log");

// Implémenter ces requêtes UNIQUEMENT avec std::views (pas de boucles) :

// 1. Tous les messages ERROR des 24 dernières heures
auto erreurs = logs | /* à compléter */;

// 2. Les 10 derniers messages de la source "database"
auto derniersBDD = logs | /* à compléter */;

// 3. Compter les messages par niveau
// (hint: pas de groupBy natif — utiliser fold + map)

// 4. Les messages contenant "timeout" (insensible à la casse)
auto timeouts = logs | /* à compléter */;

// 5. Transformer en format CSV
auto csv = logs
    | std::views::transform([](const LogEntry& e) {
          return e.timestamp + "," + e.niveau + "," + e.message;
      })
    | /* joindre avec \n */;
```

---

### 2.3 Validation et sanitisation {#23-validation}

**Exercice 2.3 — InputValidator robuste**

```cpp
class InputValidator {
public:
    // Résultat de validation
    struct ResultatValidation {
        bool valide;
        std::string messageErreur;
        std::string valeurSanitisee;
    };

    // Valider et sanitiser
    static ResultatValidation validerEmail(const std::string& email);
    static ResultatValidation validerURL(const std::string& url);
    static ResultatValidation validerTelephoneFR(const std::string& tel);
    static ResultatValidation validerCodePostalFR(const std::string& cp);
    static ResultatValidation validerDateISO(const std::string& date);

    // Sanitisation (toujours sans valeur de retour conditionnelle)
    static std::string sanitiserHTML(const std::string& html);
    static std::string sanitiserSQL(const std::string& val);  // échappement
    static std::string sanitiserNomFichier(const std::string& nom);
    static std::string sanitiserJSON(const std::string& val);

    // Valider une plage numérique
    template<typename T>
    static ResultatValidation validerPlage(T val, T min, T max,
                                            const std::string& nomChamp);
};
```

Testez avec des cas normaux ET des inputs malicieux (XSS, SQL injection, path traversal).

---

### 2.4 Synchronisation {#24-synchronisation}

**Exercice 2.4.a — File de messages thread-safe**

```cpp
template<typename T>
class FileMessages {
    std::queue<T> file;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> fermee{false};

public:
    // Producteur : ajoute un message
    void envoyer(T msg);

    // Consommateur : attend et retire un message
    // Retourne nullopt si la file est fermée et vide
    std::optional<T> recevoir();

    // Consommateur non-bloquant
    std::optional<T> tenterRecevoir();

    // Fermer la file (signale aux consommateurs)
    void fermer();

    size_t taille() const;
    bool estFermee() const;
};
```

Testez avec 3 producteurs et 2 consommateurs pendant 5 secondes.

---

**Exercice 2.4.b — Barrière de synchronisation**

```cpp
// Implémenter une barrière cyclique :
// N threads arrivent, tous attendent que le dernier arrive,
// puis tous repartent ensemble (se répète K fois)

class BarriereSync {
    int n;  // nombre de threads participants
    std::atomic<int> compteur;
    std::mutex mtx;
    std::condition_variable cv;
    int generation{0};  // évite les spurious wakeups inter-cycles

public:
    explicit BarriereSync(int n);

    void attendre();  // bloque jusqu'à ce que n threads soient arrivés
};

// Test :
// 4 threads, chacun effectue 3 phases
// Chaque thread affiche "Thread X - Phase Y : début/fin"
// Vérifier que toutes les fins de la phase Y apparaissent
// avant les débuts de la phase Y+1
```

---

### 2.5 Futures et async {#25-futures}

**Exercice 2.5 — Téléchargement parallèle simulé**

```cpp
// Simuler le téléchargement parallèle de plusieurs ressources
struct Ressource {
    std::string url;
    std::vector<uint8_t> contenu;
    size_t taille;
    std::chrono::milliseconds duree;
};

// Télécharger une ressource (simulé avec sleep + génération aléatoire)
Ressource telecharger(const std::string& url);

// Version séquentielle
std::vector<Ressource> telechargerTous_seq(
    const std::vector<std::string>& urls);

// Version parallèle avec std::async
std::vector<Ressource> telechargerTous_async(
    const std::vector<std::string>& urls);

// Version avec limite de concurrence (max 4 downloads simultanés)
std::vector<Ressource> telechargerTous_limite(
    const std::vector<std::string>& urls, int maxConcurrent);
```

Mesurez le speedup de la version parallèle vs séquentielle pour 20 URLs.

---

## 🚀 Niveau 3 — Défi {#niveau-3}

---

### 3.1 ThreadPool complet {#31-threadpool}

**Exercice 3.1 — ThreadPool avec futures et priorités**

```cpp
class ThreadPool {
    struct Tache {
        std::function<void()> fn;
        int priorite;
        // Comparer par priorité pour priority_queue
        bool operator<(const Tache& o) const { return priorite < o.priorite; }
    };

    std::vector<std::jthread> workers;
    std::priority_queue<Tache> file;
    std::mutex mtx;
    std::condition_variable cv;

public:
    explicit ThreadPool(size_t nbThreads);

    // Soumettre une tâche et récupérer un future
    template<typename Fn, typename... Args>
    auto soumettre(int priorite, Fn&& fn, Args&&... args)
        -> std::future<std::invoke_result_t<Fn, Args...>>;

    // Attendre que toutes les tâches soient terminées
    void attendreFin();

    // Statistiques
    size_t tacheEnAttente() const;
    size_t threadsActifs() const;
};

// Test :
ThreadPool pool(4);
std::vector<std::future<int>> futures;
for (int i = 0; i < 20; i++) {
    int prio = i % 3;  // 0=basse, 1=normale, 2=haute
    futures.push_back(pool.soumettre(prio, [i]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10*i % 50));
        return i * i;
    }));
}
for (auto& f : futures) std::cout << f.get() << " ";
```

---

### 3.2 Audit de sécurité {#32-audit-sécurité}

**Exercice 3.2 — Corriger un serveur HTTP simplifié bugué**

Le code suivant implémente un mini-serveur HTTP. Identifiez et corrigez **au moins 8 vulnérabilités** :

```cpp
class ServeurHTTP {
    int port;
    char buffer[1024];  // Bug 1 ?

public:
    void traiterRequete(int socket) {
        recv(socket, buffer, sizeof(buffer), 0);  // Bug 2 ?

        // Parser la ligne de requête
        char methode[16], chemin[256], version[16];
        sscanf(buffer, "%s %s %s", methode, chemin, version);  // Bug 3 ?

        // Lire le fichier demandé
        std::string fichier = "/var/www/html" + std::string(chemin);  // Bug 4 ?
        FILE* f = fopen(fichier.c_str(), "r");

        char contenu[8192];
        fread(contenu, 1, sizeof(contenu), f);  // Bug 5 ?

        // Construire la réponse
        char reponse[10240];
        sprintf(reponse,                          // Bug 6 ?
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %d\r\n\r\n%s",
            strlen(contenu), contenu);

        send(socket, reponse, strlen(reponse), 0);
        fclose(f);
    }

    void demarrer() {
        while (true) {
            int client = accept(/* ... */);
            traiterRequete(client);  // Bug 7 ?
        }
    }
};
```

Pour chaque bug : nommez la vulnérabilité, son impact potentiel (CWE), et montrez la correction.

---

### 3.3 Benchmark et optimisation {#33-benchmark}

**Exercice 3.3 — Optimiser une fonction de recherche**

```cpp
// Version naïve à optimiser
struct Article {
    int id;
    std::string reference;
    std::string nom;
    std::string categorie;
    double prix;
    int stock;
    std::vector<std::string> tags;
};

class CatalogueNaif {
    std::vector<Article> articles;  // 100 000 articles

public:
    // Benchmark ces fonctions :
    std::vector<Article> rechercherParCategorie(const std::string& cat) const;
    std::optional<Article> trouverParRef(const std::string& ref) const;
    std::vector<Article> rechercherParTag(const std::string& tag) const;
    std::vector<Article> rechercherParPrix(double min, double max) const;
};
```

Étapes :
1. Mesurer les performances de la version naïve (Google Benchmark)
2. Concevoir une version optimisée avec des index appropriés
3. Mesurer le speedup obtenu pour chaque opération
4. Documenter le trade-off mémoire/performance

| Opération | Naïf (ms) | Optimisé (ms) | Speedup |
|---|---|---|---|
| rechercherParCategorie | | | |
| trouverParRef | | | |
| rechercherParTag | | | |
| rechercherParPrix | | | |

---

## 🏆 Mini-projet du Jour 4 : Serveur de traitement asynchrone {#mini-projet}

**Durée estimée** : 3h

**Description** : Système de traitement de jobs asynchrone (style Celery en miniature).

```cpp
// Types de jobs
enum class TypeJob { CALCUL, TEXTE, FICHIER };

struct Job {
    int id;
    TypeJob type;
    std::string parametres;  // JSON
    int priorite;            // 1-5
    std::string statut;      // "pending", "running", "done", "error"
    std::string resultat;
    std::chrono::system_clock::time_point cree;
    std::chrono::system_clock::time_point debut;
    std::chrono::system_clock::time_point fin;
};

// Architecture
class JobQueue {                   // File de jobs thread-safe
    /* priority_queue + mutex */
public:
    void soumettre(Job j);
    std::optional<Job> suivant();
    size_t taille() const;
};

class Worker {                     // Thread de traitement
    std::string nom;
    std::atomic<bool> actif{false};
public:
    void demarrer(JobQueue& q, JobStore& store);
    void arreter();
    bool estActif() const;
};

class JobStore {                   // Stockage des résultats
    std::unordered_map<int, Job> jobs;
    mutable std::shared_mutex mtx;
public:
    void sauvegarder(const Job& j);
    std::optional<Job> trouver(int id) const;
    std::vector<Job> parStatut(const std::string& s) const;
};

class Serveur {                    // Orchestrateur
    JobQueue queue;
    JobStore store;
    std::vector<Worker> workers;
    std::atomic<int> nextId{1};
public:
    explicit Serveur(int nbWorkers);
    int soumettre(TypeJob type, const std::string& params, int prio = 3);
    std::optional<Job> getStatut(int id) const;
    void afficherStats() const;
    void arreter();
};
```

**Traitements à implémenter** :
- `CALCUL` : évaluer une expression mathématique (paramètre: `"3+4*2"`)
- `TEXTE` : compter mots/lignes/chars (paramètre: chemin de fichier)
- `FICHIER` : calculer le SHA-256 d'un fichier (paramètre: chemin)

**Contraintes** :
- Zéro data race (TSan propre)
- Shutdown propre : finir les jobs en cours avant de fermer
- 10 tests unitaires minimum (Catch2 ou GoogleTest)

---

## 💡 Indices et solutions partielles {#indices}

<details>
<summary><strong>1.1.a — Réponses captures</strong></summary>

```
Lambda 1 : capture par valeur → x = 10 (valeur au moment de la capture)
Lambda 2 : capture par référence → x = 30 (valeur actuelle de x)
Lambda 3 : mutable → 11, 12, 13 (la copie interne de x est incrémentée)
           x extérieur reste à 30
Lambda 4 : capture tout par valeur → 3 (1+2, valeurs au moment de la capture)
Lambda 5 : init capture = move → peut appeler une fois; ptr est nullptr après
```
</details>

<details>
<summary><strong>1.5.b — Corriger le data race</strong></summary>

```cpp
// Version mutex
std::mutex mtx;
int compteur = 0;
void incrementer(int n) {
    for (int i = 0; i < n; i++) {
        std::lock_guard<std::mutex> lock(mtx);
        compteur++;
    }
}

// Version atomique (plus rapide pour ce cas)
std::atomic<int> compteur{0};
void incrementer(int n) {
    for (int i = 0; i < n; i++) compteur++;
}
```
</details>

<details>
<summary><strong>2.4.a — File de messages (squelette)</strong></summary>

```cpp
template<typename T>
void FileMessages<T>::envoyer(T msg) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (fermee) throw std::runtime_error("File fermée");
        file.push(std::move(msg));
    }
    cv.notify_one();
}

template<typename T>
std::optional<T> FileMessages<T>::recevoir() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]{ return !file.empty() || fermee; });
    if (file.empty()) return std::nullopt;
    T msg = std::move(file.front());
    file.pop();
    return msg;
}
```
</details>

<details>
<summary><strong>3.1 — ThreadPool soumettre avec future</strong></summary>

```cpp
template<typename Fn, typename... Args>
auto ThreadPool::soumettre(int prio, Fn&& fn, Args&&... args)
    -> std::future<std::invoke_result_t<Fn, Args...>> {

    using ReturnType = std::invoke_result_t<Fn, Args...>;
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...)
    );
    auto future = task->get_future();
    {
        std::lock_guard<std::mutex> lock(mtx);
        file.push({[task](){ (*task)(); }, prio});
    }
    cv.notify_one();
    return future;
}
```
</details>

---

## ✅ Auto-évaluation {#auto-évaluation}

### C++ Moderne
- [ ] Je comprends les 4 types de captures de lambda (valeur, référence, tout valeur, tout référence)
- [ ] Je sais utiliser `structured bindings` avec paires, tuples et maps
- [ ] Je sais utiliser `std::optional` pour éviter les valeurs sentinelles
- [ ] Je sais utiliser `std::variant` avec `std::visit` pour le visitor pattern
- [ ] Je connais au moins 3 vues Ranges (`filter`, `transform`, `take`)

### Sécurité
- [ ] Je sais identifier un buffer overflow et sa correction
- [ ] Je sais identifier une SQL injection et comment la prévenir
- [ ] Je sais identifier un use-after-free et sa correction
- [ ] Je sais activer ASan/UBSan dans mon projet

### Multithreading
- [ ] Je sais créer et joindre des threads avec `std::thread`
- [ ] Je sais protéger une section critique avec `lock_guard<mutex>`
- [ ] Je sais utiliser `std::atomic` pour les compteurs simples
- [ ] Je sais utiliser `std::async` pour lancer une tâche asynchrone
- [ ] Je sais récupérer le résultat d'un thread avec `std::future`

### Score indicatif
- **13-15 cases** : Excellent — prêt pour le Jour 5
- **9-12 cases** : Bien — consolider la concurrence
- **< 9 cases** : Reprendre les exercices threading avant le Jour 5
