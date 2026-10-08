# Aide formateur — Formation C++ 5 jours
## Reprise à mi-journée du Jour 2 → Jour 5

> **Hypothèse de reprise** : le support du Jour 2 enchaîne la gestion mémoire, les pointeurs, RAII, smart pointers et move semantics, puis démarre la STL. Je considère donc que tu récupères le groupe au début de la section **« 06 · Introduction à la STL »**, juste après le TP mémoire. Si le formateur précédent s'est arrêté quelques slides avant ou après, le sas de reprise ci-dessous permet de recaler le groupe sans difficulté.

---

# 1. Ligne directrice d'animation

Le support est très dense. Le rôle du formateur n'est pas de lire toutes les slides, mais de **donner le modèle mental qui manque entre deux blocs de code**.

Pour chaque notion, utilise le cycle suivant :

1. **Pourquoi cette notion existe ?** Quel problème concret elle résout.
2. **Que faut-il regarder dans la syntaxe ?** Deux ou trois éléments maximum.
3. **Quel est le piège principal ?** L'erreur typique d'un développeur qui débute.
4. **Quel exemple réel ?** Code métier, performance, maintenance, sécurité.
5. **Une question au groupe.** Avant de donner la réponse.
6. **Une micro-démo.** Modifier une ligne et demander le résultat avant de compiler.

La règle à garder pendant toute la formation : **la slide montre, toi tu expliques le “pourquoi”, les conséquences et les limites.**

---

# 2. Reprise du groupe — 20 à 25 minutes maximum

## 2.1 Phrase d'ouverture

Tu peux démarrer ainsi :

> « Avant de continuer, je ne vais pas refaire les deux premières demi-journées. Je veux simplement vérifier que nous avons tous les mêmes repères, parce que ce qu'on va voir maintenant — STL, patterns, templates et concurrence — repose directement dessus. Je vais vous poser quelques questions rapides. Si quelque chose bloque, on le remet en place immédiatement. »

## 2.2 Rappel Jour 1 — 8 minutes

### Programme minimal

```cpp
#include <iostream>

int main() {
    int x = 5;
    std::cout << x << '\n';
    return 0;
}
```

### Ce que tu dois rappeler oralement

- `#include <iostream>` demande au préprocesseur de rendre disponibles les déclarations liées aux flux standard.
- `main` est le point d'entrée du programme exécutable.
- `int x = 5;` combine **type, nom et initialisation**.
- `std::cout` est un objet de flux ; `<<` enchaîne des insertions dans ce flux.
- `return 0` signifie conventionnellement que le programme se termine correctement. En C++ moderne, arriver à la fin de `main` équivaut à retourner `0`.

### Question groupe

> « Si je remplace `int x = 5;` par `auto x = 5;`, quel type est déduit ? Et si j'écris `auto x = 5.0;` ? »

Objectif : vérifier qu'ils distinguent valeur et type.

---

## 2.3 Rappel Jour 1 — références, classes et polymorphisme — 5 minutes

Écrire :

```cpp
void incrementer(int& n) {
    ++n;
}
```

Dire :

> « Une référence est un alias. Ici, je ne travaille pas sur une copie de `n` : je travaille sur l'objet de l'appelant. C'est une idée importante parce que la STL manipule énormément de références et d'itérateurs. »

Puis rappeler :

```cpp
class Animal {
public:
    virtual void parler() const = 0;
    virtual ~Animal() = default;
};
```

Points oraux :

- méthode virtuelle = choix du comportement à l'exécution ;
- `= 0` = méthode virtuelle pure, donc classe abstraite ;
- destructeur virtuel indispensable si destruction via pointeur de base ;
- ne pas développer la vtable : citer simplement le mécanisme.

---

## 2.4 Rappel début Jour 2 — mémoire et ownership — 8 minutes

Dessiner au tableau deux zones : **stack** / **heap**.

### Image mentale

- stack : casier temporaire associé à la portée d'une fonction ; destruction automatique à la sortie de portée ;
- heap : espace dans lequel la durée de vie n'est pas liée à une portée précise ; il faut donc un propriétaire clair.

Montrer :

```cpp
int x = 42;                           // durée de vie automatique
int* p = new int(42);                // allocation manuelle
std::unique_ptr<int> q =
    std::make_unique<int>(42);       // propriété explicite
```

Dire :

> « Le problème du C++ moderne n'est pas “comment appeler `delete` correctement ?”. La bonne question est : “qui possède cette ressource et qui est responsable de sa destruction ?”. RAII et les smart pointers servent précisément à rendre cette réponse visible dans le type. »

### Questions flash

1. `unique_ptr` est-il copiable ? **Non.**
2. Peut-il être déplacé ? **Oui.**
3. `shared_ptr` doit-il être le choix par défaut ? **Non.** Il exprime une propriété réellement partagée.
4. À quoi sert `weak_ptr` ? **Observer une ressource gérée par `shared_ptr` sans prolonger sa durée de vie**, notamment pour casser les cycles.
5. Que fait `std::move` ? **Il ne déplace rien à lui seul. Il convertit l'expression pour autoriser l'utilisation d'une opération de déplacement.**

### Transition

> « Maintenant que la question de la durée de vie des objets est claire, on peut manipuler des collections d'objets sans réinventer la gestion mémoire. C'est exactement le rôle de la STL. »

---

# 3. Jour 2 après-midi — STL

## Slide « La Standard Template Library »

### Ce qu'il faut ajouter

La STL n'est pas seulement « une bibliothèque de conteneurs ». Présente-la comme un **vocabulaire commun composé de trois briques** :

- les **conteneurs** stockent ;
- les **itérateurs** décrivent une position ou une plage ;
- les **algorithmes** travaillent sur des plages sans dépendre du conteneur concret.

Écris au tableau :

> **conteneur + itérateurs + algorithmes = code générique**

Exemple réel : une fonction qui cherche un élément n'a pas besoin de connaître `vector`. Elle reçoit un début et une fin.

---

## Slide `std::vector`

### Précisions indispensables

Le support montre `reserve` et `resize`. Insiste fortement sur la différence :

- `reserve(1000)` : change la **capacité**, pas la taille ; aucun nouvel élément n'existe ;
- `resize(1000)` : la taille devient 1000 ; les éléments existent.

Dessine 3 cases utilisées sur 8 cases allouées :

- `size = 3`
- `capacity = 8`

### Démo à faire

```cpp
std::vector<int> v;
v.reserve(10);
std::cout << v.size() << " / " << v.capacity() << '\n';
```

Demander le résultat avant de lancer : `0 / au moins 10`.

### Point que la slide aborde trop peu : invalidation

Quand `vector` se réalloue, ses éléments peuvent être déplacés ailleurs. Les pointeurs, références et itérateurs qui désignaient l'ancien stockage peuvent devenir invalides.

Exemple :

```cpp
std::vector<int> v = {1, 2, 3};
int& ref = v[0];
v.push_back(4); // peut réallouer
// ref peut être devenue invalide
```

Ne dis pas que `push_back` invalide toujours : **il invalide tous les itérateurs/références si réallocation ; sinon seuls `end()` et certains éléments selon l'opération.**

### Question réelle

> « Pour une liste de 100 000 objets que je parcours beaucoup et modifie peu, quel conteneur choisiriez-vous spontanément ? »

Réponse attendue : souvent `vector`, grâce à la localité mémoire, même si une `list` semble séduisante sur le papier.

---

## Slide `list` et `deque`

### Message à faire passer

Ne présente pas `list` comme « meilleure pour les insertions ». La complexité O(1) ne vaut que **si l'on possède déjà l'itérateur de la position**. Chercher la position reste O(n).

Ajoute :

> « En pratique, `vector` gagne souvent même lorsqu'il doit déplacer quelques éléments, parce que ses données sont contiguës et très favorables au cache CPU. »

`deque` est utile lorsqu'on a besoin de pousser/retirer aux deux extrémités tout en gardant un accès indexé.

---

## Slide `map`

### Piège essentiel : `operator[]`

```cpp
std::map<std::string, int> ages;
std::cout << ages["Alice"];
```

Cette lecture **insère** `Alice` avec une valeur par défaut `0` si la clé n'existe pas.

Pour une lecture qui ne doit pas modifier :

```cpp
ages.at("Alice");
// ou find / contains selon le besoin
```

### Exemple réel

Compteur de fréquences : `freq[mot]++` est justement un bon cas où l'insertion automatique est pratique.

---

## Slide `unordered_map`

### Nuance à apporter

Ne dis jamais simplement « accès O(1) ». Dis : **O(1) en moyenne, O(n) dans le pire cas**, dépendant de la qualité de la fonction de hachage et des collisions.

Expliquer `reserve` ici comme moyen de réduire les rehashes.

Analogie : casiers numérotés par une fonction de hachage ; deux clés peuvent tomber dans le même casier.

---

## Slide `set` / `unordered_set`

À faire comprendre : un `set` encode une **contrainte métier d'unicité**.

Exemples réels :

- identifiants déjà vus ;
- utilisateurs connectés ;
- permissions uniques ;
- suppression de doublons.

Mentionner que convertir un `vector` en `unordered_set` supprime les doublons mais **perd l'ordre**.

---

## Slide adaptateurs `stack`, `queue`, `priority_queue`

Image mentale : ce ne sont pas de nouveaux stockages, ce sont des **interfaces restreintes au-dessus d'un conteneur**.

Exemples réels :

- `stack` : historique undo, parcours en profondeur ;
- `queue` : file de travaux ;
- `priority_queue` : ordonnanceur de tâches, tickets urgents, Dijkstra.

---

## Slide itérateurs

### Explication à ne pas sauter

`end()` ne désigne **pas** le dernier élément. Il désigne une position fictive **juste après** le dernier.

Dessiner :

`[10][20][30][40] | end`

`begin()` → première case ; `end()` → après la dernière.

C'est ce modèle `[first, last)` qui rend les algorithmes composables.

### Piège

Ne jamais déréférencer `end()`.

---

## Slides `sort`, `find`, `count`, `transform`, `accumulate`

### Modèle mental

Insiste sur le fait que les algorithmes STL expriment une **intention** :

```cpp
std::count_if(...)
```

se lit « compter les éléments satisfaisant cette condition ».

Cela donne souvent un code plus facile à vérifier qu'une boucle manuelle avec compteur, index et conditions imbriquées.

### `sort`

La fonction de comparaison doit définir un ordre strict. Une erreur classique est :

```cpp
[](int a, int b) { return a <= b; } // mauvais
```

La comparaison doit plutôt utiliser `<` ou `>` selon l'ordre.

### `accumulate`

La valeur initiale détermine aussi le type de l'accumulateur.

