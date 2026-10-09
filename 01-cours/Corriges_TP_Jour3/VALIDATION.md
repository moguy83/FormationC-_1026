# Vérifications effectuées

Environnement de préparation : Linux, GCC 13.3, CMake (installé via pip), nlohmann_json 3.11.3 et spdlog 1.14.1.

| Vérification | Résultat |
|---|---|
| Configuration et compilation Debug | Réussies, toutes les cibles locales dont std::expected |
| CTest Debug | 8/8 exécutables réussis |
| Configuration et compilation Release | Réussies |
| CTest Release | 8/8 exécutables réussis ; CHECK reste actif |
| FichierConfig | 15 scénarios exécutés dans tp10b |
| Tableau dynamique | Tests de copies qui lèvent, état conservé, compteur d'objets vivants revenu à zéro |
| ASan + UBSan sur TP09A | Exécution réussie avec détection des fuites désactivée dans cet environnement |
| gcovr, sources auteur/livre/catalogue | 95,1 % des lignes (58/61), 100 % des fonctions, 50,9 % des branches |
| cmake --install dans un dossier temporaire | Réussi |
| CPack TGZ | Création réussie |
| Syntaxe YAML Compose et GitHub Actions | Analysée sans erreur |

Le test ASan initial a rencontré une limitation de LeakSanitizer liée à l'accès à /proc dans l'environnement de préparation. Une seconde exécution avec `ASAN_OPTIONS=detect_leaks=0` a validé les contrôles d'adresses et UBSan ; elle ne constitue pas une vérification LeakSanitizer des fuites. Le compteur de durées de vie du TP09A complète les tests fonctionnels.

## Non exécuté ici

- Compilation/liaison de l'adaptateur libpq et exécution des 10 scénarios PostgreSQL : Docker, PostgreSQL et libpq ne sont pas disponibles dans cet environnement. Les commandes et le service Docker sont fournis.
- Workflow GitHub Actions réel, cppcheck, clang-tidy, Doxygen et création du paquet DEB : nécessitent les outils/services externes. Leur configuration est fournie ; aucune réussite distante n'est revendiquée.
- Couverture incluant catalogue_bdd.cpp : sera mesurée par le job CI avec PostgreSQL. Les 95,1 % ci-dessus concernent uniquement les trois sources métier locales.

Les 8 tests CTest sont des exécutables regroupant plusieurs vérifications, pas huit assertions uniques. Aucun binaire, dossier build ou dépendance téléchargée n'est inclus dans le ZIP.
