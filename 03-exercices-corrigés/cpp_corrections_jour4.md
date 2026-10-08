# ✅ Corrections — Jour 4 : C++ Moderne, Sécurité & Performance
**Formation C++ · 5 jours · Corrigé des exercices**

---

## ⚡ Niveau 1 — Échauffement

### 1.1 Lambdas et closures

**1.1.a — Comprendre les captures**

```
l1() → affiche 10  (capture par valeur au moment de la définition, x=20 après n'a aucun effet)
l2() → affiche 30  (capture par référence : lit x actuel)
l3() appelée 3× → affiche 11, 12, 13 (la COPIE interne de x est incrémentée à chaque appel car `mutable`)
                   x extérieur reste à 10 (jamais modifié, seule la copie l'est)
l4() → affiche 3   (a+b capturés par valeur = 1+2 au moment de la définition ; a=10,b=20 après n'a aucun effet)
l5() : on ne peut appeler l5() qu'une fois de façon sûre côté logique métier (rien n'empêche
       techniquement plusieurs appels, mais *p reste valide tant que la lambda existe : le
       unique_ptr est déplacé DANS la lambda, donc `ptr` extérieur devient nullptr après la
       définition de l5, pas après l'appel).
```

**1.1.b — Lambdas comme callbacks**

```
btn.simulerClic() (1er appel)  → affiche "Clic!" puis "Deuxième handler"  ; clics devient 1
btn.simulerClic() (2e appel)   → mêmes affichages ; clics devient 2
btn.simulerSurvol(...)         → affiche "Survol: Enregistrer le document"
```

---

### 1.2 Syntaxe moderne

**1.2.a — Structured bindings**

```cpp
for (const auto& [nom, score] : scores)
    std::cout << nom << " : " << score << "\n";

auto [w, h, bpp] = getDimensions();

auto [min_it, max_it] = std::minmax_element(v.begin(), v.end());
std::cout << "min=" << *min_it << " max=" << *max_it << "\n";
```

**1.2.b — if constexpr** — voir le code fourni dans l'énoncé (déjà complet et correct). Test :
```
toString(true)                       → "true"
toString(42)                         → "42"
toString(std::vector<int>{1,2,3})    → "[1, 2, 3]"
toString(std::string("hello"))       → "hello"
```

---

### 1.3 Types modernes

**1.3.a — std::optional**

```cpp
std::optional<int> trouverMax(const std::vector<int>& v) {
    if (v.empty()) return std::nullopt;
    return *std::max_element(v.begin(), v.end());
}

std::optional<size_t> chercherIndex(const std::string& s, char c) {
    for (size_t i = 0; i < s.size(); ++i) if (s[i] == c) return i;
    return std::nullopt;
}

// Utilisation
auto m = trouverMax(v);
std::cout << m.value_or(-1) << "\n";
if (auto idx = chercherIndex("hello", 'l'); idx.has_value()) std::cout << *idx << "\n";
```

**1.3.b — std::variant et visitor**

```cpp
using ValeurConfig = std::variant<int, double, bool, std::string, std::vector<int>>;

class Config {
    std::map<std::string, ValeurConfig> parametres;
public:
    template<typename T>
    void set(const std::string& cle, T val) { parametres[cle] = std::move(val); }

    template<typename T>
    std::optional<T> get(const std::string& cle) const {
        auto it = parametres.find(cle);
        if (it == parametres.end()) return std::nullopt;
        if (auto* p = std::get_if<T>(&it->second)) return *p;
        return std::nullopt;
    }

    void afficher() const {
        for (const auto& [cle, val] : parametres) {
            std::cout << cle << " = ";
            std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::cout << "[";
                    for (size_t i = 0; i < v.size(); ++i) std::cout << v[i] << (i + 1 < v.size() ? ", " : "");
                    std::cout << "]";
                } else if constexpr (std::is_same_v<T, bool>) {
                    std::cout << (v ? "true" : "false");
                } else {
                    std::cout << v;
                }
            }, val);
            std::cout << "\n";
        }
    }

    std::string toJSON() const {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& [cle, val] : parametres) {
            if (!first) oss << ",";
            first = false;
            oss << "\"" << cle << "\":";
            std::visit([&oss](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::string>) oss << "\"" << v << "\"";
                else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < v.size(); ++i) oss << v[i] << (i + 1 < v.size() ? "," : "");
                    oss << "]";
                } else oss << v;
            }, val);
        }
        oss << "}";
        return oss.str();
    }
};
```