```cpp
std::vector<double> v = {1.5, 2.5};
auto a = std::accumulate(v.begin(), v.end(), 0);   // int !
auto b = std::accumulate(v.begin(), v.end(), 0.0); // double
```

Très bonne mini-démo.

---

## Slide `remove` — point absolument à expliciter

`std::remove` **ne réduit pas la taille du conteneur**. Il réorganise les éléments et retourne un nouvel « end logique ».

Dessiner avant/après :

`1 2 3 2 4 2 5`

Après `remove(...,2)` : début logique `1 3 4 5 | ? ? ?`

Puis :

```cpp
v.erase(newEnd, v.end());
```

En C++20, montrer aussi l'alternative plus lisible :

```cpp
std::erase(v, 2);
```

---

## Slide STL + lambdas

### À expliquer

Une lambda est un petit objet fonction construit localement. Les trois zones à lire sont :

```cpp
[capture](parametres) -> type_retour { corps }
```

Pour l'instant, ne développe pas toute la mécanique de closure ; elle reviendra Jour 4.

Phrase utile :

> « Ici la lambda donne le morceau de comportement spécifique, tandis que `sort`, `find_if` ou `count_if` fournissent le mécanisme générique. »

---

# 4. Jour 2 — Design Patterns

## Slide catégories de patterns

Ne les présente pas comme des recettes à appliquer systématiquement.

> « Un pattern est un nom donné à une solution récurrente à un problème de conception récurrent. Le premier réflexe ne doit pas être “quel pattern puis-je mettre ?”, mais “quel problème de couplage, création ou variation suis-je en train de résoudre ?”. »

---

## Singleton

### Ce qu'il faut nuancer

Le Singleton peut être utile pour une ressource vraiment unique, mais il introduit souvent un **état global caché**, complique les tests et crée du couplage.

En C++11+, montrer le Meyers Singleton :

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger x;
        return x;
    }
private:
    Logger() = default;
};
```

La construction de la variable locale statique est thread-safe depuis C++11.

Question :

> « Pourquoi préférer injecter un `Logger&` dans une classe plutôt que d'appeler le Singleton partout ? »

Réponse : dépendance explicite, testabilité, remplacement possible.

---

## Factory Method

### Image réelle

Un lecteur de fichiers reçoit une extension, mais le code appelant ne veut pas connaître `JsonParser`, `CsvParser`, etc.

La Factory centralise la **création** ; elle ne remplace pas nécessairement le polymorphisme, elle le prépare.

---

## Abstract Factory

Expliquer la différence avec Factory Method :

- Factory Method : créer un type de produit ;
- Abstract Factory : créer une **famille cohérente de produits**.

Exemple : thème Windows/macOS → bouton + fenêtre + menu compatibles.

---

## Builder

Le problème n'est pas « construire un objet ». Le problème est de construire un objet avec beaucoup d'options en évitant un constructeur illisible :

```cpp
Requete(url, "POST", headers, body, 5000, true, false, ...);
```

Le Builder rend chaque choix explicite et permet une API fluide.

---

## Adapter

Analogie : adaptateur de prise électrique. Il ne modifie ni l'appareil ni la prise murale ; il traduit l'une vers l'autre.

Cas réel : intégrer une ancienne API C dans une interface C++ moderne.

---

## Decorator

Précision importante : le décorateur **conserve la même interface** et ajoute du comportement en enveloppant un objet.

Exemples réels :

- compression puis chiffrement d'un flux ;
- ajout de logs autour d'un service ;
- métriques autour d'un repository.

À distinguer d'un simple héritage : on peut combiner les décorateurs dynamiquement.

---

## Observer

Exemple réel : GUI, bus d'événements, notifications, systèmes réactifs.

Point à ajouter : la difficulté réelle n'est pas seulement `subscribe`; c'est le **cycle de vie des abonnés**. Un callback capturant une référence vers un objet détruit est dangereux.

---

## Strategy

Phrase simple :

> « Strategy, c'est rendre interchangeable la partie de l'algorithme qui varie. »

Montre qu'en C++ moderne une Strategy peut être une interface virtuelle, un `std::function`, un functor ou un paramètre template.

---

## Command

Exemple réel idéal : `undo/redo`.

La commande transforme une action en objet manipulable : on peut la stocker, la mettre en file, la rejouer ou l'annuler.

---

## SOLID — Open/Closed

Ne récite pas l'acronyme. Montre l'objectif : ajouter un nouveau calcul de taxe sans modifier un gros `switch` déjà validé.

Évite toutefois de présenter l'héritage comme solution automatique. Un `std::function` ou une composition peut parfois être plus simple.

---

# 5. Jour 2 — Testing avancé et optimisation

## Mocks

À dire :

- un mock ne sert pas à « simuler tout » ;
- il isole une dépendance difficile/lente/non déterministe ;
- trop de mocks rendent les tests fragiles car ils testent l'implémentation plutôt que le comportement.

Exemple réel : service de paiement, base de données, API distante.

## Tests paramétrés

But : appliquer le même comportement attendu à une table de cas, sans copier-coller le test.

## Couverture

Phrase essentielle :

> « 100 % de couverture signifie que les lignes ont été exécutées, pas que le programme est correct. »

Utiliser la couverture pour trouver du code non exercé, pas comme preuve de qualité.

## Refactoring

Définition stricte : **modifier la structure interne sans modifier le comportement observable**. Donc idéalement : tests verts avant, modification, tests verts après.

---

# 6. Jour 3 — Templates et programmation générique

## Pourquoi les templates ?

Démarrer avec deux fonctions :

```cpp
int maxInt(int a, int b);
double maxDouble(double a, double b);
```

Puis :

```cpp
template<typename T>
T maximum(T a, T b) {
    return a > b ? a : b;
}
```

Dire :

> « Nous ne demandons pas au programme de choisir un type à l'exécution. Nous demandons au compilateur de générer une version adaptée aux types réellement utilisés. »

---

## Template de fonction

Préciser : l'instanciation se fait lorsqu'un type concret est utilisé. Une erreur située dans un template peut donc n'apparaître qu'au moment où une instanciation particulière est demandée.

Exemple : `maximum<std::string>` n'est valide que si l'opération utilisée est disponible pour `std::string`.

---

## Templates de classes

Insister sur la différence entre :

```cpp
Pile<int>
Pile<std::string>
```

Ce sont deux types concrets différents produits à partir du même patron.

### Point important absent des explications superficielles

Les définitions des templates sont généralement placées dans les headers, car le compilateur doit voir l'implémentation au moment de l'instanciation.

---

## Spécialisation

À présenter comme une exception ciblée au comportement générique, pas comme la première solution.

---

## Variadic templates

Image mentale : un paquet de types `Ts...` et un paquet de valeurs `args...`.

Fais lire :

```cpp
template<typename... Args>
void log(Args&&... args);
```

Ne plonge pas immédiatement dans perfect forwarding si le groupe hésite encore sur `&&`.

---

## SFINAE

Présente d'abord le problème historique : « activer une fonction seulement pour certains types ».

Puis dis clairement que **Concepts C++20 est généralement la forme moderne, plus lisible, à privilégier pour de nouveau code**.

---

## Concepts C++20

Exemple :

```cpp
#include <concepts>

