# 📘 Exercices C++ — Jour 1 : Fondamentaux du Langage
**Formation C++ · 5 jours · Niveau débutant–intermédiaire**

---

## 📋 Sommaire

- [Objectifs du jour](#objectifs)
- [⚡ Niveau 1 — Échauffement](#niveau-1) *(~30 min)*
  - [1.1 Types et variables](#11-types-et-variables)
  - [1.2 Structures de contrôle](#12-structures-de-contrôle)
  - [1.3 Fonctions simples](#13-fonctions-simples)
  - [1.4 Tableaux et std::array](#14-tableaux-et-stdarray)
  - [1.5 std::string — manipulation de chaînes](#15-stdstring)
- [🔥 Niveau 2 — Pratique](#niveau-2) *(~60 min)*
  - [2.1 Fonctions avancées et surcharge](#21-fonctions-avancées)
  - [2.2 Structures et énumérations](#22-structures-et-énumérations)
  - [2.3 Fichiers et I/O](#23-fichiers-et-io)
  - [2.4 Classes — encapsulation](#24-classes--encapsulation)
  - [2.5 Héritage simple](#25-héritage-simple)
- [🚀 Niveau 3 — Défi](#niveau-3) *(~90 min)*
  - [3.1 Polymorphisme et classes abstraites](#31-polymorphisme)
  - [3.2 Surcharge d'opérateurs](#32-surcharge-dopérateurs)
  - [3.3 Système de tests unitaires](#33-tests-unitaires)
- [🏆 Mini-projet du Jour 1](#mini-projet)
- [💡 Indices et solutions partielles](#indices)
- [✅ Auto-évaluation](#auto-évaluation)

---

## 🎯 Objectifs du jour {#objectifs}

À l'issue de ces exercices, vous serez capable de :
- Déclarer et utiliser les types fondamentaux C++ avec `auto` et `constexpr`
- Écrire des fonctions avec surcharge et valeurs par défaut
- Manipuler des tableaux (`std::array`) et des chaînes (`std::string`)
- Concevoir des classes avec encapsulation, constructeurs et destructeurs
- Mettre en place un héritage simple avec polymorphisme
- Écrire des tests unitaires de base avec Catch2 ou GoogleTest

---

## ⚡ Niveau 1 — Échauffement {#niveau-1}

> Objectif : valider la compréhension de la syntaxe de base. Chaque exercice doit tenir en **moins de 20 lignes**.

---

### 1.1 Types et variables {#11-types-et-variables}

**Exercice 1.1.a — Déclarer et afficher**

Déclarez les variables suivantes en utilisant les types C++ appropriés, puis affichez-les formatées avec `std::cout` :

| Variable | Valeur | Type suggéré |
|---|---|---|
| Nom de famille | "Dupont" | `std::string` |
| Âge | 28 | `int` |
| Taille en mètres | 1.75 | `double` |
| Actif | vrai | `bool` |
| Initiale | 'J' | `char` |
| Salaire mensuel | 3200.50 | `double` |

Affichage attendu :
```
Nom : Dupont | Age : 28 | Taille : 1.75 m | Actif : oui | Initiale : J
```

---

**Exercice 1.1.b — auto et constexpr**

Réécrivez le code suivant en remplaçant tous les types explicites par `auto` là où c'est pertinent, et en utilisant `constexpr` pour les constantes :

```cpp
// À réécrire
int ANNEE_MAX = 2100;
double PI = 3.14159265;
std::string MESSAGE = "Bonjour";
int x = 10;
double y = x * PI;
std::vector<int>::iterator it = monVecteur.begin();
```

**Question** : pour quelles variables `auto` est-il **déconseillé** et pourquoi ?

---

**Exercice 1.1.c — Conversions et casting**

Écrivez un programme qui :
1. Lit deux entiers depuis `cin`
2. Calcule leur quotient **en virgule flottante** (pas entier)
3. Affiche le résultat avec 2 décimales de précision
4. Affiche également le résultat de la division entière et le reste

```
Entrez deux entiers : 7 3
Division : 2.33
Division entière : 2
Reste (modulo) : 1
```

---

### 1.2 Structures de contrôle {#12-structures-de-contrôle}

**Exercice 1.2.a — FizzBuzz étendu**

Écrivez le programme FizzBuzz classique de 1 à 100, puis ajoutez :
- Multiples de 7 → "Bazz"
- Multiples de 3 et 7 → "FizzBazz"
- Multiples de 5 et 7 → "BuzzBazz"
- Multiples de 3, 5 et 7 → "FizzBuzzBazz"

Comptez et affichez combien de fois chaque mot a été affiché.

---

**Exercice 1.2.b — Switch sur enum class**

Définissez un `enum class Saison { Printemps, Ete, Automne, Hiver }`.

Écrivez une fonction `decrireSaison(Saison s)` qui, avec un `switch`, retourne une description de la saison. Testez avec les 4 valeurs et gérez le cas `default`.

---

**Exercice 1.2.c — Deviner le nombre**

Jeu "Deviner le nombre" :
1. Générez un nombre aléatoire entre 1 et 100 avec `rand() % 100 + 1`
2. Demandez à l'utilisateur de deviner, indiquez "trop grand" / "trop petit"
3. Comptez le nombre d'essais
4. Affichez un message selon le nombre d'essais (≤5 : excellent, ≤10 : bien, >10 : insuffisant)

---

### 1.3 Fonctions simples {#13-fonctions-simples}

**Exercice 1.3.a — Surcharge de fonctions**

Implémentez les 4 surcharges de `afficherInfo()` :
```cpp
void afficherInfo(int n);               // "Entier : 42"
void afficherInfo(double d);            // "Réel : 3.14"
void afficherInfo(const std::string& s); // "Chaîne : hello (5 car.)"
void afficherInfo(bool b);              // "Booléen : vrai/faux"
```

Testez avec au moins 2 valeurs par type.

---

**Exercice 1.3.b — Passage par référence**

Écrivez les fonctions suivantes (toutes sans `return`) :
```cpp
void echanger(int& a, int& b);
void mettreEnMajuscules(std::string& s);
void normaliser(double& x, double min, double max);
// normaliser : met x dans [0,1] proportionnellement à [min,max]
```

Vérifiez que les valeurs originales sont bien modifiées.

---

**Exercice 1.3.c — Récursivité**

Implémentez ces 3 fonctions récursives :
```cpp
long long factorielle(int n);   // factorielle de n
int fibonacci(int n);           // n-ième terme de Fibonacci
int sommeChiffres(int n);       // somme des chiffres de n
// Ex: sommeChiffres(1234) = 1+2+3+4 = 10
```

Pour chaque fonction, indiquez la complexité temporelle et l'alternative itérative.

---

### 1.4 Tableaux et std::array {#14-tableaux-et-stdarray}

**Exercice 1.4.a — Statistiques**

Déclarez `std::array<double, 10> notes = {12.5, 15.0, 8.0, 18.5, 11.0, 14.0, 9.5, 16.0, 13.0, 7.5}`.

Calculez et affichez :
- La moyenne
- Le minimum et le maximum
- La médiane (après tri)
- L'écart-type (formule : `sqrt(Σ(xi - moyenne)² / n)`)
- Le nombre de notes ≥ 10

---

**Exercice 1.4.b — Matrice**

Déclarez et remplissez une matrice 4×4 avec les valeurs `i*4 + j + 1` (de 1 à 16).

Fonctions à implémenter :
```cpp
void afficherMatrice(int m[][4], int n);
int sommeColonne(int m[][4], int col, int n);
int sommeLigne(int m[][4], int lig, int n);
bool estSymetrique(int m[][4], int n);  // m[i][j] == m[j][i]
```

---

**Exercice 1.4.c — Tri à bulles**

Implémentez le tri à bulles sur un `std::array<int, 8>` :
```cpp
void triBulles(std::array<int, 8>& arr);
```

Affichez le tableau **avant** et **après** chaque passe du tri pour visualiser l'algorithme. Comptez le nombre de comparaisons et d'échanges.

---

### 1.5 std::string — manipulation de chaînes {#15-stdstring}

**Exercice 1.5.a — Analyse de texte**

Écrivez un programme qui lit une phrase depuis `cin` et affiche :
- Le nombre de mots (séparés par des espaces)
- Le nombre de voyelles (a, e, i, o, u, y — majuscules et minuscules)
- Le nombre de caractères sans espaces
- La phrase inversée
- Si la phrase est un palindrome (ignorer casse et espaces)

---

**Exercice 1.5.b — Manipulation de chaînes**

Implémentez ces fonctions **sans utiliser** les méthodes `.find()` ou `.replace()` de la STL :
```cpp
std::string remplacerTout(std::string s, char ancien, char nouveau);
std::string supprEspaces(const std::string& s);  // trim left+right
int compterOccurrences(const std::string& s, const std::string& motif);
std::vector<std::string> decouper(const std::string& s, char delimiteur);
```

---

**Exercice 1.5.c — Validation**

Écrivez des fonctions de validation :
```cpp
bool estNombreEntier(const std::string& s);
bool estAdresseIPv4(const std::string& s);  // format "X.X.X.X"
bool estPalindrome(const std::string& s);
std::string capitaliser(const std::string& s);  // "bonjour monde" → "Bonjour Monde"
```

---

## 🔥 Niveau 2 — Pratique {#niveau-2}

> Objectif : combiner plusieurs concepts. Chaque exercice demande une **réflexion de conception** en plus de l'implémentation.

---

### 2.1 Fonctions avancées {#21-fonctions-avancées}

**Exercice 2.1.a — Calculatrice RPN**

Implémentez une calculatrice en **Notation Polonaise Inversée** (RPN) :
- Entrée : `"3 4 + 2 * 7 -"` → Résultat : `((3+4)*2)-7 = 7`
- Utiliser une `std::stack<double>` pour les opérations
- Supporter : `+`, `-`, `*`, `/`, `%`, `^` (puissance)
- Gérer les erreurs : division par zéro, expression invalide, pile vide

```cpp
double evaluerRPN(const std::string& expression);
// Lancer une exception std::invalid_argument si invalide
```

---

**Exercice 2.1.b — Fonctions avec paramètres par défaut et surcharge**

Concevez une API de formatage :
```cpp
// Format un nombre avec options
std::string formater(double val,
                     int precision = 2,
                     bool signe = false,
                     char separateurMilliers = '\0');
// Ex: formater(1234567.891, 2, true, ' ') → "+1 234 567.89"

// Surcharges
std::string formater(int val, int base = 10);
// base 10: "42", base 16: "0x2A", base 2: "0b101010"

std::string formater(bool val, bool texte = true);
// texte=true: "vrai"/"faux", texte=false: "1"/"0"
```

---

### 2.2 Structures et énumérations {#22-structures-et-énumérations}

**Exercice 2.2.a — Gestion de contacts**

Définissez :
```cpp
enum class TypeTel { Mobile, Fixe, Travail };

struct Telephone {
    TypeTel type;
    std::string numero;
    bool principal;
};

struct Contact {
    std::string prenom, nom;
    std::string email;
    std::vector<Telephone> telephones;
    std::string adresse;
};
```

Implémentez :
- `Contact creerContact(...)` — avec validation email basique
- `void afficherContact(const Contact& c)` — affichage formaté
- `std::string contactToCSV(const Contact& c)` — sérialisation
- `Contact contactFromCSV(const std::string& ligne)` — désérialisation
- `bool rechercherContact(const std::vector<Contact>&, const std::string& terme)` — recherche partielle nom/prénom

---

**Exercice 2.2.b — Système de notes scolaires**

```cpp
enum class Mention { Insuffisant, Passable, Assez_Bien, Bien, Tres_Bien, Excellent };

struct Note {
    std::string matiere;
    double valeur;  // 0–20
    double coefficient;
};

struct Etudiant {
    std::string nom;
    std::string classe;
    std::vector<Note> notes;
};
```

Implémentez :
- `double moyennePonderee(const Etudiant& e)` — moyenne pondérée par coefficient
- `Mention getMention(double moyenne)` — conversion
- `void afficherBulletin(const Etudiant& e)` — bulletin formaté avec tableau
- `std::string classement(const std::vector<Etudiant>& classe)` — tri et classement

---

### 2.3 Fichiers et I/O {#23-fichiers-et-io}

**Exercice 2.3.a — Gestionnaire de journal (log)**

Implémentez un système de journalisation simple :
```cpp
enum class NiveauLog { DEBUG, INFO, WARN, ERROR, FATAL };

void logMessage(NiveauLog niveau,
                const std::string& source,
                const std::string& message,
                const std::string& fichierLog = "app.log");

// Lire et filtrer le fichier de log
std::vector<std::string> lireLog(const std::string& fichier,
                                  NiveauLog niveauMin = NiveauLog::DEBUG);
```

Format de log : `[2024-01-15 14:32:05] [INFO] [main] Démarrage de l'application`

---

**Exercice 2.3.b — Lecteur de fichier CSV**

Implémentez un lecteur CSV robuste :
```cpp
struct LigneCSV {
    std::vector<std::string> champs;
    int numeroLigne;
};

class LecteurCSV {
public:
    LecteurCSV(const std::string& fichier, char separateur = ',');
    bool suivant();
    LigneCSV getLigne() const;
    std::vector<std::string> getEntetes() const;
    int getNombreLignes() const;
};
```

Tester avec un fichier CSV contenant des champs avec virgules entre guillemets.

---

### 2.4 Classes — Encapsulation {#24-classes--encapsulation}

**Exercice 2.4.a — Classe `CompteBancaire`**

Implémentez une classe complète :

```cpp
class CompteBancaire {
public:
    CompteBancaire(std::string titulaire, double soldeInitial = 0.0);

    // Opérations
    bool deposer(double montant);
    bool retirer(double montant);   // false si solde insuffisant
    bool virement(CompteBancaire& destination, double montant);

    // Historique
    struct Transaction {
        std::string type;
        double montant;
        double soldeApres;
        std::string date;
    };

    // Accesseurs
    double getSolde() const;
    std::string getNumero() const;    // auto-généré : FR76-XXXX
    std::vector<Transaction> getHistorique() const;

    // Affichage
    void afficherReleve(int nbDernieres = 10) const;

private:
    std::string titulaire;
    double solde;
    std::string numero;
    std::vector<Transaction> historique;
    static int compteur;  // pour générer les numéros
};
```

**Contraintes** : le solde ne peut jamais être négatif. Chaque opération est enregistrée dans l'historique avec un timestamp.

---

**Exercice 2.4.b — Classe `Vecteur2D`**

```cpp
class Vecteur2D {
    double x, y;
public:
    Vecteur2D(double x = 0, double y = 0);

    // Opérations mathématiques
    Vecteur2D operator+(const Vecteur2D&) const;
    Vecteur2D operator-(const Vecteur2D&) const;
    Vecteur2D operator*(double scalaire) const;
    double operator*(const Vecteur2D& autre) const;  // produit scalaire
    double norme() const;
    Vecteur2D normaliser() const;
    double angle() const;  // angle avec l'axe X en degrés
    double angleavec(const Vecteur2D& autre) const;

    // Comparaison
    bool operator==(const Vecteur2D&) const;

    // Affichage
    friend std::ostream& operator<<(std::ostream&, const Vecteur2D&);
};
```

---

### 2.5 Héritage simple {#25-héritage-simple}

**Exercice 2.5.a — Hiérarchie de formes**

Concevez la hiérarchie suivante et implémentez toutes les méthodes :

```
Forme (abstraite)
├── Cercle(rayon)
├── Rectangle(largeur, hauteur)
│   └── Carre(cote)          // hérite de Rectangle
├── Triangle(a, b, c)         // triangle quelconque (formule de Héron)
└── Ellipse(a, b)             // demi-axes
```

Chaque classe doit avoir :
```cpp
virtual double aire() const = 0;
virtual double perimetre() const = 0;
virtual std::string description() const;
virtual Forme* clone() const = 0;  // prototype pattern
```

Testez le polymorphisme avec un `std::vector<std::unique_ptr<Forme>>`.

---

**Exercice 2.5.b — Système de véhicules**

```
Vehicule (marque, annee, vitesseMax)
├── VehiculeElectrique(autonomieKm, tempsRechargeH)
│   └── VoitureElectrique(nbPortes, typeBoite)
├── VehiculeThermique(cylindree, carburant)
│   ├── Voiture(nbPortes, typeBoite)
│   └── Moto(typeMoto)
└── VehiculeHybride (hérite des deux)
```

Implémentez `virtual std::string ficheComplete() const` dans chaque classe. Gérez le problème du diamant avec l'héritage virtuel pour `VehiculeHybride`.

---

## 🚀 Niveau 3 — Défi {#niveau-3}

> Objectif : exercices complexes nécessitant une conception soignée. Prévoir **60 à 90 minutes** par exercice.

---

### 3.1 Polymorphisme et classes abstraites {#31-polymorphisme}

**Exercice 3.1 — Moteur de rendu de documents**

Concevez un système de rendu de documents multi-format :

```cpp
// Interface de rendu
class IRendu {
public:
    virtual void debutDocument(const std::string& titre) = 0;
    virtual void finDocument() = 0;
    virtual void titre(const std::string& texte, int niveau) = 0;
    virtual void paragraphe(const std::string& texte) = 0;
    virtual void liste(const std::vector<std::string>& items) = 0;
    virtual void tableau(const std::vector<std::vector<std::string>>&) = 0;
    virtual std::string getRendu() const = 0;
    virtual ~IRendu() = default;
};

class RenduHTML : public IRendu { /* ... */ };
class RenduMarkdown : public IRendu { /* ... */ };
class RenduTexte : public IRendu { /* ... */ };  // texte brut

class Document {
    std::unique_ptr<IRendu> rendu;
public:
    Document(std::unique_ptr<IRendu> r) : rendu(std::move(r)) {}
    void ecrire(/* méthodes déléguées au rendu */);
    std::string compiler() const;
};
```

Testez en générant le même document dans les 3 formats.

---

### 3.2 Surcharge d'opérateurs {#32-surcharge-dopérateurs}

**Exercice 3.2 — Classe `Fraction`**

Implémentez une classe `Fraction` complète :

```cpp
class Fraction {
    long long numerateur;
    long long denominateur;  // toujours > 0, toujours réduite

    void reduire();  // diviser par le PGCD
    static long long pgcd(long long a, long long b);

public:
    Fraction(long long num = 0, long long den = 1);
    Fraction(double val);  // approximation avec précision 1e-6

    Fraction operator+(const Fraction&) const;
    Fraction operator-(const Fraction&) const;
    Fraction operator*(const Fraction&) const;
    Fraction operator/(const Fraction&) const;
    Fraction operator-() const;  // négation

    bool operator==(const Fraction&) const;
    bool operator<(const Fraction&) const;
    // Les autres (<, >, <=, >=, !=) à déduire avec <=>

    explicit operator double() const;
    friend std::ostream& operator<<(std::ostream&, const Fraction&);
    friend std::istream& operator>>(std::istream&, Fraction&);
};
```

Testez : `Fraction(1,3) + Fraction(1,6) == Fraction(1,2)` doit être `true`.

---

### 3.3 Tests unitaires {#33-tests-unitaires}

**Exercice 3.3 — Tester la classe `CompteBancaire`**

Écrivez au moins 20 tests pour la classe `CompteBancaire` de l'exercice 2.4.a.

Couvrez obligatoirement :
- [ ] Création avec solde initial valide
- [ ] Création avec solde négatif (doit échouer)
- [ ] Dépôt d'un montant positif
- [ ] Dépôt d'un montant nul ou négatif (doit échouer)
- [ ] Retrait inférieur au solde
- [ ] Retrait égal au solde (doit passer)
- [ ] Retrait supérieur au solde (doit échouer)
- [ ] Virement entre deux comptes valide
- [ ] Virement avec solde insuffisant
- [ ] Virement vers le même compte
- [ ] L'historique contient la transaction après dépôt
- [ ] Le solde après 10 opérations consécutives est correct
- [ ] Le numéro de compte est unique pour chaque instance

```cpp
// Template Catch2
TEST_CASE("CompteBancaire — dépôt", "[compte]") {
    CompteBancaire c("Alice", 1000.0);

    SECTION("Dépôt valide augmente le solde") { /* ... */ }
    SECTION("Dépôt nul est refusé") { /* ... */ }
    SECTION("Dépôt négatif est refusé") { /* ... */ }
}
```

---

## 🏆 Mini-projet du Jour 1 : Carnet d'adresses {#mini-projet}

**Durée estimée** : 2h–3h

**Description** : Concevez et implémentez un carnet d'adresses en ligne de commande.

### Fonctionnalités obligatoires

1. **Ajouter un contact** : nom, prénom, email, téléphone, adresse (optionnel)
2. **Rechercher** : par nom (partiel, insensible à la casse), par email, par téléphone
3. **Modifier** : changer n'importe quel champ d'un contact existant
4. **Supprimer** : par index ou par nom exact
5. **Afficher tous** : liste triée alphabétiquement par nom
6. **Persister** : sauvegarde/chargement automatique en fichier CSV
7. **Valider les données** : format email, format téléphone français

### Contraintes techniques

```cpp
// Architecture attendue
struct Contact { /* cf. exercice 2.2.a */ };

class CarnetAdresses {
    std::vector<Contact> contacts;
    std::string fichier;

public:
    void charger(const std::string& path);
    void sauvegarder() const;
    void ajouter(Contact c);
    bool supprimer(const std::string& nom);
    std::vector<Contact> rechercher(const std::string& terme) const;
    void trier(const std::string& champ = "nom");
    void afficherTous() const;
    int getTaille() const;
};

class MenuPrincipal {
    CarnetAdresses& carnet;
    void afficherMenu() const;
    int lireChoix() const;
public:
    void run();
};
```

### Critères de réussite

- [ ] Compilation sans warning avec `-Wall -Wextra`
- [ ] Aucun `new`/`delete` — utiliser uniquement `std::vector` et `std::string`
- [ ] Les données sont persistées entre deux exécutions
- [ ] La recherche fonctionne en insensible à la casse
- [ ] Gestion des erreurs : fichier absent, CSV malformé
- [ ] Au moins 10 tests unitaires sur la classe `CarnetAdresses`

---

## 💡 Indices et solutions partielles {#indices}

<details>
<summary><strong>1.1.c — Division avec précision</strong></summary>

```cpp
// Forcer la division flottante
int a = 7, b = 3;
double resultat = static_cast<double>(a) / b;
// Ou : double resultat = (double)a / b;  // style C (déconseillé)
// Ou : double resultat = 1.0 * a / b;

std::cout << std::fixed << std::setprecision(2) << resultat;
```
</details>

<details>
<summary><strong>1.3.c — Fibonacci itératif (hint)</strong></summary>

```cpp
// Récursif : O(2^n) — très lent !
int fibRecursif(int n) {
    if (n <= 1) return n;
    return fibRecursif(n-1) + fibRecursif(n-2);
}

// Itératif : O(n) — préférable
int fibIteratif(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}
```
</details>

<details>
<summary><strong>2.1.a — Calculatrice RPN (structure)</strong></summary>

```cpp
double evaluerRPN(const std::string& expr) {
    std::stack<double> pile;
    std::istringstream iss(expr);
    std::string token;

    while (iss >> token) {
        if (std::isdigit(token[0]) || 
            (token[0] == '-' && token.size() > 1)) {
            pile.push(std::stod(token));
        } else {
            if (pile.size() < 2)
                throw std::invalid_argument("Expression invalide");
            double b = pile.top(); pile.pop();
            double a = pile.top(); pile.pop();
            // Appliquer l'opérateur...
        }
    }
    if (pile.size() != 1) throw std::invalid_argument("Expression invalide");
    return pile.top();
}
```
</details>

<details>
<summary><strong>3.2 — PGCD de Fraction (algorithme d'Euclide)</strong></summary>

```cpp
long long Fraction::pgcd(long long a, long long b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

void Fraction::reduire() {
    if (denominateur < 0) {
        numerateur   = -numerateur;
        denominateur = -denominateur;
    }
    long long g = pgcd(std::abs(numerateur), denominateur);
    numerateur   /= g;
    denominateur /= g;
}
```
</details>

---

## ✅ Auto-évaluation {#auto-évaluation}

Cochez les cases correspondant aux compétences acquises :

### Syntaxe et types
- [ ] Je sais utiliser `auto`, `const`, `constexpr` correctement
- [ ] Je comprends la différence entre passage par valeur, référence et référence constante
- [ ] Je sais lire et écrire depuis `cin`/`cout` avec formatage (`iomanip`)
- [ ] Je sais lire et écrire un fichier texte ligne par ligne

### POO — Bases
- [ ] Je sais déclarer une classe avec attributs privés et méthodes publiques
- [ ] Je sais écrire un constructeur avec liste d'initialisation
- [ ] Je sais écrire un destructeur
- [ ] Je comprends `public`, `private`, `protected`
- [ ] Je sais écrire des getters `const` et des setters avec validation

### Héritage et polymorphisme
- [ ] Je sais créer une classe dérivée qui hérite d'une classe de base
- [ ] Je sais déclarer et implémenter des méthodes `virtual` et `override`
- [ ] Je sais créer une classe abstraite avec des méthodes virtuelles pures
- [ ] Je comprends pourquoi le destructeur d'une classe de base doit être `virtual`

### Tests
- [ ] J'ai écrit au moins 10 tests avec Catch2 ou GoogleTest
- [ ] Mes tests couvrent les cas normaux ET les cas d'erreur

### Score indicatif
- **14-16 cases** : Excellent — prêt pour le Jour 2
- **10-13 cases** : Bien — revoir les points non cochés ce soir
- **< 10 cases** : Reprendre les exercices Niveau 1 avant de continuer
