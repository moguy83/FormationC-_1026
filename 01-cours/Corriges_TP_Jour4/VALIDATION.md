# Validation du pack jour 4

Environnement : Linux, GCC 13.3, nlohmann_json 3.11.3. Les 12 cibles compilent avec `-Wall -Wextra -Wpedantic`, sans avertissement observé après correction et mise en forme.

| Contrôle | Résultat |
|---|---|
| CMake + compilation Debug | Réussis |
| CTest Debug | 12/12 exécutables réussis |
| CMake + compilation Release | Réussis |
| CTest Release | 12/12 exécutables réussis |
| TP13A | Compilé en C++17 ; les autres cibles en C++20 |
| CSV de ventes | 1000 lignes de données ; top 5 comparé au résultat de référence |
| EventBus | Abonnement pendant publication, erreurs et état mutable des closures vérifiés |
| Parseur | Priorités, parenthèses, signes, puissance, sqrt, erreurs et profondeur maximale vérifiés |
| Producteur-consommateur | 2000 éléments traités, somme des carrés égale au total produit, file drainée |
| Somme async | Résultat attendu 50 000 005 000 000 obtenu dans les deux versions |
| ThreadSanitizer, version volontairement fautive | Race détectée, sortie 66 ; extrait fourni |
| ThreadSanitizer, compteur atomique corrigé | Exécution réussie sans rapport de race |
| ThreadSanitizer, producteur-consommateur | Exécution réussie sans rapport de race |
| AddressSanitizer + UBSan, parseur TP13D | Réussi avec `ASAN_OPTIONS=detect_leaks=0` |
| Benchmark | Six séries (3 tailles × 2 recherches), 100 répétitions par série, 50 résultats présents vérifiés |
| Monitoring | 24 logs JSONL produits et analysés ; percentiles testés sur une série connue |
| Optimisation TP16C | Égalité complète des résultats avant/après vérifiée, durées natives mesurées |

Un premier lancement Release du benchmark a été empêché par un bit d'exécution absent sur le binaire généré dans l'environnement. Après restauration de ce bit, les 12 tests Release ont réussi. Les binaires ne sont pas inclus : ils seront reconstruits chez vous.

LeakSanitizer n'a pas pu fonctionner dans cet environnement à cause de l'accès à `/proc`. ASan et UBSan ont été relancés sans détection des fuites et ont réussi ; cela ne constitue pas une validation des fuites mémoire par LSan.

## Limites et vérifications à reproduire

Valgrind/Callgrind n'est pas installé ici : le guide de profiling est fourni, mais aucun top 3 Callgrind mesuré n'est revendiqué. Les configurations Windows/macOS ne sont pas testées. L'audit SQL utilise un adaptateur enregistrant les paramètres, sans connexion à une base réelle.

Les tests ne sont pas une preuve d'absence de tout défaut. Les exemples de mesures dans `exemples_sorties/` proviennent de cette exécution et varieront selon la machine et sa charge. Les programmes peuvent régénérer leurs rapports.

Les sources sont formatées pour la lecture et commentées en français. Aucune dépendance téléchargée ni aucun binaire n'est livré dans le ZIP.
