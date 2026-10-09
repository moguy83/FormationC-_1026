# Corrigés commentés — TP du jour 4

Ce pack correspond aux **TP 13 à 16 du support cpp_formation_jour4.md** fourni, sur le même périmètre que le ZIP du jour 3. Les exercices du recueil séparé et les bonus des annexes (ThreadPool, fuzzing, lock-free…) ne sont pas inclus.
Tous les points importants sont expliqués directement dans le code : durée de vie, captures, bornes, garanties, synchronisation, précision des mesures et limites des exemples.

| TP | Correction | Source |
|---|---|---|
| 13 A | Recherche modernisée, optional, const et références | TP13_moderne/tp13a_recherche.cpp |
| 13 B | 1000 ventes CSV, dates, Ranges, sommes et top 5 | TP13_moderne/tp13b_ranges_csv.cpp |
| 13 C | EventBus, lambdas, any, abonnements pendant publication | TP13_moderne/tp13c_event_bus.cpp |
| 13 D | Parseur avec priorités, signes, parenthèses, puissance et sqrt | TP13_moderne/tp13d_expressions.cpp |
| 14 A | Audit mémoire, bornes, droits et SQL | TP14_securite/tp14a_audit.cpp |
| 14 B | Email, URL, téléphone, date, HTML et JSON | TP14_securite/tp14b_input_validator.cpp |
| 15 A | 2 producteurs, 3 consommateurs, mutex, condition_variable, arrêt atomique | TP15_concurrence/tp15a_producteur_consommateur.cpp |
| 15 B | 10 millions d'entiers, séquentiel et quatre async | TP15_concurrence/tp15b_somme_async.cpp |
| 15 C | Race volontaire et correction atomique | TP15_concurrence/tp15c_data_race.cpp |
| 16 A | Recherche linéaire/binaire, 100 répétitions, statistiques et CSV | TP16_performance/tp16a_benchmark.cpp |
| 16 B | Logs JSONL, alertes >100ms, P50/P95/P99 | TP16_performance/tp16b_monitoring.cpp |
| 16 C | Code avant/après, mesures et procédure Callgrind | TP16_performance/tp16c_profiling.cpp + GUIDE_PROFILING.md |

## Démarrer

Prérequis : CMake >=3.24, Make et un compilateur/bibliothèque standard C++20 (GCC 13+ conseillé pour reproduire la validation). TP13A utilise C++17. JSON utilise nlohmann_json 3.11.3, téléchargé à la première configuration si aucun paquet compatible n'est installé ; connexion Internet nécessaire dans ce cas.

Depuis ce dossier :

```bash
cmake --preset debug
cmake --build --preset debug --parallel 2
ctest --preset debug
```

Un `.cpp` = un exécutable avec son propre `main()` ; ne pas les lier tous ensemble.
Les tests utilisent `CHECK`, actif également en Release. CTest place les fichiers de sortie dans un répertoire propre à chaque exécutable : `build/debug/travail/tp16a/benchmark.csv`, `build/debug/travail/tp16b/requetes.jsonl`, etc.

```bash
# Compiler et tester uniquement le calculateur.
cmake --build build/debug --target tp13d
ctest --test-dir build/debug -R '^tp13d$' --output-on-failure
./build/debug/tp13d 'sqrt(16) + 2^3^2'

# TP13B : référence fixe pour obtenir le résultat attendu livré.
./build/debug/tp13b donnees/ventes_1000.csv 2026-10-09
# Puis essayer votre propre date de référence : le jeu ne se met pas à jour automatiquement.

# Les performances se mesurent en Release, sans sanitizer.
cmake --preset release
cmake --build --preset release --parallel 2
ctest --preset release
./build/release/tp15b
./build/release/tp16a benchmark.csv
./build/release/tp16b requetes.jsonl
./build/release/tp16b --analyser requetes.jsonl
./build/release/tp16c comparaison
```

Windows : utiliser un générateur compatible (`cmake -S . -B build/win`, `cmake --build build/win --config Debug`, `ctest --test-dir build/win -C Debug`). Les commandes des presets sont prévues pour Linux/macOS avec Make.

## Données TP13B