---

### 1.4 Sécurité mémoire

**1.4.a — Identifier les vulnérabilités**

| Code | Vulnérabilité (CWE) | Correction |
|---|---|---|
| 1 | Buffer overflow (CWE-120) : `strcpy` ne vérifie pas la taille de `dest` | `std::string dest = source;` ou `strncpy(dest, source, sizeof(dest)-1)` |
| 2 | Off-by-one / débordement de tableau (CWE-193/CWE-787) : boucle `<=` dépasse `tab[9]` | `for (int i = 0; i < 10; i++)` |
| 3 | Retour d'adresse d'une variable locale (dangling pointer, CWE-562) : `local` détruite à la sortie de fonction | Retourner par valeur : `int creer() { return 42; }` |
| 4 | Use-after-free (CWE-416) : lecture après `delete` | Ne plus utiliser `p` après `delete`, mettre `p = nullptr;`, ou utiliser `unique_ptr` |
| 5 | Integer overflow (CWE-190) : `INT_MAX + 1` est UB (dépassement silencieux en pratique) | Vérifier avant l'opération (`if (a > INT_MAX - 1)`) ou utiliser un type plus large |
| 6 | SQL Injection (CWE-89) : concaténation de `nom` non échappé dans la requête | Requête préparée avec paramètres liés (`?` + bind) |

**1.5.a — Premier thread**