template<std::integral T>
T doubler(T x) {
    return x * 2;
}
```

Phrase :

> « Le concept transforme une erreur de template obscure en contrat lisible sur le type accepté. »

---

## Métaprogrammation / constexpr

Ne vends pas cela comme « rendre tout plus rapide ». Explique : certaines valeurs peuvent être calculées à la compilation **si les entrées et les opérations permettent une évaluation constante**.

---

# 7. Jour 3 — Exceptions et robustesse

## try / catch / throw

Modèle mental :

1. une fonction détecte une situation qu'elle ne sait pas résoudre localement ;
2. elle lance une exception ;
3. la pile se déroule ;
4. les objets automatiques sont détruits : **RAII devient crucial** ;
5. un niveau capable de prendre une décision intercepte.

Ne mets pas des `try/catch` partout.

---

## Exceptions personnalisées

Donne une hiérarchie métier seulement lorsque l'appelant peut réellement réagir différemment.

```cpp
class ErreurCatalogue : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
```

---

## `noexcept`

À expliquer avec précision : `noexcept` est un contrat. Si une exception sort d'une fonction déclarée `noexcept`, le programme appelle `std::terminate`.

Pourquoi les moves `noexcept` sont importants : les conteneurs comme `vector` peuvent préférer le déplacement lors d'une réallocation s'ils savent qu'il ne lancera pas.

---

## Garanties d'exception

Donner trois niveaux simples :

- **no-throw** : l'opération ne lance pas ;
- **strong guarantee** : en cas d'échec, l'état reste comme avant ;
- **basic guarantee** : l'objet reste valide, mais son état peut avoir changé.

Exemple réel : virement bancaire → ne pas débiter puis échouer avant le crédit sans stratégie de rollback.

---

# 8. Jour 3 — Structure de projet, CMake et CI/CD

## Headers et `.cpp`

Rappeler :

- header = interface que les autres unités doivent connaître ;
- `.cpp` = implémentation cachée autant que possible ;
- chaque `.cpp` est compilé séparément puis lié.

Dessine : `a.cpp -> a.o`, `b.cpp -> b.o`, puis **linker -> executable**.

---

## CMake

Le point à marteler : CMake n'est pas un compilateur. Il **génère/configure le système de build**.

Privilégier l'approche moderne par targets :

```cmake
add_library(core src/core.cpp)
target_include_directories(core PUBLIC include)
target_compile_features(core PUBLIC cxx_std_20)
```

Expliquer `PRIVATE`, `PUBLIC`, `INTERFACE` comme propagation d'une dépendance.

---

## Dépendances

- `FetchContent` : pratique pour une dépendance intégrée au build ;
- `find_package` : dépendance installée ou fournie par un package manager ;
- vcpkg/Conan : gérer des versions et binaires de dépendances.

Ne passe pas trop de temps sur les commandes spécifiques si l'environnement des stagiaires ne les utilise pas.

---

## Bibliothèque statique/dynamique

Image réelle :

- statique : code de la bibliothèque intégré au binaire final ;
- dynamique : bibliothèque séparée chargée/liée au runtime.

Préciser que « statique = plus rapide » ou « dynamique = moins de mémoire » sont des simplifications trompeuses ; les motivations principales sont distribution, mise à jour, partage, ABI et déploiement.

---

## Tests unitaires vs intégration

- unitaire : petite unité isolée, rapide, diagnostic précis ;
- intégration : plusieurs composants réels ensemble, vérifie les contrats entre composants.

Un test avec une vraie base temporaire n'est plus un simple test unitaire.

---

## CI/CD

Définition simple :

> « La CI exécute automatiquement ce que nous devrions faire manuellement avant de livrer : construire, tester, analyser et éventuellement empaqueter sur un environnement propre. »

Ne présente pas GitHub Actions comme la CI elle-même : c'est un outil qui implémente le pipeline.

---

# 9. Jour 4 — C++ moderne

## Lambdas

Commencer simple :

```cpp
int seuil = 10;
auto sup = [seuil](int x) { return x > seuil; };
```

Puis expliquer les captures :

- `[seuil]` : copie ;
- `[&seuil]` : référence ;
- `[=]` : captures utilisées par copie ;
- `[&]` : captures utilisées par référence.

### Piège majeur

Une lambda qui capture une variable locale **par référence** puis survit à cette variable possède une référence pendante.

```cpp
std::function<int()> f() {
    int x = 42;
    return [&] { return x; }; // dangereux
}
```

---

## `auto`

Ne dis pas « auto = type dynamique ». C'est faux. Le type est **déduit à la compilation** et reste statique.

Montre :

```cpp
const int x = 3;
auto a = x;        // int
const auto b = x;  // const int
const auto& c = x; // const int&
```

---

## Structured bindings

```cpp
for (const auto& [nom, age] : ages) { ... }
```

Insister sur `auto` vs `auto&` : avec une copie, modifier la variable ne modifie pas forcément l'objet source.

---

## `if constexpr`

La branche non retenue est éliminée à la compilation dans le contexte du template. Cela permet d'écrire du code différent selon le type sans rendre toutes les branches valides pour toutes les instanciations.

---

## `optional`, `variant`, `any`

Donner une phrase par type :

- `optional<T>` : « il y a peut-être un T » ;
- `variant<A,B>` : « exactement une valeur parmi un ensemble fermé de types » ;
- `any` : « une valeur de type arbitraire connue seulement à l'exécution ».

Recommandation : préférer `optional`/`variant` lorsque le domaine permet d'exprimer les possibilités précisément ; `any` sacrifie davantage de sûreté statique.

---

# 10. Jour 4 — Sécurité et robustesse

## Buffer overflow

Fais une démonstration très simple sans exploitation :

```cpp
char nom[8];
// copie non bornée = risque
```

Puis montrer `std::string` / conteneurs et validation de taille.

Message : le problème est une écriture hors limites, qui produit un **comportement indéfini**.

---

## Use-after-free / double free

Relier immédiatement au Jour 2 : ownership + RAII.

```cpp
int* p = new int(3);
delete p;
// *p = 4; // use-after-free
```

Puis version C++ moderne avec `unique_ptr` où la destruction est automatique.

---

## Integer overflow

Pour les entiers signés, un overflow arithmétique est un comportement indéfini. Pour les non signés, l'arithmétique est modulo 2^N, mais cela ne signifie pas que le résultat est métierement correct.

Exemple réel : taille provenant de l'utilisateur avant allocation `count * sizeof(T)`.

---

## Validation d'entrée

Principe : valider **longueur, format, plage, cohérence métier** avant de traiter.

Ne confonds pas validation avec « supprimer quelques caractères dangereux ». La validation dépend du contexte : SQL, HTML, chemin de fichier, shell, etc.

---

# 11. Jour 4 — Multithreading

## Thread

Modèle mental : plusieurs chemins d'exécution partagent potentiellement la même mémoire.

```cpp
std::thread t(travail);
t.join();
```

`join` attend la fin du thread. Un `std::thread` encore joinable à sa destruction provoque `std::terminate`; mentionner `std::jthread` C++20 comme alternative RAII plus sûre.

---

## Data race

Définition utile : accès concurrents au même objet, au moins un est une écriture, sans synchronisation adéquate → comportement indéfini.

```cpp
int compteur = 0;
// plusieurs threads font ++compteur
```

`++` n'est pas une opération atomique magique.

---

## Mutex et lock guard

```cpp
{
    std::lock_guard<std::mutex> lock(m);
    ++compteur;
}
```

Relier à RAII : le verrou est libéré même si une exception survient.

`std::scoped_lock` est très pratique pour verrouiller plusieurs mutex en réduisant le risque de deadlock lié à l'ordre de verrouillage.

---

## Deadlock

Dessiner :

- thread A tient M1 et attend M2 ;
- thread B tient M2 et attend M1.

Prévention : ordre global, acquisition groupée, réduire les zones critiques.

---

## Condition variable

Ne la présente pas comme « un mutex amélioré ». Elle permet à un thread de **dormir en attendant qu'une condition devienne vraie**.

Toujours attendre avec un prédicat :

```cpp
cv.wait(lock, [&] { return !file.empty(); });
```

Cela gère les réveils spurieux et revérifie la condition sous verrou.

---

## `future`, `promise`, `async`

Image : un `future<T>` est un ticket donnant accès à une valeur qui sera disponible plus tard.

Nuance : la politique de lancement de `std::async` peut être différée si elle n'est pas explicitée. Pour une exécution asynchrone réelle :

```cpp
auto f = std::async(std::launch::async, calcul);
```

---

## Atomiques

À dire : une atomique protège **une opération/variable atomique**, pas automatiquement un invariant impliquant plusieurs variables.

`std::atomic<int> compteur` est adapté à un simple compteur, mais pas nécessairement à une structure métier complexe.

---

# 12. Jour 4 — Performance

Toujours rappeler : **mesurer avant d'optimiser**.

Ordre pédagogique :

1. cas de test représentatif ;
2. mesure de référence ;
3. profiler ;
4. identifier le hotspot ;
5. modifier une chose ;
6. re-mesurer.

Évite les chiffres de gain génériques de certaines slides comme vérités universelles : ils dépendent du programme, du compilateur, du CPU et des données.

---

# 13. Jour 5 — Projet LibraryManager

## Comment lancer la journée

> « Aujourd'hui, l'objectif n'est plus d'ajouter beaucoup de nouvelles notions. On va vérifier si vous savez choisir et combiner celles des quatre premiers jours pour produire quelque chose de maintenable. »

---

## Cahier des charges

Fais d'abord identifier par les stagiaires :

- les **entités métier** : livre, adhérent, emprunt ;
- les **cas d'utilisation** : ajouter, chercher, emprunter, retourner ;
- les **infrastructures** : JSON, SQLite, CLI ;
- les contraintes : unicité, disponibilité, dates, concurrence.

Ne laissez pas commencer immédiatement par `main.cpp`.

---

## Interface Repository

Explique comme une frontière : le domaine veut sauvegarder/rechercher des objets sans savoir si le stockage est JSON, SQLite ou mémoire.

```cpp
class ILivreRepository {
public:
    virtual void ajouter(const Livre&) = 0;
    virtual std::optional<Livre> trouver(const std::string& id) const = 0;
    virtual ~ILivreRepository() = default;
};
```

Avantage concret : tests avec un repository en mémoire, production avec SQLite.

---

## Exceptions métier

Une exception `LivreIndisponible` est plus informative qu'un simple `std::runtime_error("erreur")` si l'appelant doit afficher un message spécifique ou gérer ce cas différemment.

---

## Recherche

Fais distinguer :

- recherche exacte ;
- recherche partielle ;
- casse ;
- indexation ;
- complexité sur petit vs grand catalogue.

Ne pousse pas une optimisation full-text si le dataset est minuscule et que le but est pédagogique.

---

## Concurrence des emprunts

Le vrai invariant est : **deux threads ne doivent pas pouvoir emprunter simultanément le même exemplaire**.

La section critique doit englober le test de disponibilité **et** la modification de l'état ; protéger seulement l'écriture ne suffit pas.

---

## Tests du projet

Cas indispensables :

- ajouter puis retrouver un livre ;
- identifiant dupliqué ;
- emprunter disponible ;
- emprunter indisponible ;
- retourner ;
- recherche inexistante ;
- persistance/rechargement ;
- concurrence si elle est implémentée.

---

# 14. Ce que je passerais rapidement / garderais en bonus

Le support contient de nombreuses sections avancées au-delà du cœur de la progression. En formation de 5 jours, je les traiterais comme **capsules optionnelles** sauf profil très avancé :

- allocateur custom ;
- SFINAE complexe lorsque Concepts suffit ;
- CRTP / expression templates ;
- lock-free + hazard pointers ;
- memory ordering avancé ;
- coroutines complètes ;
- PGO/LTO détaillés ;
- type erasure avancé ;
- Event Sourcing/CQRS/DDD détaillé.

Tu peux les présenter en 2 minutes : « voilà pourquoi cela existe, voilà dans quel contexte vous le reverrez », puis revenir au fil rouge.

---

# 15. Corrections — méthode générale en salle

Pour chaque exercice :

1. **affiche l'objectif**, pas la solution ;
2. demande : « quelles structures de données / abstractions allons-nous utiliser ? » ;
3. écris le squelette ;
4. fais compléter une étape par les stagiaires ;
5. compile souvent ;
6. laisse une erreur instructive apparaître ;
7. à la fin seulement, compare avec la solution complète.

Les sections suivantes sont écrites dans le format demandé : **corrigé complet d'abord, déroulé pas à pas ensuite**.

---

# 16. Corrections prioritaires — Jour 2

## 16.1 `vector` + algorithmes — pipeline STL

### Corrigé complet

```cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <unordered_set>
#include <vector>