- `donnees/ventes_1000.csv` : exactement 1000 ventes, montants positifs, huit catégories, dates réparties sur 66 jours.
- `donnees/ventes_exemple.csv` : dix lignes pour lire facilement le format.
- `donnees/resultat_attendu.txt` : top 5 vérifié avec la référence 2026-10-09.
- Format : `date;categorie;montant`, montants avec point et deux décimales ; guillemets doubles possibles, champs multilignes non supportés.
- Fenêtre : jour de référence et 29 jours précédents inclus. Les dates futures sont exclues.
- Ranges C++20 fournit filtres, parcours et tri ; l'agrégation est faite dans une map via `ranges::for_each`, car il n'existe pas de `views::group_by` standard en C++20.

## Les choix de correction à expliquer

**TP13C** est mono-thread et synchrone. Un callback défaillant interrompt la publication ; un abonnement ajouté par un callback prend effet lors de la prochaine publication. L’instantané copie des shared_ptr de callbacks : les closures mutable conservent leur état entre publications, sans invalider la boucle quand un abonnement est ajouté.

**TP13D** suit les priorités mathématiques : `-2^2=-4`, `2^3^2=512`, `2^-2=0.25`. `^` est notre opérateur de puissance ; ce n'est pas l'opérateur C++ natif (XOR). Le parseur est borné en longueur et profondeur. Il n'est pas réentrant ni prévu pour être partagé entre threads.

**TP14A** corrige aussi l'analyse du support : `to_string(int)` ne permet pas directement une injection de caractères SQL. Il faut néanmoins une requête paramétrée, des identifiants distincts et une vérification d'appartenance. L'adaptateur fourni enregistre les paramètres pour montrer le contrat ; aucune base externe ni vraie suppression n'est nécessaire.

**TP14B** applique des politiques syntaxiques explicites et limitées (ASCII, noms DNS, téléphones internationaux). Il ne prétend pas couvrir toutes les RFC ni vérifier qu'une adresse existe. L'échappement HTML encode du texte ; ce n'est pas un nettoyage autorisant certaines balises, ni un encodeur JavaScript/CSS. Le JSON est validé syntaxiquement, puis un petit schéma métier `nom/email` est montré.

**TP15A** ferme la file après la fin des producteurs ; les consommateurs la vident avant de sortir. Les exceptions des workers sont transmises au thread principal. Aucune boucle de polling ; le mutex protège la file et les transitions liées à la condition_variable. La file n'est pas bornée : ajouter une seconde condition « non pleine » pour introduire la contre-pression dans un prolongement.

**TP15B/TP16** affichent des mesures réelles sans promettre un gain universel. Le démarrage des threads est inclus dans le coût async. Le benchmark de recherche exclut le tri et compare 50 cibles présentes et 50 absentes. Pour les recherches très rapides, le chronométrage lui-même introduit un coût significatif.

## ThreadSanitizer (Linux GCC/Clang)

```bash
cmake -S . -B build/tsan -DCMAKE_BUILD_TYPE=Debug -DENABLE_TSAN=ON -DBUILD_RACE_DEMO=ON
cmake --build build/tsan --target tp15c_race tp15c tp15a --parallel 2
# Version volontairement fautive : rapport de race et code de sortie non nul attendus.
./build/tsan/tp15c_race
# Versions corrigées : pas de race attendue.
./build/tsan/tp15c
./build/tsan/tp15a
```

Un échec de démarrage de TSan dû à l'environnement n'est PAS une détection de race. La version fautive est exclue de CTest et n'est jamais compilée par défaut. Un résultat numérique correct occasionnel ne rend pas cette version correcte.

## AddressSanitizer + UBSan

```bash
cmake -S . -B build/asan -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build/asan --parallel 2
ctest --test-dir build/asan --output-on-failure
```

`-g` conserve les informations de débogage, `-Wall -Wextra -Wpedantic` active les avertissements, `-fsanitize=thread` détecte certaines races lors de l'exécution, `-fsanitize=address,undefined` instrumente mémoire et comportements indéfinis, `-fno-omit-frame-pointer` facilite les piles d'appels. ASan et TSan s'utilisent dans des compilations séparées. Aucune exécution instrumentée ne prouve à elle seule l'absence de tous les défauts.

Voir **VALIDATION.md** pour les contrôles réellement exécutés et **GUIDE_PROFILING.md** pour Callgrind.
