# Corrigés commentés — TP du jour 3

Ce ZIP corrige les **TP 9, 10, 11 et 12 du support cpp_formation_jour3.md** fourni.
Les exercices additionnels numérotés 1.1/2.1/3.1 dans le recueil séparé et les TP bonus des annexes ne font pas partie de ce pack.
Les solutions sont pédagogiques et exécutables : les commentaires dans les sources expliquent les décisions et les pièges.

## Correspondance

| TP | Exercice | Fichier / dossier |
|---|---|---|
| 9 A | Tableau dynamique, règle des cinq | TP09_templates/tableau_dynamique.hpp + tp09a_tableau.cpp |
| 9 B | contient, transformer, reduire | TP09_templates/tp09b_algorithmes.cpp |
| 9 C | Spécialisations de Serialiseur | TP09_templates/tp09c_serialiseur.cpp |
| 9 D | Concept Persistable | TP09_templates/tp09d_persistable.cpp |
| 10 A | Calculatrice, exceptions, garantie forte | TP10_exceptions/tp10a_calculatrice.cpp |
| 10 B | Configuration, 15 scénarios de test | TP10_exceptions/fichier_config.hpp + tp10b_fichier_config.cpp |
| 10 C | Quatre stratégies, 10 000 appels chacune | TP10_exceptions/tp10c_strategies.cpp |
| 11 A/B/C | Livre, Auteur, CRUD, JSON, spdlog, CMake, Doxygen, installation, CPack | TP11_12_bibliotheque/ + CMakeLists.txt |
| 12 A | PostgreSQL Docker, 10 scénarios d'intégration | compose.yaml + tests/test_integration.cpp |
| 12 B/C/D | CI Debug/Release, couverture 85 %, analyses, paquet Debian | .github/workflows/ci.yml |

## Démarrage (Linux / macOS)

Prérequis : CMake >= 3.24, Make, compilateur C++20. **GCC 13+ / libstdc++ récent** conseillé pour compiler aussi `std::expected` du TP10C. C++23 est activé seulement pour cette cible.
Au premier lancement, une connexion Internet permet à CMake de télécharger nlohmann_json et spdlog. Les dépendances ne sont pas recopiées dans l'archive.

Depuis le dossier contenant ce README :

```bash
cmake --preset debug
cmake --build --preset debug --parallel 2
ctest --preset debug
```

Les programmes TP9/TP10 sont dans `build/debug/`. L'application est dans `build/debug/TP11_12_bibliotheque/bibliotheque_app`.
Chaque exécutable contient un `main()` : **ne pas compiler tous les .cpp ensemble**.
CTest crée un répertoire de travail par programme pour isoler les fichiers produits.
Les vérifications utilisent `CHECK` (commun/test.hpp), qui reste actif en Release, contrairement à `assert` avec NDEBUG.

```bash
# Un seul exercice
cmake --build build/debug --target tp10b
ctest --test-dir build/debug -R '^tp10b$' --output-on-failure
# Mesures TP10C : utiliser Release, répéter et observer la variabilité.
cmake --preset release
cmake --build --preset release --parallel 2
./build/release/tp10c
```

Si votre bibliothèque standard n'offre pas `std::expected`, CMake annonce explicitement que TP10C est omis ; son code reste fourni. Sur Mac, utiliser un GCC récent installé séparément si Apple Clang/libc++ ne le prend pas en charge.
Sous Windows, hors presets Unix : `cmake -S . -B build/win`, `cmake --build build/win --config Debug`, `ctest --test-dir build/win -C Debug --output-on-failure`.

## TP12 : vraie base de test

Installer Docker avec Compose, et les headers/librairies PostgreSQL (`libpq-dev` sous Debian/Ubuntu, `libpq` via votre gestionnaire sous macOS).

```bash
docker compose up -d --wait
export TEST_DATABASE_URL='host=127.0.0.1 port=55432 dbname=bibliotheque_test user=formation password=formation_test connect_timeout=3'
cmake -S . -B build/integration -DCMAKE_BUILD_TYPE=Debug -DBUILD_INTEGRATION_TESTS=ON
cmake --build build/integration --parallel 2
ctest --test-dir build/integration --output-on-failure
# Arrêter la base de TP après les tests :
docker compose down
```