int main() {
    std::vector<int> valeurs = {
        12, 7, 18, 12, 5, 20, 44, 18, 31, 50,
        2, 9, 44, 16, 27, 6, 33, 8, 40, 10
    };

    // 1. Supprimer les doublons sans imposer d'ordre.
    std::unordered_set<int> uniques(valeurs.begin(), valeurs.end());
    std::vector<int> resultat(uniques.begin(), uniques.end());

    // 2. Garder les valeurs paires.
    resultat.erase(
        std::remove_if(resultat.begin(), resultat.end(),
                       [](int x) { return x % 2 != 0; }),
        resultat.end());

    // 3. Multiplier par 3.
    std::transform(resultat.begin(), resultat.end(), resultat.begin(),
                   [](int x) { return x * 3; });

    // 4. Trier décroissant.
    std::sort(resultat.begin(), resultat.end(), std::greater<>{});

    // 5. Garder au maximum les 5 premières valeurs.
    if (resultat.size() > 5) {
        resultat.resize(5);
    }

    // 6. Somme et produit.
    const int somme = std::accumulate(resultat.begin(), resultat.end(), 0);
    const long long produit =
        std::accumulate(resultat.begin(), resultat.end(), 1LL,
                        std::multiplies<>{});

    std::cout << "Valeurs finales : ";
    for (int x : resultat) std::cout << x << ' ';
    std::cout << "\nSomme = " << somme
              << "\nProduit = " << produit << '\n';
}
```

### Déroulé de correction en direct

**Étape 1 — faire choisir les structures.** Demande : « Pour supprimer des doublons sans conserver l'ordre, quelle structure connaît déjà l'unicité ? » Attendre `set`/`unordered_set`, puis demander laquelle ne trie pas.

**Étape 2 — filtrage.** Avant de donner `remove_if`, demander comment ils feraient avec une boucle. Puis montrer l'idiome erase-remove et rappeler que `remove_if` ne réduit pas la taille physique.

**Étape 3 — transformation.** Leur faire proposer `transform`. Demander : « Pourquoi peut-on écrire la sortie dans le même vecteur ? » Parce que chaque élément est transformé indépendamment et la taille ne change pas.

**Étape 4 — tri.** Faire écrire la lambda décroissante, puis simplifier avec `std::greater<>`.

**Étape 5 — top 5.** Demander ce que ferait `resize(5)` si le vecteur ne contenait que 3 valeurs : il ajouterait 2 valeurs initialisées à zéro. D'où le `if`.

**Étape 6 — accumulate.** Faire volontairement démarrer le produit avec `0`, demander le résultat, puis corriger à `1LL`. C'est un bon moment pour parler de l'élément neutre.

**Question de clôture.** « Cette solution est-elle littéralement une seule expression ? Non. L'objectif pédagogique est surtout d'enchaîner des briques STL sans réécrire les mécanismes. En C++20, les ranges permettent d'aller plus loin dans le style pipeline. »

---

## 16.2 Catalogue produits — STL avancée

### Corrigé complet

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <vector>

struct Produit {
    std::string ref;
    std::string nom;
    double prix;
    int stock;
    std::string categorie;
};

int main() {
    std::vector<Produit> catalogue = {
        {"P01", "Clavier", 49.90, 5, "Informatique"},
        {"P02", "Souris", 29.90, 0, "Informatique"},
        {"P03", "Cable", 9.90, 50, "Accessoires"},
        {"P04", "Ecran", 199.90, 3, "Informatique"},
        {"P05", "Housse", 24.90, 8, "Accessoires"}
    };

    // 1. Produits en stock et à moins de 50 €.
    std::vector<Produit> disponibles;
    std::copy_if(catalogue.begin(), catalogue.end(),
                 std::back_inserter(disponibles),
                 [](const Produit& p) {
                     return p.stock > 0 && p.prix < 50.0;
                 });

    // 2. Prix croissant puis nom.
    std::sort(disponibles.begin(), disponibles.end(),
              [](const Produit& a, const Produit& b) {
                  if (a.prix != b.prix) return a.prix < b.prix;
                  return a.nom < b.nom;
              });

    // 3. Groupement par catégorie.
    std::map<std::string, std::vector<Produit>> parCategorie;
    for (const auto& p : catalogue) {
        parCategorie[p.categorie].push_back(p);
    }

    // 4. Prix moyen par catégorie.
    std::map<std::string, double> moyenneParCategorie;
    for (const auto& [categorie, produits] : parCategorie) {
        const double somme = std::accumulate(
            produits.begin(), produits.end(), 0.0,
            [](double acc, const Produit& p) {
                return acc + p.prix;
            });
        moyenneParCategorie[categorie] = somme / produits.size();
    }

    // 5. Produit en stock le plus cher.
    auto plusCher = catalogue.end();
    for (auto it = catalogue.begin(); it != catalogue.end(); ++it) {
        if (it->stock <= 0) continue;
        if (plusCher == catalogue.end() || it->prix > plusCher->prix) {
            plusCher = it;
        }
    }

    // 6. Remise de 10 % au-dessus de 100 €.
    std::for_each(catalogue.begin(), catalogue.end(),
                  [](Produit& p) {
                      if (p.prix > 100.0) p.prix *= 0.90;
                  });

    if (plusCher != catalogue.end()) {
        std::cout << "Plus cher en stock : "
                  << plusCher->nom << "\n";
    }
}
```

### Déroulé pas à pas

1. Commence par le filtre et demande : « Est-ce qu'on modifie `catalogue` ou crée-t-on une vue/résultat séparé ? » Ici on crée un résultat séparé pour conserver la source.
2. Sur le tri, fais expliciter l'ordre secondaire : prix égal → nom.
3. Pour le groupement, explique que `map[categorie]` crée automatiquement le vecteur vide si la clé n'existe pas.
4. Sur la moyenne, insiste sur `0.0` et non `0` pour garder un accumulateur `double`.
5. Pour le maximum, préfère ici une boucle claire plutôt qu'une lambda subtile qui traite mal les produits hors stock. C'est un bon exemple montrant que « utiliser uniquement des algorithmes STL » n'est pas toujours synonyme de code plus clair.
6. Pour la remise, demander : « Pourquoi le paramètre de lambda est-il `Produit&` et non `const Produit&` ? » Parce qu'on modifie le produit.

---

## 16.3 Factory de parseurs

### Corrigé complet

```cpp
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class IParser {
public:
    virtual std::string parse(const std::string& contenu) = 0;
    virtual std::string format() const = 0;
    virtual ~IParser() = default;
};

class ParserJSON : public IParser {
public:
    std::string parse(const std::string& contenu) override {
        return "JSON:" + contenu;
    }
    std::string format() const override { return "json"; }
};

class ParserCSV : public IParser {
public:
    std::string parse(const std::string& contenu) override {
        return "CSV:" + contenu;
    }
    std::string format() const override { return "csv"; }
};

class ParserFactory {
public:
    using Createur = std::function<std::unique_ptr<IParser>()>;

    void enregistrer(std::string format, Createur createur) {
        createurs[std::move(format)] = std::move(createur);
    }

    std::unique_ptr<IParser> creer(const std::string& format) const {
        auto it = createurs.find(format);
        if (it == createurs.end()) {
            throw std::invalid_argument("Format inconnu : " + format);
        }
        return it->second();
    }

    std::vector<std::string> formatsDisponibles() const {
        std::vector<std::string> resultat;
        resultat.reserve(createurs.size());
        for (const auto& [format, _] : createurs) {
            resultat.push_back(format);
        }
        return resultat;
    }

private:
    std::unordered_map<std::string, Createur> createurs;
};

int main() {
    ParserFactory factory;
    factory.enregistrer("json", [] {
        return std::make_unique<ParserJSON>();
    });
    factory.enregistrer("csv", [] {
        return std::make_unique<ParserCSV>();
    });

    auto parser = factory.creer("json");
    parser->parse("{...}");
}
```

### Déroulé pas à pas

1. Écris d'abord `IParser` : le client doit dépendre d'une interface, pas des classes concrètes.
2. Ajoute deux parseurs concrets.
3. Pose la question : « Si demain j'ajoute TOML, voulons-nous modifier un gros `if` partout ? » Non.
4. Introduis le type `Createur` comme fonction qui fabrique un `unique_ptr<IParser>`.
5. Ajoute la map format → créateur.
6. Écris `enregistrer`, puis `creer`.
7. Fais remarquer que la Factory ne possède pas les parseurs créés : elle possède seulement leurs **fonctions de création**.
8. Termine par l'enregistrement via lambdas. Cela fait le lien STL + lambdas + polymorphisme + smart pointers.

---

## 16.4 Observer météo

### Corrigé complet

```cpp
#include <deque>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

struct DonneesMeteo {
    double temperature;
    double humidite;
    double vent;
    double pression;
};

class StationMeteo {
public:
    using Observateur = std::function<void(const DonneesMeteo&)>;

    void abonner(Observateur fn) {
        observateurs.push_back(std::move(fn));
    }

    void mettreAJour(DonneesMeteo nouvelles) {
        donnees = nouvelles;
        for (auto& fn : observateurs) {
            fn(donnees);
        }
    }

private:
    DonneesMeteo donnees{};
    std::vector<Observateur> observateurs;
};

class Historique {
public:
    void ajouter(const DonneesMeteo& d) {
        mesures.push_back(d);
        if (mesures.size() > 100) {
            mesures.pop_front();
        }
    }

private:
    std::deque<DonneesMeteo> mesures;
};

int main() {
    StationMeteo station;
    Historique historique;

    station.abonner([](const DonneesMeteo& d) {
        std::cout << "T=" << d.temperature
                  << " C, H=" << d.humidite << "%\n";
    });

    station.abonner([](const DonneesMeteo& d) {
        if (d.temperature > 35.0)
            std::cout << "ALERTE CHALEUR\n";
    });

    station.abonner([](const DonneesMeteo& d) {
        if (d.temperature < 0.0)
            std::cout << "ALERTE GEL\n";
    });

    station.abonner([&historique](const DonneesMeteo& d) {
        historique.ajouter(d);
    });

    station.mettreAJour({38.2, 42.0, 18.0, 1014.0});
}
```

### Déroulé pas à pas

1. Demande : « Qui connaît qui ? » La station connaît une liste de callbacks, mais elle n'a pas besoin de connaître `AlerteChaleur`, `Historique`, etc.
2. Écris `using Observateur = std::function<...>`.
3. Écris `abonner`.
4. Écris `mettreAJour` puis la boucle de notification.
5. Ajoute un observateur simple d'affichage.
6. Ajoute une alerte pour montrer que les comportements sont indépendants.
7. Ajoute `Historique` capturé par référence et demande : « Quelle contrainte de durée de vie cela introduit ? » `historique` doit rester vivant aussi longtemps que la callback est utilisée.
8. Conclus sur le vrai sujet du pattern : **découpler l'émetteur des réactions**.

---

# 17. Corrections prioritaires — Jour 3

## 17.1 Templates de fonctions

### Corrigé complet

```cpp
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T>
T maximum(T a, T b) {
    return (a < b) ? b : a;
}

template<typename T>
T clampValeur(T val, T min, T max) {
    if (val < min) return min;
    if (max < val) return max;
    return val;
}

template<typename T>
void echanger(T& a, T& b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

template<typename Container>
typename Container::value_type somme(const Container& c) {
    using T = typename Container::value_type;
    T resultat{};
    for (const auto& valeur : c) {
        resultat += valeur;
    }
    return resultat;
}

int main() {
    std::cout << maximum(3, 8) << '\n';
    std::cout << maximum(3.5, 2.1) << '\n';
    std::cout << maximum(std::string("chat"), std::string("chien")) << '\n';

    std::vector<double> valeurs{1.5, 2.5, 3.0};
    std::cout << somme(valeurs) << '\n';
}
```

### Déroulé pas à pas