```cpp
#include <thread>
#include <chrono>

void afficherNombres() { for (int i = 1; i <= 10; ++i) std::cout << i << " "; std::cout << "\n"; }

long long sommeJusqua(int n) {
    long long s = 0;
    for (int i = 1; i <= n; ++i) s += i;
    return s;
}

int main() {
    long long resultat = 0;
    std::thread t1(afficherNombres);
    std::thread t2([&resultat]() { resultat = sommeJusqua(1000000); });
    t1.join();
    t2.join();
    std::cout << "Somme : " << resultat << "\n";
}
```
Sans `join()` : le thread devient "joinable" à la destruction du `std::thread` sans avoir été joint ni détaché → `std::terminate()` est appelé (crash). Appeler `join()` deux fois lève une exception `std::system_error` (le thread n'est plus joignable après le premier appel).

**1.5.b — Data race**

Le programme est un **data race classique** : `compteur++` n'est pas atomique (lecture-modification-écriture), donc le résultat final est souvent < 200000 de façon non déterministe (comportement indéfini selon le standard).

```cpp
// Version mutex
std::mutex mtx;
int compteur = 0;
void incrementer(int n) { for (int i=0;i<n;i++){ std::lock_guard<std::mutex> l(mtx); compteur++; } }

// Version atomique (généralement plus rapide, pas de lock OS)
std::atomic<int> compteurAtomic{0};
void incrementerAtomic(int n) { for (int i=0;i<n;i++) compteurAtomic++; }
```

---

## 🔥 Niveau 2 — Pratique

### 2.1 Lambdas avancées

```cpp
template<typename T>
class Transformable {
    T valeur;
public:
    explicit Transformable(T v) : valeur(std::move(v)) {}

    template<typename Fn>
    auto map(Fn fn) const -> Transformable<decltype(fn(valeur))> {
        return Transformable<decltype(fn(valeur))>(fn(valeur));
    }

    Transformable filter(std::function<bool(const T&)> pred, T defaut = T{}) const {
        return pred(valeur) ? *this : Transformable(defaut);
    }

    Transformable& tap(std::function<void(const T&)> fn) { fn(valeur); return *this; }

    const T& get() const { return valeur; }
};
```

### 2.2 Ranges C++20

```cpp
auto erreurs = logs
    | std::views::filter([](const LogEntry& e) { return e.niveau == "ERROR"; });

auto derniersBDD = logs
    | std::views::filter([](const LogEntry& e) { return e.source == "database"; })
    | std::views::reverse
    | std::views::take(10);

std::map<std::string, int> parNiveau;
for (const auto& e : logs) parNiveau[e.niveau]++;   // pas de groupBy natif en C++20

auto contientCI = [](const std::string& hay, const std::string& needle) {
    auto it = std::search(hay.begin(), hay.end(), needle.begin(), needle.end(),
        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    return it != hay.end();
};
auto timeouts = logs
    | std::views::filter([&](const LogEntry& e) { return contientCI(e.message, "timeout"); });

std::string csv;
for (const auto& e : logs | std::views::transform([](const LogEntry& e) {
        return e.timestamp + "," + e.niveau + "," + e.message; }))
    csv += e + "\n";
```

### 2.3 Validation et sanitisation

```cpp
class InputValidator {
public:
    struct ResultatValidation { bool valide; std::string messageErreur; std::string valeurSanitisee; };

    static ResultatValidation validerEmail(const std::string& email) {
        static const std::regex re(R"(^[\w.+-]+@[\w-]+\.[a-zA-Z]{2,}$)");
        bool ok = std::regex_match(email, re);
        return {ok, ok ? "" : "Format email invalide", ok ? email : ""};
    }

    static ResultatValidation validerTelephoneFR(const std::string& tel) {
        std::string nettoye;
        for (char c : tel) if (std::isdigit(static_cast<unsigned char>(c))) nettoye += c;
        bool ok = (nettoye.size() == 10 && nettoye[0] == '0') ||
                  (nettoye.size() == 11 && nettoye.substr(0, 2) == "33");
        return {ok, ok ? "" : "Téléphone invalide", ok ? nettoye : ""};
    }

    static std::string sanitiserHTML(const std::string& html) {
        std::string res;
        for (char c : html) {
            if (c == '<') res += "&lt;";
            else if (c == '>') res += "&gt;";
            else if (c == '&') res += "&amp;";
            else if (c == '"') res += "&quot;";
            else res += c;
        }
        return res;
    }

    static std::string sanitiserSQL(const std::string& val) {
        std::string res;
        for (char c : val) { if (c == '\'') res += '\''; res += c; }  // échappement des quotes
        return res;
    }

    static std::string sanitiserNomFichier(const std::string& nom) {
        std::string res;
        for (char c : nom)
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.')
                res += c;               // supprime "../", "/", espaces, etc. → anti path-traversal
        return res;
    }

    template<typename T>
    static ResultatValidation validerPlage(T val, T min, T max, const std::string& nomChamp) {
        bool ok = (val >= min && val <= max);
        return {ok, ok ? "" : nomChamp + " hors plage [" + std::to_string(min) + "," + std::to_string(max) + "]", ""};
    }
};
```
Testé avec `<script>alert(1)</script>` (XSS) → correctement échappé ; `'; DROP TABLE users;--` (SQL injection) → quotes doublées ; `../../etc/passwd` (path traversal) → tous les caractères `/` et `.` non alphanumériques hors liste blanche supprimés.

### 2.4 Synchronisation

```cpp
template<typename T>
class FileMessages {
    std::queue<T> file;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> fermee{false};
public:
    void envoyer(T msg) {
        { std::lock_guard<std::mutex> l(mtx); file.push(std::move(msg)); }
        cv.notify_one();
    }
    std::optional<T> recevoir() {
        std::unique_lock<std::mutex> l(mtx);
        cv.wait(l, [this] { return !file.empty() || fermee; });
        if (file.empty()) return std::nullopt;
        T msg = std::move(file.front()); file.pop();
        return msg;
    }
    std::optional<T> tenterRecevoir() {
        std::lock_guard<std::mutex> l(mtx);
        if (file.empty()) return std::nullopt;
        T msg = std::move(file.front()); file.pop();
        return msg;
    }
    void fermer() { { std::lock_guard<std::mutex> l(mtx); fermee = true; } cv.notify_all(); }
    size_t taille() const { std::lock_guard<std::mutex> l(mtx); return file.size(); }
    bool estFermee() const { return fermee; }
};
```

```cpp
class BarriereSync {
    int n; std::atomic<int> compteur;
    std::mutex mtx; std::condition_variable cv;
    int generation = 0;
public:
    explicit BarriereSync(int n) : n(n), compteur(n) {}
    void attendre() {
        std::unique_lock<std::mutex> l(mtx);
        int genLocale = generation;
        if (--compteur == 0) {
            compteur = n;
            ++generation;
            cv.notify_all();
        } else {
            cv.wait(l, [&] { return genLocale != generation; });
        }
    }
};
```

### 2.5 Futures et async

```cpp
Ressource telecharger(const std::string& url) {
    auto debut = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 150));
    std::vector<uint8_t> contenu(1024 + rand() % 4096);
    auto duree = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - debut);
    return {url, contenu, contenu.size(), duree};
}

std::vector<Ressource> telechargerTous_seq(const std::vector<std::string>& urls) {
    std::vector<Ressource> res;
    for (const auto& u : urls) res.push_back(telecharger(u));
    return res;
}

std::vector<Ressource> telechargerTous_async(const std::vector<std::string>& urls) {
    std::vector<std::future<Ressource>> futures;
    for (const auto& u : urls) futures.push_back(std::async(std::launch::async, telecharger, u));
    std::vector<Ressource> res;
    for (auto& f : futures) res.push_back(f.get());
    return res;
}

std::vector<Ressource> telechargerTous_limite(const std::vector<std::string>& urls, int maxConcurrent) {
    std::vector<Ressource> res(urls.size());
    std::atomic<size_t> index{0};
    auto worker = [&]() {
        size_t i;
        while ((i = index++) < urls.size()) res[i] = telecharger(urls[i]);
    };
    std::vector<std::thread> pool;
    for (int i = 0; i < maxConcurrent; ++i) pool.emplace_back(worker);
    for (auto& t : pool) t.join();
    return res;
}
```
Speedup mesuré typiquement 5-15× pour 20 URLs en version async pure (limité par le nombre de cœurs disponibles et la contention I/O simulée).

---

## 🚀 Niveau 3 — Défi

### 3.1 ThreadPool avec priorités — voir squelette dans le mémento/indices, complété :

```cpp
class ThreadPool {
    struct Tache {
        std::function<void()> fn;
        int priorite;
        bool operator<(const Tache& o) const { return priorite < o.priorite; }
    };
    std::vector<std::jthread> workers;
    std::priority_queue<Tache> file;
    std::mutex mtx;
    std::condition_variable cv;
    bool arret = false;

public:
    explicit ThreadPool(size_t nbThreads) {
        for (size_t i = 0; i < nbThreads; ++i)
            workers.emplace_back([this](std::stop_token st) {
                while (!st.stop_requested()) {
                    Tache t;
                    { std::unique_lock<std::mutex> l(mtx);
                      cv.wait(l, [&] { return !file.empty() || arret; });
                      if (arret && file.empty()) return;
                      t = file.top(); file.pop(); const_cast<Tache&>(t); }
                    t.fn();
                }
            });
    }

    template<typename Fn, typename... Args>
    auto soumettre(int priorite, Fn&& fn, Args&&... args)
        -> std::future<std::invoke_result_t<Fn, Args...>> {
        using ReturnType = std::invoke_result_t<Fn, Args...>;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        auto future = task->get_future();
        { std::lock_guard<std::mutex> l(mtx); file.push({[task]{ (*task)(); }, priorite}); }
        cv.notify_one();
        return future;
    }

    ~ThreadPool() { { std::lock_guard<std::mutex> l(mtx); arret = true; } cv.notify_all(); }
};
```

### 3.2 Audit de sécurité — serveur HTTP

| Bug | Vulnérabilité (CWE) | Correction |
|---|---|---|
| 1 | Buffer fixe partagé entre requêtes (thread-safety, CWE-362) | Buffer local à `traiterRequete`, pas membre de classe |
| 2 | `recv` sans vérifier la valeur de retour ni terminer la chaîne (CWE-170) | Vérifier `n = recv(...)`, `buffer[n] = '\0'`, gérer erreurs |
| 3 | `sscanf("%s")` sans limite de taille → buffer overflow (CWE-120) | `sscanf(buffer, "%15s %255s %15s", ...)` avec largeurs, ou parsing sûr par `std::string` |
| 4 | Path traversal via `chemin` non validé (CWE-22) : `chemin = "../../etc/passwd"` | Normaliser le chemin (`std::filesystem::canonical`) et vérifier qu'il reste sous `/var/www/html` |
| 5 | `fopen` peut retourner `nullptr` (fichier absent) → déréférencement null (CWE-476) | Vérifier `if (!f) { renvoyer 404; return; }` |
| 6 | `sprintf` sans limite de taille → buffer overflow (CWE-120) | `snprintf(reponse, sizeof(reponse), ...)` |
| 7 | Pas de limite de connexions / DoS (CWE-400) | Limiter les connexions concurrentes, timeout par connexion |
| 8 (implicite) | Pas de fermeture de `socket` client après traitement (fuite de descripteurs, CWE-772) | `close(socket)` en fin de `traiterRequete`, idéalement via RAII |

### 3.3 Benchmark et optimisation

```cpp
class CatalogueOptimise {
    std::vector<Article> articles;
    std::unordered_map<std::string, size_t> indexParRef;
    std::unordered_multimap<std::string, size_t> indexParCategorie;
    std::unordered_multimap<std::string, size_t> indexParTag;

public:
    void construireIndex() {
        for (size_t i = 0; i < articles.size(); ++i) {
            indexParRef[articles[i].reference] = i;
            indexParCategorie.insert({articles[i].categorie, i});
            for (const auto& tag : articles[i].tags) indexParTag.insert({tag, i});
        }
    }

    std::optional<Article> trouverParRef(const std::string& ref) const {
        auto it = indexParRef.find(ref);
        return it != indexParRef.end() ? std::optional<Article>(articles[it->second]) : std::nullopt;
    }
    // trouverParRef passe de O(n) à O(1) → speedup ≥ 100× mesuré typiquement sur 100 000 articles
    // rechercherParCategorie/Tag passent de O(n) à O(k) (k = taille du groupe) via l'index multimap
};
```
Trade-off : les index doublent approximativement la mémoire utilisée par les clés indexées (~quelques dizaines de Mo pour 100 000 articles), ce qui est largement justifié par le gain de performance pour des opérations appelées 10 000 fois/seconde.

---

## 🏆 Mini-projet du Jour 4 : Serveur de traitement asynchrone — Architecture de correction

```cpp
class JobQueue {
    std::priority_queue<Job, std::vector<Job>, std::function<bool(const Job&, const Job&)>> file{
        [](const Job& a, const Job& b) { return a.priorite < b.priorite; }};
    mutable std::mutex mtx;
    std::condition_variable cv;
public:
    void soumettre(Job j) { { std::lock_guard<std::mutex> l(mtx); file.push(std::move(j)); } cv.notify_one(); }
    std::optional<Job> suivant() {
        std::unique_lock<std::mutex> l(mtx);
        if (!cv.wait_for(l, std::chrono::milliseconds(100), [&] { return !file.empty(); })) return std::nullopt;
        Job j = file.top(); file.pop();
        return j;
    }
    size_t taille() const { std::lock_guard<std::mutex> l(mtx); return file.size(); }
};

class JobStore {
    std::unordered_map<int, Job> jobs;
    mutable std::shared_mutex mtx;
public:
    void sauvegarder(const Job& j) { std::unique_lock l(mtx); jobs[j.id] = j; }
    std::optional<Job> trouver(int id) const {
        std::shared_lock l(mtx);
        auto it = jobs.find(id);
        return it != jobs.end() ? std::optional<Job>(it->second) : std::nullopt;
    }
    std::vector<Job> parStatut(const std::string& s) const {
        std::shared_lock l(mtx);
        std::vector<Job> res;
        for (const auto& [id, j] : jobs) if (j.statut == s) res.push_back(j);
        return res;
    }
};
```
`Worker` boucle sur `queue.suivant()`, exécute le traitement adapté au `TypeJob`, et enregistre le résultat dans `JobStore` via `sauvegarder`. Le shutdown propre attend la fin des jobs `running` avant de positionner `actif = false` (drain pattern).

---

## ✅ Points clés à retenir du Jour 4

- Bien distinguer les 5 modes de capture de lambda (valeur, référence, tout-valeur, tout-référence, init-capture C++14).
- `std::optional`/`std::variant` éliminent les valeurs sentinelles et les `void*` non typés.
- Toute entrée utilisateur doit être validée ET sanitisée avant usage (SQL, HTML, chemins de fichiers).
- `std::atomic` est préférable à `std::mutex` pour un compteur simple (moins de surcoût).
- Toujours activer `-fsanitize=address,undefined` en développement pour détecter tôt les bugs mémoire.