Choix explicite de correction : TP11 reste un catalogue en mémoire persistant en JSON ; TP12 ajoute un adaptateur PostgreSQL d'instantanés JSON (texte SQL paramétré), pour tester la chaîne Catalogue → sérialisation → réseau → BDD → désérialisation. Il ne s'agit pas d'un mapping relationnel livre/auteur.
Chaque session utilise une table temporaire, supprimée automatiquement à sa fermeture. Cette isolation est volontaire pour les tests ; une base de production utiliserait des tables durables et des migrations.
Le scénario 10 termine uniquement la session ouverte par son propre test. Le compte de la base Docker de TP a les droits requis.

## Qualité, documentation et packaging

```bash
# ASan + UBSan : pas besoin de PostgreSQL pour les tests de base.
cmake -S . -B build/asan -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
cmake --build build/asan --parallel 2
ctest --test-dir build/asan --output-on-failure

# Couverture avec GCC : gcovr doit utiliser le gcov associé à ce GCC.
cmake -S . -B build/coverage -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build/coverage --parallel 2
ctest --test-dir build/coverage --output-on-failure
python3 -m gcovr --root . --filter 'TP11_12_bibliotheque/src/(auteur|livre|catalogue)\.cpp' --print-summary --fail-under-line 85

# Doxygen doit être installé pour cette option.
cmake -S . -B build/docs -DBUILD_DOCS=ON
cmake --build build/docs --target docs

cmake --build build/release --parallel 2
cmake --install build/release --prefix "$PWD/installation"
(cd build/release && cpack -G TGZ)
# Sur Debian/Ubuntu : dpkg-dev requis pour la détection des dépendances.
(cd build/release && cpack -G DEB)
```

La couverture CI porte sur les lignes des sources métier `auteur.cpp`, `livre.cpp`, `catalogue.cpp`, `catalogue_bdd.cpp`. Elle ne représente pas la couverture de tout le cours. Un seuil automatisé n'est pas une preuve de qualité absolue.

## Activer la CI dans votre dépôt

Créer/forker votre dépôt vous-même, puis placer **le contenu de ce dossier à la racine**, y compris `.github/` (dossier parfois masqué par l'explorateur).
Le workflow s'exécute au push/PR sur `main`. Aucun dépôt n'a été créé ou modifié pour préparer ce ZIP.
Badge à adapter au propriétaire et au dépôt :

```markdown
[![CI Jour 3](https://github.com/VOTRE_COMPTE/VOTRE_DEPOT/actions/workflows/ci.yml/badge.svg)](https://github.com/VOTRE_COMPTE/VOTRE_DEPOT/actions/workflows/ci.yml)
```

La CI contient des contrôles bloquants cppcheck et clang-tidy, une mesure gcovr et un upload des rapports. Le paquet DEB est publié comme artefact uniquement après réussite des deux builds et push sur main. Aucun serveur n'est déployé.

## Repères pour la correction orale

- **TP9A** : distinguer allocation et construction ; montrer la copie, puis le move et l'objet source vide ; finir par le nettoyage si une copie lève.
- **TP9B** : `const auto&` évite les copies ; le type de retour de transformer peut changer ; la réduction d'un conteneur vide rend init.
- **TP9C** : spécialisation totale vs partielle ; parser et échapper correctement le JSON ; distinguer type valide et valeur représentable.
- **TP9D** : un concept vérifie la syntaxe disponible à la compilation, pas le bon fonctionnement métier.
- **TP10A/B** : montrer l'état avant l'appel, déclencher une erreur, constater que l'état reste identique. Pointer la ligne de commit.
- **TP10C** : séparer absence normale, erreur attendue et panne exceptionnelle ; les micro-mesures ne justifient pas un classement universel.
- **TP11** : le header expose le contrat, le .cpp cache les dépendances ; PUBLIC/PRIVATE décrivent les besoins de compilation et de liaison.
- **TP12** : un test d'intégration traverse plusieurs composants réels ; une CI décrit des commandes, elle n'est validée à distance qu'après un vrai run.

Références techniques : https://json.nlohmann.me/integration/cmake/ ; https://github.com/gabime/spdlog ; https://docs.github.com/en/actions/tutorials/store-and-share-data ; https://www.postgresql.org/docs/16/libpq.html
Voir VALIDATION.md pour les vérifications réellement effectuées lors de la préparation.