1. Commence par `maximum`. Demande : « Quelle contrainte implicite avons-nous sur `T` ? » Il doit être comparable avec l'opérateur utilisé.
2. Montre que le template n'impose pas « numérique ». `std::string` fonctionne aussi grâce à son ordre lexicographique.
3. Écris `clampValeur` et insiste sur l'ordre des comparaisons. Ne parle pas tout de suite de `std::clamp`, puis montre à la fin que la STL possède déjà l'algorithme.
4. Pour `echanger`, demande quelle serait la version naïve avec copie, puis montre `std::move`. Rappeler que le type doit supporter les opérations utilisées.
5. Pour `somme`, arrête-toi sur `typename Container::value_type`. Explique que `value_type` est un nom de type dépendant du paramètre template, d'où `typename`.
6. Sur `T resultat{};`, rappeler l'initialisation par défaut sûre : `0` pour types arithmétiques, chaîne vide pour `std::string`, etc., sous réserve que le type soit default-constructible.
7. Conclusion : un template déplace une partie du contrôle du runtime vers la compilation.

---

## 17.2 Concepts C++20

### Corrigé complet

```cpp
#include <algorithm>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

template<typename T>
concept Triable = requires(T& c) {
    c.begin();
    c.end();
    { *c.begin() < *c.begin() } -> std::convertible_to<bool>;
};

template<typename T>
concept Serialisable = requires(const T& obj, const std::string& texte) {
    { obj.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(texte) } -> std::same_as<T>;
};

template<typename T>
concept Numerique = std::integral<T> || std::floating_point<T>;

template<Triable C>
void trierEtAfficher(C& conteneur) {
    std::sort(conteneur.begin(), conteneur.end());
    for (const auto& x : conteneur) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

template<Numerique T>
T racineCarree(T x) {
    return static_cast<T>(std::sqrt(x));
}

void sauvegarder(const Serialisable auto& obj) {
    std::cout << obj.serialize() << '\n';
}

struct Utilisateur {
    std::string nom;

    std::string serialize() const {
        return nom;
    }

    static Utilisateur deserialize(const std::string& texte) {
        return {texte};
    }
};

int main() {
    std::vector<int> v{4, 1, 3, 2};
    trierEtAfficher(v);

    std::cout << racineCarree(25.0) << '\n';

    Utilisateur u{"Alice"};
    sauvegarder(u);
}
```

### Déroulé pas à pas

1. Repars du problème : « Je veux une fonction générique, mais pas pour n'importe quoi. »
2. Écris `Triable` progressivement : seulement `begin/end`, puis ajoute la comparaison.
3. Demande : « Est-ce que cela garantit que `std::sort` fonctionnera pour tous les conteneurs répondant à ce concept ? » Pas complètement : `std::sort` exige des itérateurs random-access. C'est un excellent point pour montrer qu'un concept doit représenter précisément le contrat réel.
4. Propose une version plus stricte si besoin avec `std::ranges::random_access_range`.
5. Écris `Serialisable` : le concept vérifie une API, pas une classe de base.
6. Montre `Numerique` comme composition de concepts standard.
7. Compile volontairement `racineCarree(std::string{"9"})` pour montrer la qualité du diagnostic.
8. Conclus : Concepts remplace souvent un SFINAE difficile à lire par un contrat de type explicite.

---

## 17.3 Hiérarchie d'exceptions bancaire — version pédagogique

### Corrigé complet

```cpp
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

class ExceptionBancaire : public std::exception {
public:
    ExceptionBancaire(std::string code, std::string message)
        : code_(std::move(code)), message_(std::move(message)) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    const std::string& code() const noexcept {
        return code_;
    }

private:
    std::string code_;
    std::string message_;
};

std::ostream& operator<<(std::ostream& os, const ExceptionBancaire& e) {
    return os << '[' << e.code() << "] " << e.what();
}

class ErreurValidation : public ExceptionBancaire {
public:
    ErreurValidation(std::string code, std::string message)
        : ExceptionBancaire(std::move(code), std::move(message)) {}
};

class ErreurMetier : public ExceptionBancaire {
public:
    ErreurMetier(std::string code, std::string message)
        : ExceptionBancaire(std::move(code), std::move(message)) {}
};

class ErreurTechnique : public ExceptionBancaire {
public:
    ErreurTechnique(std::string code, std::string message)
        : ExceptionBancaire(std::move(code), std::move(message)) {}
};

class ChampObligatoire : public ErreurValidation {
public:
    explicit ChampObligatoire(const std::string& champ)
        : ErreurValidation("BNK-VAL-001",
                           "Champ obligatoire manquant : " + champ) {}
};

class SoldeInsuffisant : public ErreurMetier {
public:
    SoldeInsuffisant(double solde, double montant)
        : ErreurMetier("BNK-MET-001", construireMessage(solde, montant)) {}

private:
    static std::string construireMessage(double solde, double montant) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "Solde insuffisant : solde=" << solde
            << ", montant demande=" << montant;
        return oss.str();
    }
};

class CompteInexistant : public ErreurMetier {
public:
    explicit CompteInexistant(const std::string& numero)
        : ErreurMetier("BNK-MET-002",
                       "Compte inexistant : " + numero) {}
};

class ErreurConnexionBDD : public ErreurTechnique {
public:
    explicit ErreurConnexionBDD(const std::string& message)
        : ErreurTechnique("BNK-TEC-001",
                          "Erreur de connexion BDD : " + message) {}
};

int main() {
    try {
        throw SoldeInsuffisant(120.50, 250.00);
    }
    catch (const ErreurMetier& e) {
        std::cerr << "Erreur metier interceptee : " << e << '\n';
    }
    catch (const ExceptionBancaire& e) {
        std::cerr << "Erreur bancaire : " << e << '\n';
    }
}
```

### Déroulé pas à pas

1. Ne commence pas par écrire 12 classes. Fais d'abord la base avec `code`, `message`, `what()`.
2. Explique pourquoi `what()` doit être `noexcept`.
3. Ajoute les trois grandes catégories : validation, métier, technique.
4. Demande au groupe : « Pourquoi cette classification est-elle utile ? » Parce que la couche appelante peut réagir différemment : message utilisateur pour validation, rollback pour métier, retry/log/alerte pour technique.
5. Implémente seulement trois exceptions concrètes pendant la correction. Les autres se déduisent du même patron.
6. Montre l'ordre des `catch` : du plus spécifique au plus général.
7. Question importante : « Faut-il créer une classe différente pour chaque message d'erreur ? » Non. Seulement si le type apporte une sémantique utile à la gestion de l'erreur.

---

## 17.4 Projet CMake multi-cibles

### Corrigé complet — `CMakeLists.txt` racine

```cmake
cmake_minimum_required(VERSION 3.20)
project(monprojet VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(monlib STATIC
    src/calcul.cpp
    src/utils.cpp
)

target_include_directories(monlib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(monlib PUBLIC cxx_std_17)

target_compile_options(monlib
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_executable(monapp src/main.cpp)
target_link_libraries(monapp PRIVATE monlib)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
```

### Corrigé complet — `tests/CMakeLists.txt`

```cmake
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests
    test_calcul.cpp
    test_utils.cpp
)

target_link_libraries(tests
    PRIVATE
        monlib
        Catch2::Catch2WithMain
)

target_compile_features(tests PRIVATE cxx_std_17)

include(Catch)
catch_discover_tests(tests)
```

### Corrigé complet — `CMakePresets.json`

```json
{
  "version": 6,
  "configurePresets": [
    {
      "name": "debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "BUILD_TESTING": "ON"
      }
    },
    {
      "name": "release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "BUILD_TESTING": "ON"
      }
    }
  ],
  "buildPresets": [
    { "name": "debug", "configurePreset": "debug" },
    { "name": "release", "configurePreset": "release" }
  ],
  "testPresets": [
    {
      "name": "debug",
      "configurePreset": "debug",
      "output": { "outputOnFailure": true }
    }
  ]
}
```

### Déroulé pas à pas

1. Écris uniquement `cmake_minimum_required`, `project`, `add_library`.
2. Fais verbaliser : « Une target est l'unité centrale de configuration CMake moderne. »
3. Ajoute les includes **sur la target** et explique `PUBLIC` : `monlib` en a besoin et ses consommateurs aussi.
4. Ajoute `target_compile_features` plutôt qu'un `set(CMAKE_CXX_STANDARD ...)` global. Montrer que le besoin suit la cible.
5. Ajoute l'exécutable puis `target_link_libraries`.
6. Active les tests avec `CTest`.
7. Dans `tests`, introduis Catch2 avec `FetchContent`.
8. Termine par les presets : ils mémorisent une configuration reproductible, ils ne remplacent pas CMake.
9. Commandes à faire taper :

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

---

# 18. Corrections prioritaires — Jour 4

## 18.1 Lambdas et captures

### Corrigé complet

```cpp
#include <iostream>
#include <memory>

int main() {
    int x = 10;

    auto l1 = [x]() { std::cout << x << '\n'; };
    x = 20;
    l1(); // 10

    auto l2 = [&x]() { std::cout << x << '\n'; };
    x = 30;
    l2(); // 30

    auto l3 = [x]() mutable {
        ++x;
        std::cout << x << '\n';
    };
    l3(); // 31
    l3(); // 32
    l3(); // 33
    std::cout << "x externe = " << x << '\n'; // 30

    int a = 1;
    int b = 2;
    auto l4 = [=]() { std::cout << a + b << '\n'; };
    a = 10;
    b = 20;
    l4(); // 3

    auto ptr = std::make_unique<int>(42);
    auto l5 = [p = std::move(ptr)]() {
        std::cout << *p << '\n';
    };

    l5();
    l5(); // possible : la lambda conserve son unique_ptr

    std::cout << std::boolalpha
              << "ptr encore valide ? " << static_cast<bool>(ptr) << '\n';
    // false : ptr a été déplacé dans la closure
}
```

### Déroulé pas à pas

1. Fais prédire toutes les sorties **avant compilation**.
2. `l1` : explique que la valeur capturée est celle du moment où la lambda est créée.
3. `l2` : la lambda consulte le même `x` que le code extérieur.
4. `l3` : `mutable` autorise la modification de la **copie interne** de `x`, pas du `x` extérieur.
5. `l4` : `[=]` capture les variables utilisées par valeur au moment de la création.
6. `l5` : l'init-capture permet de déplacer un objet non copiable dans la closure.
7. Demande : « Pourquoi `l5` peut-elle être appelée deux fois alors que `unique_ptr` n'est pas copiable ? » Parce que le pointeur est un membre de l'objet lambda ; l'appel ne le déplace pas à nouveau.
8. Termine avec le piège de durée de vie d'une capture par référence.

