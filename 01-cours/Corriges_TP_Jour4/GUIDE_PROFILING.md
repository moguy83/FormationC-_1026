# TP16C — Correction et démonstration Callgrind

Le programme traite 20 000 mots appartenant à 1 000 catégories. Il expose trois étapes nommées : `normaliser`, `compterAvant`/`compterApres`, `produireRapport`. La version initiale recompte toute la collection pour chaque mot distinct. La version corrigée construit un index de fréquences en un passage, puis remet le résultat dans une map ordonnée. Une assertion vérifie l'égalité complète des résultats.

## 1. Préparer les symboles et conserver l'optimisation

Sous Linux, installer Valgrind avec le gestionnaire de paquets de votre distribution, puis :

```bash
cmake -S . -B build/profil -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/profil --target tp16c --parallel 2
./build/profil/tp16c comparaison
```

À dire : « RelWithDebInfo nous donne un programme optimisé tout en conservant les symboles qui permettent de relier les coûts aux fonctions. Un build Debug peut modifier fortement le classement. »

## 2. Mesurer le profil initial

```bash
valgrind --tool=callgrind --callgrind-out-file=callgrind.avant ./build/profil/tp16c avant
callgrind_annotate --inclusive=yes --threshold=100 callgrind.avant > profil_avant.txt
```

Lire le tableau trié du rapport. Relever les trois fonctions dominantes et leur coût, en précisant si l'on regarde les fonctions de l'application ou également celles de la bibliothèque standard. Pour explorer visuellement : `kcachegrind callgrind.avant` si KCachegrind est installé.

À dire : « Par défaut, Callgrind compte principalement des instructions instrumentées. Le coût inclusif comprend les fonctions appelées ; les coûts inclusifs ne s'additionnent pas tous entre eux. Une fonction STL en tête peut révéler un algorithme appelant trop souvent cette fonction. »

Le hotspot attendu est le comptage répétitif, mais le top 3 exact doit provenir de votre rapport : aucune liste mesurée n'est inventée dans ce corrigé.

## 3. Lire l'optimisation dans le code

Dans `compterAvant`, pointer `std::count` à l'intérieur de la boucle des mots distincts : environ U*N comparaisons. Dans `compterApres`, pointer `++index[mot]` : un passage, avec accès en temps moyen constant à la table de hachage. La map finale conserve le même ordre de sortie.

Le compromis : une structure de hachage consomme de la mémoire ; le temps constant est une moyenne, pas une garantie pour toute entrée. Ici les données et les chaînes sont bornées et déterministes.

## 4. Profiler après, puis mesurer nativement

```bash
valgrind --tool=callgrind --callgrind-out-file=callgrind.apres ./build/profil/tp16c apres
callgrind_annotate --inclusive=yes --threshold=100 callgrind.apres > profil_apres.txt
./build/profil/tp16c comparaison
./build/profil/tp16c comparaison
./build/profil/tp16c comparaison
```

À dire : « Nous comparons d'abord les coûts instrumentés entre eux. Puis nous mesurons la durée réelle hors Valgrind, qui ralentit le programme. Le gain affiché est temps avant divisé par temps après. Nous vérifions surtout que les résultats métier restent identiques. »

Sur macOS, utiliser Instruments/Time Profiler si Valgrind n'est pas disponible pour votre architecture. Les commandes Callgrind ci-dessus ciblent Linux ; ne pas supposer une compatibilité universelle.

## Références techniques

- https://valgrind.org/docs/manual/cl-manual.html
- https://clang.llvm.org/docs/ThreadSanitizer.html
- https://cheatsheetseries.owasp.org/cheatsheets/SQL_Injection_Prevention_Cheat_Sheet.html
- https://cheatsheetseries.owasp.org/cheatsheets/Cross_Site_Scripting_Prevention_Cheat_Sheet.html
- https://json.nlohmann.me/