---

## 18.2 Types modernes : `optional` et `variant`

### Corrigé complet

```cpp
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Utilisateur {
    int id;
    std::string nom;
};

std::optional<Utilisateur>
trouverUtilisateur(const std::vector<Utilisateur>& utilisateurs, int id) {
    for (const auto& u : utilisateurs) {
        if (u.id == id) return u;
    }
    return std::nullopt;
}

using ResultatChargement =
    std::variant<Utilisateur, std::string>;

ResultatChargement chargerUtilisateur(bool succes) {
    if (succes) return Utilisateur{42, "Alice"};
    return std::string{"Utilisateur introuvable"};
}

int main() {
    std::vector<Utilisateur> utilisateurs{
        {1, "Alice"}, {2, "Bob"}
    };

    if (auto u = trouverUtilisateur(utilisateurs, 2)) {
        std::cout << "Trouve : " << u->nom << '\n';
    }

    auto resultat = chargerUtilisateur(false);

    std::visit([](const auto& valeur) {
        using T = std::decay_t<decltype(valeur)>;
        if constexpr (std::is_same_v<T, Utilisateur>) {
            std::cout << "Utilisateur : " << valeur.nom << '\n';
        } else {
            std::cout << "Erreur : " << valeur << '\n';
        }
    }, resultat);
}
```

### Déroulé pas à pas

1. Pose : « Comment représentiez-vous auparavant “pas de résultat” ? » `nullptr`, valeur sentinelle, bool + paramètre de sortie, exception...
2. Écris `optional<Utilisateur>` et explique que l'absence fait désormais partie du type.
3. Montre le `if (auto u = ...)` et `u->nom`.
4. Passe à `variant` : « Ici il y a toujours une valeur, mais elle peut être de deux natures différentes. »
5. Utilise `std::visit` et `if constexpr` pour relier les notions des slides précédentes.
6. Demande : « Pourquoi ne pas utiliser `any` ? » Parce que les possibilités sont connues : `variant` encode ce contrat dans le type.

---

## 18.3 Sécurité mémoire — correction RAII d'un arbre

### Corrigé complet

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Noeud {
public:
    explicit Noeud(std::string v)
        : valeur(std::move(v)) {}

    Noeud& ajouterEnfant(std::string valeurEnfant) {
        enfants.push_back(
            std::make_unique<Noeud>(std::move(valeurEnfant)));
        return *enfants.back();
    }

    Noeud* trouver(const std::string& recherche) {
        if (valeur == recherche) return this;

        for (auto& enfant : enfants) {
            if (auto* trouve = enfant->trouver(recherche)) {
                return trouve;
            }
        }
        return nullptr;
    }

    void afficher(int profondeur = 0) const {
        std::cout << std::string(profondeur * 2, ' ')
                  << valeur << '\n';
        for (const auto& enfant : enfants) {
            enfant->afficher(profondeur + 1);
        }
    }

private:
    std::string valeur;
    std::vector<std::unique_ptr<Noeud>> enfants;
};

class Arbre {
public:
    void setRacine(std::string valeur) {
        racine = std::make_unique<Noeud>(std::move(valeur));
    }

    bool ajouter(const std::string& parent,
                 const std::string& enfant) {
        if (!racine) return false;

        Noeud* p = racine->trouver(parent);
        if (!p) return false;

        p->ajouterEnfant(enfant);
        return true;
    }

    void afficher() const {
        if (racine) racine->afficher();
    }

private:
    std::unique_ptr<Noeud> racine;
};

int main() {
    Arbre arbre;
    arbre.setRacine("Racine");
    arbre.ajouter("Racine", "Enfant1");
    arbre.ajouter("Racine", "Enfant2");
    arbre.ajouter("Enfant1", "Petit-enfant1");
    arbre.afficher();
}
```

### Déroulé pas à pas

1. Commence par exécuter mentalement le programme d'origine : `racine == nullptr`, puis `trouver(racine, ...)` déréférence immédiatement `nullptr`.
2. Corrige d'abord le crash minimal : vérifier `noeud` avant `noeud->valeur` et initialiser la racine.
3. Puis demande : « Même si ça ne crashe plus, quel problème reste ? » Tous les `new` ne sont jamais libérés.
4. Remplace la racine par `unique_ptr<Noeud>` : ownership exclusif évident.
5. Remplace les enfants par `vector<unique_ptr<Noeud>>`. Le parent possède ses enfants.
6. Montre que le destructeur n'a plus besoin d'être écrit : destruction récursive automatique.
7. Conserve un `Noeud*` non propriétaire pour la recherche. Insiste : **un raw pointer n'est pas interdit ; il est acceptable lorsqu'il exprime une observation non propriétaire dont la durée de vie est maîtrisée.**
8. Relance avec AddressSanitizer pour montrer l'absence de fuite/use-after-free.

---

## 18.4 Synchronisation — compteur et file de travaux

### Corrigé complet

```cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class FileTravail {
public:
    void pousser(std::string tache) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            file_.push(std::move(tache));
        }
        cv_.notify_one();
    }

    bool attendreEtRetirer(std::string& sortie) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {
            return arret_ || !file_.empty();
        });

        if (arret_ && file_.empty()) {
            return false;
        }

        sortie = std::move(file_.front());
        file_.pop();
        return true;
    }

    void arreter() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            arret_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<std::string> file_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool arret_ = false;
};

int main() {
    FileTravail file;

    std::thread consommateur([&] {
        std::string tache;
        while (file.attendreEtRetirer(tache)) {
            std::cout << "Traitement : " << tache << '\n';
        }
    });

    file.pousser("Compiler");
    file.pousser("Tester");
    file.pousser("Deployer");

    file.arreter();
    consommateur.join();
}
```

### Déroulé pas à pas

1. Pars d'une `queue` simple et demande ce qui se passe si producteur et consommateur y accèdent en même temps.
2. Ajoute le mutex pour protéger **l'invariant complet de la queue**.
3. Demande : « Le consommateur doit-il boucler à 100 % CPU en vérifiant sans cesse si la queue est vide ? » Non.
4. Introduis `condition_variable` : le thread dort jusqu'à notification.
5. Montre pourquoi `wait` prend un `unique_lock` : il doit pouvoir déverrouiller le mutex pendant l'attente puis le reverrouiller.
6. Explique le prédicat : gérer réveils spurieux + état d'arrêt.
7. Déplace `notify_one()` après la zone verrouillée : c'est généralement plus efficace, car le thread réveillé peut acquérir le mutex immédiatement.
8. Termine par `join` et le protocole d'arrêt. Un système concurrent a besoin d'une stratégie de fin, pas seulement d'une stratégie de démarrage.

---

## 18.5 `std::async` / `future`

### Corrigé complet

```cpp
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

long long calculLong() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 42;
}

int main() {
    auto futur = std::async(std::launch::async, calculLong);

    std::cout << "Le thread principal continue...\n";

    if (futur.wait_for(std::chrono::milliseconds(100))
        == std::future_status::timeout) {
        std::cout << "Pas encore termine\n";
    }

    try {
        const long long resultat = futur.get();
        std::cout << "Resultat = " << resultat << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur asynchrone : " << e.what() << '\n';
    }
}
```

### Déroulé pas à pas

1. Demande : « Comment récupérer la valeur retournée par une tâche exécutée ailleurs ? »
2. Introduis le couple tâche/future.
3. Explique `std::launch::async` pour éviter l'ambiguïté d'une exécution différée.
4. Montre `wait_for` : attendre n'est pas nécessairement bloquer indéfiniment.
5. Explique que `get()` récupère la valeur **ou relance l'exception produite dans la tâche**.
6. Mentionne que `get()` n'est généralement appelé qu'une fois sur un `future` classique.

---

# 19. Corrections prioritaires — Jour 5

## 19.1 Code review — gestionnaire de fichiers

### Problèmes à faire trouver avant de corriger

Au moins :

1. résultat de `fopen` non vérifié ;
2. `nullptr` peut être stocké dans le vecteur ;
3. ownership de `FILE*` manuel et implicite ;
4. destructeur vide → fuite de ressources ;
5. `fermerTous()` peut être oublié ;
6. appel multiple à `fermerTous()` pourrait conduire à des usages invalides si le vecteur n'est pas nettoyé ;
7. `index` non vérifié ;
8. `fread` peut lire moins de 256 octets ;
9. le buffer n'est pas nécessairement terminé par `\0` ;
10. `std::string(buffer)` suppose une chaîne C terminée ;
11. erreurs de lecture non gérées ;
12. copie implicite de la classe dupliquerait les raw pointers, donc risque de double `fclose` si un destructeur correct était ajouté.

### Corrigé complet en C++ moderne

```cpp
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class GestionnaireFichiers {
public:
    std::size_t ouvrirFichier(const std::string& path) {
        std::ifstream fichier(path, std::ios::binary);
        if (!fichier) {
            throw std::runtime_error(
                "Impossible d'ouvrir le fichier : " + path);
        }

        fichiers.push_back(std::move(fichier));
        return fichiers.size() - 1;
    }

    std::string lire(std::size_t index) {
        if (index >= fichiers.size()) {
            throw std::out_of_range("Index de fichier invalide");
        }

        auto& fichier = fichiers[index];
        fichier.clear();
        fichier.seekg(0);

        std::ostringstream contenu;
        contenu << fichier.rdbuf();

        if (fichier.bad()) {
            throw std::runtime_error("Erreur de lecture");
        }

        return contenu.str();
    }

private:
    std::vector<std::ifstream> fichiers;
};

int main() {
    try {
        GestionnaireFichiers g;
        const auto id = g.ouvrirFichier("exemple.txt");
        const auto contenu = g.lire(id);
        // Aucun fermerTous() : les ifstream se ferment automatiquement.
    }
    catch (const std::exception& e) {
        // traitement/log approprié
    }
}
```

### Déroulé de correction

1. **Ne montre pas tout de suite RAII.** Demande d'abord au groupe de lister les problèmes ligne par ligne.
2. Sur `fopen`, demande : « Que vaut `f` si l'ouverture échoue ? »
3. Sur `fread`, demande : « Où est garanti le `\0` ? » Réponse : nulle part.
4. Pose ensuite la question d'architecture : « Pourquoi avons-nous besoin de `FILE*` ici ? » La réponse est généralement : aucune raison particulière.
5. Remplace par `std::ifstream`. Explique que le flux est un objet RAII : le descripteur est fermé avec l'objet.
6. Ajoute le contrôle d'index et les erreurs d'ouverture.
7. Conclus : la modernisation n'est pas seulement syntaxique ; elle **supprime des états invalides et des responsabilités manuelles**.

---

## 19.2 Code review — cache concurrent

### Problèmes à faire trouver

1. thread détaché capturant `this` : il peut accéder au `Cache` après destruction ;
2. aucune condition d'arrêt ;
3. `set` accède à `data` sans mutex ;
4. `contains` puis `get` est une séquence TOCTOU : l'état peut changer entre les deux ;
5. `data.at` lance si la clé disparaît ;
6. `detach` rend la synchronisation de fin impossible ;
7. le destructeur ne coordonne pas le thread ;
8. verrouiller chaque méthode ne rend pas atomique une séquence de plusieurs appels métier.

### Corrigé complet C++20 avec `jthread`

```cpp
#include <chrono>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>

class Cache {
public:
    Cache()
        : nettoyeur([this](std::stop_token token) {
            while (!token.stop_requested()) {
                for (int i = 0;
                     i < 60 && !token.stop_requested();
                     ++i) {
                    std::this_thread::sleep_for(
                        std::chrono::seconds(1));
                }

                if (token.stop_requested()) break;

                std::lock_guard<std::mutex> lock(mtx);
                data.clear();
            }
        }) {}

    std::optional<std::string>
    get(const std::string& cle) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = data.find(cle);
        if (it == data.end()) return std::nullopt;
        return it->second;
    }

    void set(std::string cle, std::string val) {
        std::lock_guard<std::mutex> lock(mtx);
        data[std::move(cle)] = std::move(val);
    }

private:
    mutable std::mutex mtx;
    std::map<std::string, std::string> data;
    std::jthread nettoyeur;
};

int main() {
    Cache cache;
    cache.set("user", "Alice");

    if (auto valeur = cache.get("user")) {
        // présence + récupération faites sous un seul verrou
    }
}
```

### Déroulé pas à pas

1. Commence par le `detach`. Demande : « Qui garantit que `this` existe encore dans 60 secondes ? » Personne.
2. Remplace par `jthread`. Expliquer que sa destruction demande l'arrêt et rejoint le thread.
3. Verrouille `set`. Rappeler : une seule écriture non synchronisée suffit à provoquer un data race.
4. Supprime le couple public `contains` + `get` du scénario d'utilisation. Fais retourner un `optional` depuis `get`.
5. Explique TOCTOU : « time of check to time of use » ; l'information vérifiée peut devenir fausse avant son utilisation.
6. Demande : « Le mutex doit-il protéger la map ou l'invariant ? » L'invariant. Ici, lookup + copie de valeur doivent être une opération cohérente.
7. Termine en rappelant que les primitives concurrentes résolvent des problèmes de cycle de vie autant que des problèmes de verrouillage.

---

## 19.3 Refactoring C++98 → C++17

### Corrigé complet

```cpp
#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Personne {
    std::string nom;
    int age;
};

using ListePersonnes = std::vector<Personne>;

bool ajouterPersonne(ListePersonnes& liste,
                     std::string nom,
                     int age) {
    if (nom.empty()) return false;
    if (age < 0 || age > 150) return false;

    liste.push_back({std::move(nom), age});
    return true;
}

std::optional<Personne>
trouverPlusVieux(const ListePersonnes& liste) {
    if (liste.empty()) return std::nullopt;

    auto it = std::max_element(
        liste.begin(), liste.end(),
        [](const Personne& a, const Personne& b) {
            return a.age < b.age;
        });

    return *it;
}

void afficher(const ListePersonnes& liste) {
    for (const auto& personne : liste) {
        std::cout << personne.nom
                  << " : " << personne.age << '\n';
    }
}

int main() {
    ListePersonnes liste;

    ajouterPersonne(liste, "Alice", 30);
    ajouterPersonne(liste, "Bob", 25);
    ajouterPersonne(liste, "Charlie", 35);

    std::sort(
        liste.begin(), liste.end(),
        [](const Personne& a, const Personne& b) {
            return a.age < b.age;
        });

    afficher(liste);

    if (auto plusVieux = trouverPlusVieux(liste)) {
        std::cout << "Plus vieux : "
                  << plusVieux->nom << '\n';
    }
}
```

### Déroulé pas à pas

1. Commence par la question la plus importante : « Pourquoi `creerListe()` retourne-t-elle un pointeur alloué dynamiquement ? » Il n'y a aucune nécessité d'allocation dynamique.
2. Remplace la création/déstruction manuelle par une variable automatique `ListePersonnes liste;`.
3. Remplace `pair<string,int>` par `Personne`. Demande au groupe laquelle des deux écritures porte le mieux le métier.
4. Passe les collections par référence plutôt que par pointeur nullable lorsque `nullptr` n'est pas un état utile.
5. Remplace les fonctions de comparaison globales par des lambdas au point d'usage.
6. Remplace la boucle indexée par range-for.
7. Remplace la recherche manuelle du maximum par `max_element`.
8. Pour l'absence de personne, introduis `optional`. Ici on retourne une copie pour garder un exemple sûr et simple ; si l'objet était lourd, discuter `optional<reference_wrapper<const Personne>>` ou itérateur.
9. Conclus : la version moderne contient moins de gestion mémoire, moins d'états invalides et exprime mieux l'intention.

---

## 19.4 Debugging arbre — stratégie de correction

La correction complète RAII se trouve déjà en **18.3**. Pour le Jour 5, ne redonne pas immédiatement cette version. Utilise l'exercice comme démonstration de démarche :

1. compiler avec symboles et ASan ;
2. reproduire le crash ;
3. lire **la première erreur utile** du rapport ;
4. remonter à la ligne où `nullptr` est déréférencé ;
5. corriger la précondition racine ;
6. relancer ;
7. chercher les fuites ;
8. seulement ensuite refactorer l'ownership avec `unique_ptr`.

Phrase à dire :

> « Déboguer n'est pas deviner. C'est formuler une hypothèse à partir d'un symptôme, obtenir une preuve, corriger, puis reproduire à nouveau. »

---

## 19.5 Optimisation mesurée — correction d'architecture

### Corrigé complet de l'idée centrale

```cpp
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Employe {
    int id;
    std::string prenom;
    std::string nom;
    std::string departement;
    double salaire;
    std::string email;
};

class RHSysteme {
public:
    void ajouter(Employe e) {
        const std::size_t index = employes.size();
        indexParId[e.id] = index;
        indexParDepartement[e.departement].push_back(index);
        employes.push_back(std::move(e));
    }

    std::optional<Employe> trouverParId(int id) const {
        auto it = indexParId.find(id);
        if (it == indexParId.end()) return std::nullopt;
        return employes[it->second];
    }

    std::vector<Employe>
    parDepartement(const std::string& dept) const {
        std::vector<Employe> resultat;

        auto it = indexParDepartement.find(dept);
        if (it == indexParDepartement.end()) return resultat;

        resultat.reserve(it->second.size());
        for (std::size_t index : it->second) {
            resultat.push_back(employes[index]);
        }
        return resultat;
    }

private:
    std::vector<Employe> employes;
    std::unordered_map<int, std::size_t> indexParId;
    std::unordered_map<std::string, std::vector<std::size_t>>
        indexParDepartement;
};
```

### Déroulé pas à pas

1. Refuse les optimisations avant mesure. Fais d'abord établir une baseline.
2. `trouverParId` parcourt 50 000 éléments à chaque requête → candidat évident après profilage.
3. Demande : « Quelle donnée calculons-nous encore et encore ? » La relation ID → position.
4. Introduis un index `unordered_map<int,size_t>` construit au moment de l'ajout.
5. Pour département, même idée : mémoriser les positions des employés par département.
6. Explique le coût : mémoire supplémentaire + obligation de maintenir les index lors des insertions/suppressions/modifications.
7. Re-mesure. Ne valide pas un objectif `100x` théorique si les mesures de la machine ne le montrent pas : le critère réel est l'amélioration reproductible et justifiée.

---

## 19.6 Tests et couverture — squelette de correction

### Corrigé complet sur une fonction représentative

```cpp
// validateur.hpp
#pragma once

#include <string>

class Validateur {
public:
    struct Resultat {
        bool valide;
        std::string erreur;
        std::string valeurNormalisee;
    };

    Resultat validerMotDePasse(const std::string& mdp) const;
};
```

```cpp
// test_validateur.cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "validateur.hpp"

TEST_CASE("Un mot de passe valide est accepte") {
    Validateur v;

    auto mdp = GENERATE(
        std::string{"Abcdef1!"},
        std::string{"MotDePasse9#"},
        std::string{"XyZ12345?"}
    );

    INFO("Mot de passe teste : " << mdp);
    CHECK(v.validerMotDePasse(mdp).valide);
}

TEST_CASE("Les mots de passe invalides sont refuses") {
    Validateur v;

    auto [mdp, raison] = GENERATE(
        table<std::string, std::string>({
            {"Ab1!", "trop court"},
            {"abcdef1!", "pas de majuscule"},
            {"ABCDEF1!", "pas de minuscule"},
            {"Abcdefgh!", "pas de chiffre"},
            {"Abcdefg1", "pas de special"}
        })
    );

    INFO("Cas : " << raison);
    CHECK_FALSE(v.validerMotDePasse(mdp).valide);
}

TEST_CASE("La limite de huit caracteres est testee") {
    Validateur v;

    CHECK_FALSE(v.validerMotDePasse("Abc1!xy").valide); // 7
    CHECK(v.validerMotDePasse("Abc1!xyz").valide);      // 8
}
```

### Déroulé pas à pas

1. Demande les **partitions d'entrée** avant d'écrire un test : valide, trop court, sans majuscule, sans minuscule, sans chiffre, sans spécial, limites.
2. Écris un cas heureux simple.
3. Transforme les variantes répétitives en test paramétré.
4. Ajoute les cas limites autour de 8 caractères : 7, 8, 9.
5. Lance la couverture.
6. Si une branche n'est pas couverte, demande : « Est-ce une branche utile que nous avons oubliée ou du code mort ? »
7. Répète la méthode pour email, téléphone, date et IBAN.
8. Rappelle : la cible 90 % guide l'exploration ; elle ne remplace ni les assertions pertinentes ni les cas métier.

---

# 20. Jour 5 — correction du projet LibraryManager

Le projet de synthèse n'a pas une unique « correction ligne par ligne » pertinente : plusieurs architectures peuvent satisfaire le cahier des charges. La correction formateur doit donc porter sur **les responsabilités, les dépendances et les invariants**.

## Architecture cible minimale

```text
Domaine
├── Livre
├── Adherent
├── Emprunt
└── exceptions métier

Application
├── CatalogueService
├── EmpruntService
└── RechercheService

Ports
├── ILivreRepository
├── IAdherentRepository
└── IEmpruntRepository

Infrastructure
├── JsonLivreRepository
├── SQLiteLivreRepository   (bonus)
└── CLI
```

### Contrats conseillés

```cpp
struct Livre {
    std::string id;
    std::string titre;
    std::string auteur;
    bool disponible = true;
};

class ILivreRepository {
public:
    virtual ~ILivreRepository() = default;
    virtual void sauvegarder(const Livre& livre) = 0;
    virtual std::optional<Livre>
    trouverParId(const std::string& id) const = 0;
    virtual std::vector<Livre>
    tous() const = 0;
};

class LivreIndisponible : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class EmpruntService {
public:
    explicit EmpruntService(ILivreRepository& livres)
        : livres_(livres) {}

    void emprunter(const std::string& id) {
        auto livre = livres_.trouverParId(id);
        if (!livre) {
            throw std::runtime_error("Livre inexistant");
        }
        if (!livre->disponible) {
            throw LivreIndisponible("Livre deja emprunte");
        }

        livre->disponible = false;
        livres_.sauvegarder(*livre);
    }

private:
    ILivreRepository& livres_;
};
```

## Déroulé de correction du projet

**1. Demander une restitution d'architecture avant de regarder le code.** « Où est le métier ? Où est la persistance ? Où est l'interface utilisateur ? »

**2. Vérifier les invariants métier.** Un identifiant doit-il être unique ? Peut-on emprunter un livre déjà indisponible ? Que se passe-t-il au retour ?

**3. Vérifier l'ownership.** Qui possède les entités ? Y a-t-il des `new/delete` inutiles ?

**4. Vérifier les dépendances.** Si le service métier inclut directement `sqlite3.h`, le découplage est insuffisant.

**5. Vérifier la testabilité.** Peut-on tester `EmpruntService` avec un repository mémoire sans toucher au disque ?

**6. Vérifier les erreurs.** Différencier entrée invalide, règle métier et erreur technique.

**7. Vérifier les tests.** Au minimum : succès + cas limite + échec métier.

**8. Vérifier la concurrence seulement si elle est dans le périmètre.** Le test + modification de disponibilité doivent être atomiques vis-à-vis des autres emprunts.

**9. Finir par la qualité, pas l'inverse.** N'introduire optimisation, cache ou architecture hexagonale détaillée qu'après une version fonctionnelle et testée.

---

# 21. Trame d'animation recommandée

## Reprise mi-Jour 2 — environ 3 h 30

### 20–25 min — Reconnexion

- quiz Jour 1 ;
- ownership/RAII/smart pointers ;
- `std::move` ;
- mini-question diagnostic.

### 75–90 min — STL

Priorités : `vector`, map/unordered_map, itérateurs, algorithmes, lambdas, erase-remove.

Faire au moins deux micro-démos : `reserve/resize`, `map::operator[]`.

### 15 min — pause / exercice court

Pipeline `vector`.

### 60–75 min — patterns

Priorités : Factory, Adapter, Observer, Strategy. Builder et Decorator si le groupe suit bien. Singleton avec mise en garde.

### 30–45 min — correction / consolidation

Factory ou Observer est idéal pour relier les notions.

---

## Jour 3 matin

### 30 min — templates fonctions/classes

Rester concret. Faire compiler plusieurs types.

### 45 min — spécialisation / variadiques / SFINAE → Concepts

SFINAE comme historique/problème ; Concepts comme solution lisible.

### 45–60 min — TP templates

Corriger un exercice complet au tableau.

### 45 min — exceptions

Relier à RAII. Faire une propagation sur 3 niveaux de fonctions.

---

## Jour 3 après-midi

### 60 min — structure projet + CMake

Faire réellement construire un mini-projet à 2 targets.

### 30 min — dépendances / libs

Concret, sans entrer trop longtemps dans les package managers.

### 60 min — tests intégration + CI

Faire un pipeline minimal : configure → build → test.

### 30 min — synthèse

Demander aux stagiaires d'expliquer la chaîne depuis `.cpp` jusqu'au test CI.

---

## Jour 4 matin

### 60 min — lambdas + `auto` + structured bindings

Faire beaucoup de prédiction de sortie.

### 30–45 min — optional / variant / `if constexpr`

Insister sur l'expression des états dans les types.

### 60 min — sécurité

Buffer overflow, use-after-free, overflow entier, validation. Relier aux outils ASan/UBSan.

---

## Jour 4 après-midi

### 75 min — threading + data races + mutex

Faire un compteur cassé, puis le corriger.

### 45 min — condition variables / async / futures

Préférer une file producteur-consommateur à plusieurs slides abstraites.

### 45 min — profiling / optimisation

Mesurer avant/après.

### 30 min — TP correction

File de travaux ou mini thread pool selon le niveau.

---

## Jour 5

### Matin — architecture + implémentation du projet

Ne pas surcharger en nouvelles notions. Circuler, faire justifier les choix.

### Début après-midi — tests + code review croisée

Donner une grille : ownership, const-correctness, erreurs, dépendances, tests, lisibilité.

### Fin d'après-midi — soutenance / consolidation

Demander pour chaque groupe :

1. un choix de conception qu'il maintient ;
2. un bug rencontré et sa méthode de résolution ;
3. une amélioration qu'il ferait avec une journée de plus.

---

# 22. Questions « pièges utiles » à poser régulièrement

1. `reserve(100)` crée-t-il 100 éléments ? **Non.**
2. `map[key]` est-il une lecture pure ? **Non si la clé n'existe pas.**
3. `unordered_map` est-il toujours O(1) ? **Non, en moyenne.**
4. `end()` est-il le dernier élément ? **Non, après le dernier.**
5. `std::remove` efface-t-il des éléments du `vector` ? **Non.**
6. `std::move` déplace-t-il réellement un objet ? **Pas seul.**
7. `shared_ptr` est-il « plus sûr » que `unique_ptr` ? **Pas comme règle générale ; il exprime une sémantique différente.**
8. `auto` rend-il le type dynamique ? **Non.**
9. Une lambda `[&]` est-elle dangereuse ? **Pas en soi ; elle l'est si la durée de vie des références capturées n'est pas maîtrisée.**
10. 100 % de couverture garantit-il zéro bug ? **Non.**
11. Un mutex autour de chaque méthode rend-il une API thread-safe pour toutes les séquences d'appels ? **Non.**
12. `atomic` remplace-t-il toujours un mutex ? **Non.**
13. `noexcept` veut-il dire « cette fonction ne devrait probablement pas lancer » ? **Non : c'est un contrat fort.**
14. Une exception doit-elle être catchée à chaque niveau ? **Non.**
15. CMake compile-t-il le C++ ? **Non.**
16. Un template est-il du polymorphisme runtime ? **Non, généralement compile-time.**
17. Un pattern est-il obligatoire dès qu'on reconnaît son nom ? **Non.**
18. Une complexité O(1) est-elle forcément plus rapide qu'O(n) pour un petit `n` ? **Non. Les constantes, allocations et caches comptent.**

---

# 23. Phrases de transition prêtes à l'emploi

### Mémoire → STL

> « Jusqu'ici nous nous sommes demandé comment un objet vit et qui le détruit. Maintenant nous allons nous demander comment stocker et traiter beaucoup d'objets sans réécrire les mêmes boucles et structures. »

### STL → patterns

> « La STL nous donne des briques génériques au niveau du code. Les patterns vont nous donner un vocabulaire similaire au niveau de la conception. »

### Patterns → templates

> « Avec les patterns, nous avons souvent choisi le comportement à l'exécution grâce au polymorphisme. Les templates vont nous montrer une autre forme de généricité, décidée principalement à la compilation. »

### Templates → exceptions

> « Les templates renforcent ce que le compilateur peut vérifier. Mais certaines erreurs dépendent nécessairement des données réelles : fichier absent, entrée incorrecte, règle métier. C'est là qu'intervient la stratégie de gestion d'erreurs. »

### Exceptions → CMake

> « Nous savons maintenant écrire des composants robustes. Il faut ensuite les organiser, les compiler ensemble, gérer leurs dépendances et automatiser leur validation. »

### Séquentiel → concurrence

> « Tant qu'un seul flux d'exécution modifie les données, l'ordre est prévisible. Dès que plusieurs threads partagent de la mémoire, la question devient : quelles opérations doivent rester cohérentes ensemble ? »

### Jour 4 → projet final

> « Vous avez maintenant beaucoup d'outils. Le Jour 5 n'est pas un concours pour en utiliser le maximum : il faut choisir le minimum d'outils qui rendent la solution correcte, claire, testable et maintenable. »

---

# 24. Grille d'observation formateur

Pendant les TP, ne regarde pas seulement si « ça marche ». Observe si les stagiaires :

- choisissent une structure de données adaptée ;
- savent expliquer l'ownership ;
- utilisent `const` correctement ;
- évitent `new/delete` sans raison ;
- vérifient les retours et erreurs ;
- découpent les responsabilités ;
- savent lire une erreur de compilation ;
- compilent/testent par petites étapes ;
- savent justifier un choix de performance avec une mesure ;
- distinguent problème de syntaxe, problème de logique et problème d'architecture.

Un stagiaire qui arrive à une solution imparfaite mais sait diagnostiquer et expliquer ses choix progresse souvent mieux qu'un stagiaire qui recopie immédiatement une solution parfaite.

---

# 25. Repères de profondeur

## À maîtriser à la fin de la formation

- RAII / ownership / smart pointers ;
- `vector`, map/unordered_map, algorithmes et itérateurs ;
- lambdas ;
- polymorphisme et principaux patterns ;
- templates de base et Concepts ;
- exceptions / `noexcept` / robustesse ;
- CMake par targets ;
- tests unitaires et d'intégration ;
- data races, mutex, futures ;
- démarche de profiling ;
- architecture simple et testable.

## À savoir reconnaître sans exiger la maîtrise

- SFINAE avancé ;
- CRTP ;
- allocateurs custom ;
- lock-free ;
- memory ordering avancé ;
- coroutines avancées ;
- Event Sourcing / CQRS ;
- PGO / LTO détaillés.

---

# 26. Conclusion formateur

Le meilleur fil rouge à maintenir du milieu du Jour 2 au Jour 5 est :

> **Qui possède la donnée ? Quelle abstraction exprime le mieux l'intention ? Quelle partie peut varier ? Que peut vérifier le compilateur ? Que doit-on vérifier à l'exécution ? Et comment prouve-t-on que le comportement reste correct ?**

Si les stagiaires savent répondre à ces questions sur leur propre code en fin de semaine, ils auront acquis beaucoup plus qu'une liste de syntaxes C++.
