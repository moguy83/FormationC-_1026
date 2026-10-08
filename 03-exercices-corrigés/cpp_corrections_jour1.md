# ✅ Corrections — Jour 1 : Fondamentaux du Langage
**Formation C++ · 5 jours · Corrigé des exercices**

> Ces corrections proposent une solution possible pour chaque exercice. D'autres implémentations correctes existent — l'important est de respecter les bonnes pratiques (RAII, `const`, pas de fuite mémoire, etc.).

---

## ⚡ Niveau 1 — Échauffement

### 1.1 Types et variables

**1.1.a — Déclarer et afficher**

```cpp
#include <iostream>
#include <string>

int main() {
    std::string nom = "Dupont";
    int age = 28;
    double taille = 1.75;
    bool actif = true;
    char initiale = 'J';
    double salaire = 3200.50;

    std::cout << "Nom : " << nom
              << " | Age : " << age
              << " | Taille : " << taille << " m"
              << " | Actif : " << (actif ? "oui" : "non")
              << " | Initiale : " << initiale << "\n";
}
```

**1.1.b — auto et constexpr**

```cpp
constexpr int ANNEE_MAX = 2100;
constexpr double PI = 3.14159265;
auto MESSAGE = std::string("Bonjour");
auto x = 10;
auto y = x * PI;
auto it = monVecteur.begin();
```

Réponse à la question : `auto` est déconseillé quand le type déduit n'est pas évident à la lecture (ex. retour d'une expression complexe, ou quand on veut explicitement un type différent du type déduit, comme `double` au lieu d'un `int` littéral). Il est aussi déconseillé quand la lisibilité en pâtit pour un lecteur qui doit connaître le type exact (API publique).

**1.1.c — Conversions et casting**

```cpp
#include <iostream>
#include <iomanip>

int main() {
    int a, b;
    std::cout << "Entrez deux entiers : ";
    std::cin >> a >> b;

    double division = static_cast<double>(a) / b;
    int divEntiere = a / b;
    int reste = a % b;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Division : " << division << "\n";
    std::cout << "Division entière : " << divEntiere << "\n";
    std::cout << "Reste (modulo) : " << reste << "\n";
}
```

---

### 1.2 Structures de contrôle

**1.2.a — FizzBuzz étendu**

```cpp
#include <iostream>
#include <map>

int main() {
    std::map<std::string, int> compteurs;
    for (int i = 1; i <= 100; ++i) {
        std::string mot;
        if (i % 3 == 0) mot += "Fizz";
        if (i % 5 == 0) mot += "Buzz";
        if (i % 7 == 0) mot += "Bazz";

        if (!mot.empty()) {
            std::cout << mot << "\n";
            compteurs[mot]++;
        } else {
            std::cout << i << "\n";
        }
    }
    for (const auto& [mot, n] : compteurs)
        std::cout << mot << " : " << n << " fois\n";
}
```

**1.2.b — Switch sur enum class**

```cpp
enum class Saison { Printemps, Ete, Automne, Hiver };

std::string decrireSaison(Saison s) {
    switch (s) {
        case Saison::Printemps: return "Renouveau et fleurs";
        case Saison::Ete:       return "Chaleur et vacances";
        case Saison::Automne:   return "Feuilles et récoltes";
        case Saison::Hiver:     return "Froid et neige";
        default:                return "Saison inconnue";
    }
}
```

**1.2.c — Deviner le nombre**

```cpp
#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int cible = std::rand() % 100 + 1;
    int essai, tentatives = 0;

    do {
        std::cout << "Devinez (1-100) : ";
        std::cin >> essai;
        ++tentatives;
        if (essai > cible) std::cout << "Trop grand\n";
        else if (essai < cible) std::cout << "Trop petit\n";
    } while (essai != cible);

    if (tentatives <= 5) std::cout << "Excellent !\n";
    else if (tentatives <= 10) std::cout << "Bien.\n";
    else std::cout << "Insuffisant.\n";
}
```

---

### 1.3 Fonctions simples

**1.3.a — Surcharge de fonctions**

```cpp
void afficherInfo(int n)                 { std::cout << "Entier : " << n << "\n"; }
void afficherInfo(double d)              { std::cout << "Réel : " << d << "\n"; }
void afficherInfo(const std::string& s)  { std::cout << "Chaîne : " << s << " (" << s.size() << " car.)\n"; }
void afficherInfo(bool b)                { std::cout << "Booléen : " << (b ? "vrai" : "faux") << "\n"; }
```

**1.3.b — Passage par référence**

```cpp
void echanger(int& a, int& b) {
    int temp = a; a = b; b = temp;
}

void mettreEnMajuscules(std::string& s) {
    for (char& c : s) c = static_cast<char>(std::toupper(c));
}

void normaliser(double& x, double min, double max) {
    x = (x - min) / (max - min);
}
```

**1.3.c — Récursivité**

```cpp
long long factorielle(int n) {           // O(n), itératif préférable
    return n <= 1 ? 1 : n * factorielle(n - 1);
}

int fibonacci(int n) {                   // O(2^n) récursif, O(n) itératif
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int sommeChiffres(int n) {               // O(log n)
    return n == 0 ? 0 : (n % 10) + sommeChiffres(n / 10);
}
```

---

### 1.4 Tableaux et std::array

**1.4.a — Statistiques**

```cpp
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>

int main() {
    std::array<double, 10> notes = {12.5, 15.0, 8.0, 18.5, 11.0, 14.0, 9.5, 16.0, 13.0, 7.5};

    double somme = std::accumulate(notes.begin(), notes.end(), 0.0);
    double moyenne = somme / notes.size();

    auto [minIt, maxIt] = std::minmax_element(notes.begin(), notes.end());

    auto triees = notes;
    std::sort(triees.begin(), triees.end());
    double mediane = (triees[4] + triees[5]) / 2.0;

    double variance = 0.0;
    for (double n : notes) variance += (n - moyenne) * (n - moyenne);
    double ecartType = std::sqrt(variance / notes.size());

    int nbAuMoins10 = std::count_if(notes.begin(), notes.end(),
                                     [](double n) { return n >= 10; });

    std::cout << "Moyenne : " << moyenne << "\n";
    std::cout << "Min : " << *minIt << " | Max : " << *maxIt << "\n";
    std::cout << "Médiane : " << mediane << "\n";
    std::cout << "Écart-type : " << ecartType << "\n";
    std::cout << "Notes >= 10 : " << nbAuMoins10 << "\n";
}
```

**1.4.b — Matrice**

```cpp
void afficherMatrice(int m[][4], int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 4; ++j) std::cout << m[i][j] << " ";
        std::cout << "\n";
    }
}

int sommeColonne(int m[][4], int col, int n) {
    int s = 0;
    for (int i = 0; i < n; ++i) s += m[i][col];
    return s;
}

int sommeLigne(int m[][4], int lig, int n) {
    (void)n;
    int s = 0;
    for (int j = 0; j < 4; ++j) s += m[lig][j];
    return s;
}

bool estSymetrique(int m[][4], int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < 4; ++j)
            if (m[i][j] != m[j][i]) return false;
    return true;
}

int main() {
    int m[4][4];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = i * 4 + j + 1;
    afficherMatrice(m, 4);
}
```

**1.4.c — Tri à bulles**

```cpp
void afficherTab(const std::array<int, 8>& a) {
    for (int v : a) std::cout << v << " ";
    std::cout << "\n";
}

void triBulles(std::array<int, 8>& arr) {
    int comparaisons = 0, echanges = 0;
    for (size_t i = 0; i < arr.size() - 1; ++i) {
        bool permute = false;
        for (size_t j = 0; j < arr.size() - i - 1; ++j) {
            ++comparaisons;
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                ++echanges;
                permute = true;
            }
        }
        std::cout << "Passe " << i + 1 << " : ";
        afficherTab(arr);
        if (!permute) break;
    }
    std::cout << "Comparaisons : " << comparaisons << " | Échanges : " << echanges << "\n";
}
```

---

### 1.5 std::string — manipulation de chaînes

**1.5.a — Analyse de texte**

```cpp
#include <cctype>

int main() {
    std::string phrase;
    std::cout << "Entrez une phrase : ";
    std::getline(std::cin, phrase);

    int mots = 1, voyelles = 0, sansEspaces = 0;
    for (char c : phrase) {
        if (c == ' ') ++mots;
        else ++sansEspaces;
        char l = static_cast<char>(std::tolower(c));
        if (l == 'a' || l == 'e' || l == 'i' || l == 'o' || l == 'u' || l == 'y') ++voyelles;
    }

    std::string inversee(phrase.rbegin(), phrase.rend());

    std::string nettoyee;
    for (char c : phrase) if (std::isalpha(static_cast<unsigned char>(c))) nettoyee += static_cast<char>(std::tolower(c));
    std::string nettoyeeInv(nettoyee.rbegin(), nettoyee.rend());
    bool palindrome = (nettoyee == nettoyeeInv);

    std::cout << "Mots : " << mots << "\n";
    std::cout << "Voyelles : " << voyelles << "\n";
    std::cout << "Caractères sans espaces : " << sansEspaces << "\n";
    std::cout << "Inversée : " << inversee << "\n";
    std::cout << "Palindrome : " << (palindrome ? "oui" : "non") << "\n";
}
```

**1.5.b — Manipulation de chaînes (sans .find/.replace)**

```cpp
std::string remplacerTout(std::string s, char ancien, char nouveau) {
    for (char& c : s) if (c == ancien) c = nouveau;
    return s;
}

std::string supprEspaces(const std::string& s) {
    size_t debut = 0, fin = s.size();
    while (debut < fin && s[debut] == ' ') ++debut;
    while (fin > debut && s[fin - 1] == ' ') --fin;
    return s.substr(debut, fin - debut);
}

int compterOccurrences(const std::string& s, const std::string& motif) {
    if (motif.empty()) return 0;
    int compte = 0;
    for (size_t i = 0; i + motif.size() <= s.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < motif.size(); ++j)
            if (s[i + j] != motif[j]) { match = false; break; }
        if (match) ++compte;
    }
    return compte;
}

std::vector<std::string> decouper(const std::string& s, char delimiteur) {
    std::vector<std::string> parties;
    std::string courant;
    for (char c : s) {
        if (c == delimiteur) { parties.push_back(courant); courant.clear(); }
        else courant += c;
    }
    parties.push_back(courant);
    return parties;
}
```

**1.5.c — Validation**

```cpp
bool estNombreEntier(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (i == s.size()) return false;
    for (; i < s.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

bool estAdresseIPv4(const std::string& s) {
    auto parties = decouper(s, '.');
    if (parties.size() != 4) return false;
    for (const auto& p : parties) {
        if (!estNombreEntier(p) || p.empty() || p.size() > 3) return false;
        int val = std::stoi(p);
        if (val < 0 || val > 255) return false;
    }
    return true;
}

bool estPalindrome(const std::string& s) {
    std::string nettoyee;
    for (char c : s) if (std::isalnum(static_cast<unsigned char>(c)))
        nettoyee += static_cast<char>(std::tolower(c));
    return nettoyee == std::string(nettoyee.rbegin(), nettoyee.rend());
}

std::string capitaliser(const std::string& s) {
    std::string res = s;
    bool debutMot = true;
    for (char& c : res) {
        if (c == ' ') debutMot = true;
        else if (debutMot) { c = static_cast<char>(std::toupper(c)); debutMot = false; }
        else c = static_cast<char>(std::tolower(c));
    }
    return res;
}
```

---

## 🔥 Niveau 2 — Pratique

### 2.1 Fonctions avancées

**2.1.a — Calculatrice RPN**

```cpp
#include <stack>
#include <sstream>
#include <cmath>
#include <stdexcept>

double evaluerRPN(const std::string& expression) {
    std::stack<double> pile;
    std::istringstream iss(expression);
    std::string token;

    while (iss >> token) {
        if (token == "+" || token == "-" || token == "*" ||
            token == "/" || token == "%" || token == "^") {
            if (pile.size() < 2) throw std::invalid_argument("Expression invalide");
            double b = pile.top(); pile.pop();
            double a = pile.top(); pile.pop();
            double res = 0;
            if (token == "+") res = a + b;
            else if (token == "-") res = a - b;
            else if (token == "*") res = a * b;
            else if (token == "/") {
                if (b == 0) throw std::invalid_argument("Division par zéro");
                res = a / b;
            } else if (token == "%") res = std::fmod(a, b);
            else if (token == "^") res = std::pow(a, b);
            pile.push(res);
        } else {
            try { pile.push(std::stod(token)); }
            catch (...) { throw std::invalid_argument("Token invalide : " + token); }
        }
    }
    if (pile.size() != 1) throw std::invalid_argument("Expression invalide");
    return pile.top();
}
```

**2.1.b — API de formatage**

```cpp
#include <sstream>
#include <algorithm>

std::string formater(double val, int precision = 2, bool signe = false, char separateurMilliers = '\0') {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << std::abs(val);
    std::string s = oss.str();

    if (separateurMilliers != '\0') {
        auto pointPos = s.find('.');
        std::string entierPart = (pointPos == std::string::npos) ? s : s.substr(0, pointPos);
        std::string decPart = (pointPos == std::string::npos) ? "" : s.substr(pointPos);
        std::string avecSep;
        int compte = 0;
        for (auto it = entierPart.rbegin(); it != entierPart.rend(); ++it) {
            if (compte != 0 && compte % 3 == 0) avecSep += separateurMilliers;
            avecSep += *it;
            ++compte;
        }
        std::reverse(avecSep.begin(), avecSep.end());
        s = avecSep + decPart;
    }
    std::string prefixe = (val < 0) ? "-" : (signe ? "+" : "");
    return prefixe + s;
}

std::string formater(int val, int base = 10) {
    std::ostringstream oss;
    if (base == 16) oss << "0x" << std::hex << std::uppercase << val;
    else if (base == 2) {
        std::string bin;
        unsigned int v = static_cast<unsigned int>(val);
        if (v == 0) bin = "0";
        while (v > 0) { bin = char('0' + (v % 2)) + bin; v /= 2; }
        return "0b" + bin;
    } else oss << val;
    return oss.str();
}

std::string formater(bool val, bool texte = true) {
    if (texte) return val ? "vrai" : "faux";
    return val ? "1" : "0";
}
```

---

### 2.2 Structures et énumérations

**2.2.a — Gestion de contacts**

```cpp
enum class TypeTel { Mobile, Fixe, Travail };

struct Telephone { TypeTel type; std::string numero; bool principal; };

struct Contact {
    std::string prenom, nom, email, adresse;
    std::vector<Telephone> telephones;
};

bool emailValide(const std::string& email) {
    auto at = email.find('@');
    auto point = email.rfind('.');
    return at != std::string::npos && point != std::string::npos && point > at;
}

Contact creerContact(std::string prenom, std::string nom, std::string email) {
    if (!emailValide(email)) throw std::invalid_argument("Email invalide");
    Contact c;
    c.prenom = std::move(prenom);
    c.nom = std::move(nom);
    c.email = std::move(email);
    return c;
}

void afficherContact(const Contact& c) {
    std::cout << c.prenom << " " << c.nom << " <" << c.email << ">\n";
    for (const auto& t : c.telephones)
        std::cout << "  - " << t.numero << (t.principal ? " (principal)" : "") << "\n";
}

std::string contactToCSV(const Contact& c) {
    return c.prenom + ";" + c.nom + ";" + c.email + ";" + c.adresse;
}

Contact contactFromCSV(const std::string& ligne) {
    auto champs = decouper(ligne, ';');
    Contact c;
    if (champs.size() >= 4) {
        c.prenom = champs[0]; c.nom = champs[1];
        c.email = champs[2]; c.adresse = champs[3];
    }
    return c;
}

bool rechercherContact(const std::vector<Contact>& contacts, const std::string& terme) {
    return std::any_of(contacts.begin(), contacts.end(), [&](const Contact& c) {
        return c.nom.find(terme) != std::string::npos ||
               c.prenom.find(terme) != std::string::npos;
    });
}
```

**2.2.b — Système de notes scolaires**

```cpp
enum class Mention { Insuffisant, Passable, Assez_Bien, Bien, Tres_Bien, Excellent };

struct Note { std::string matiere; double valeur; double coefficient; };
struct Etudiant { std::string nom, classe; std::vector<Note> notes; };

double moyennePonderee(const Etudiant& e) {
    double sommeValeurs = 0, sommeCoefs = 0;
    for (const auto& n : e.notes) { sommeValeurs += n.valeur * n.coefficient; sommeCoefs += n.coefficient; }
    return sommeCoefs == 0 ? 0.0 : sommeValeurs / sommeCoefs;
}

Mention getMention(double moyenne) {
    if (moyenne < 8) return Mention::Insuffisant;
    if (moyenne < 10) return Mention::Passable;
    if (moyenne < 12) return Mention::Assez_Bien;
    if (moyenne < 14) return Mention::Bien;
    if (moyenne < 16) return Mention::Tres_Bien;
    return Mention::Excellent;
}

void afficherBulletin(const Etudiant& e) {
    std::cout << "Bulletin de " << e.nom << " (" << e.classe << ")\n";
    for (const auto& n : e.notes)
        std::cout << "  " << n.matiere << " : " << n.valeur << "/20 (coef " << n.coefficient << ")\n";
    std::cout << "Moyenne générale : " << moyennePonderee(e) << "/20\n";
}

std::string classement(const std::vector<Etudiant>& classe) {
    auto trie = classe;
    std::sort(trie.begin(), trie.end(), [](const Etudiant& a, const Etudiant& b) {
        return moyennePonderee(a) > moyennePonderee(b);
    });
    std::ostringstream oss;
    int rang = 1;
    for (const auto& e : trie) oss << rang++ << ". " << e.nom << " (" << moyennePonderee(e) << ")\n";
    return oss.str();
}
```

---

### 2.3 Fichiers et I/O

**2.3.a — Gestionnaire de journal**

```cpp
#include <fstream>
#include <chrono>
#include <ctime>

enum class NiveauLog { DEBUG, INFO, WARN, ERROR, FATAL };

std::string niveauToString(NiveauLog n) {
    switch (n) {
        case NiveauLog::DEBUG: return "DEBUG";
        case NiveauLog::INFO:  return "INFO";
        case NiveauLog::WARN:  return "WARN";
        case NiveauLog::ERROR: return "ERROR";
        case NiveauLog::FATAL: return "FATAL";
    }
    return "?";
}

std::string horodatage() {
    auto t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

void logMessage(NiveauLog niveau, const std::string& source,
                const std::string& message, const std::string& fichierLog = "app.log") {
    std::ofstream out(fichierLog, std::ios::app);
    out << "[" << horodatage() << "] [" << niveauToString(niveau) << "] ["
        << source << "] " << message << "\n";
}

std::vector<std::string> lireLog(const std::string& fichier, NiveauLog niveauMin = NiveauLog::DEBUG) {
    std::vector<std::string> lignesRetenues;
    std::ifstream in(fichier);
    std::string ligne;
    static const std::vector<std::string> ordre = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    size_t seuil = static_cast<size_t>(niveauMin);
    while (std::getline(in, ligne)) {
        for (size_t i = seuil; i < ordre.size(); ++i) {
            if (ligne.find("[" + ordre[i] + "]") != std::string::npos) {
                lignesRetenues.push_back(ligne);
                break;
            }
        }
    }
    return lignesRetenues;
}
```

**2.3.b — Lecteur de fichier CSV**

```cpp
struct LigneCSV { std::vector<std::string> champs; int numeroLigne; };

class LecteurCSV {
    std::ifstream fichier;
    char separateur;
    std::vector<std::string> entetes;
    int numeroLigne = 0;
    LigneCSV courante;

    std::vector<std::string> parserLigne(const std::string& ligne) const {
        std::vector<std::string> champs;
        std::string courant;
        bool dansGuillemets = false;
        for (size_t i = 0; i < ligne.size(); ++i) {
            char c = ligne[i];
            if (c == '"') dansGuillemets = !dansGuillemets;
            else if (c == separateur && !dansGuillemets) { champs.push_back(courant); courant.clear(); }
            else courant += c;
        }
        champs.push_back(courant);
        return champs;
    }

public:
    LecteurCSV(const std::string& nomFichier, char sep = ',')
        : fichier(nomFichier), separateur(sep) {
        std::string premiereLigne;
        if (std::getline(fichier, premiereLigne)) {
            entetes = parserLigne(premiereLigne);
            ++numeroLigne;
        }
    }

    bool suivant() {
        std::string ligne;
        if (!std::getline(fichier, ligne)) return false;
        ++numeroLigne;
        courante = { parserLigne(ligne), numeroLigne };
        return true;
    }

    LigneCSV getLigne() const { return courante; }
    std::vector<std::string> getEntetes() const { return entetes; }
    int getNombreLignes() const { return numeroLigne; }
};
```

---

### 2.4 Classes — Encapsulation

**2.4.a — CompteBancaire**

```cpp
class CompteBancaire {
public:
    struct Transaction { std::string type; double montant; double soldeApres; std::string date; };

    explicit CompteBancaire(std::string titulaire, double soldeInitial = 0.0)
        : titulaire(std::move(titulaire)), solde(soldeInitial) {
        if (soldeInitial < 0) throw std::invalid_argument("Solde initial négatif");
        numero = "FR76-" + std::to_string(1000 + compteur++);
    }

    bool deposer(double montant) {
        if (montant <= 0) return false;
        solde += montant;
        historique.push_back({"Dépôt", montant, solde, horodatage()});
        return true;
    }

    bool retirer(double montant) {
        if (montant <= 0 || montant > solde) return false;
        solde -= montant;
        historique.push_back({"Retrait", montant, solde, horodatage()});
        return true;
    }

    bool virement(CompteBancaire& destination, double montant) {
        if (&destination == this) return false;
        if (!retirer(montant)) return false;
        destination.deposer(montant);
        return true;
    }

    double getSolde() const { return solde; }
    std::string getNumero() const { return numero; }
    std::vector<Transaction> getHistorique() const { return historique; }

    void afficherReleve(int nbDernieres = 10) const {
        int debut = std::max(0, static_cast<int>(historique.size()) - nbDernieres);
        for (int i = debut; i < static_cast<int>(historique.size()); ++i) {
            const auto& t = historique[i];
            std::cout << t.date << " | " << t.type << " : " << t.montant
                      << " | solde : " << t.soldeApres << "\n";
        }
    }

private:
    std::string titulaire;
    double solde;
    std::string numero;
    std::vector<Transaction> historique;
    static inline int compteur = 1;
};
```

**2.4.b — Vecteur2D**

```cpp
#include <cmath>

class Vecteur2D {
    double x, y;
public:
    Vecteur2D(double x = 0, double y = 0) : x(x), y(y) {}

    Vecteur2D operator+(const Vecteur2D& o) const { return {x + o.x, y + o.y}; }
    Vecteur2D operator-(const Vecteur2D& o) const { return {x - o.x, y - o.y}; }
    Vecteur2D operator*(double s) const { return {x * s, y * s}; }
    double operator*(const Vecteur2D& o) const { return x * o.x + y * o.y; }

    double norme() const { return std::sqrt(x * x + y * y); }
    Vecteur2D normaliser() const { double n = norme(); return n == 0 ? Vecteur2D{} : Vecteur2D{x / n, y / n}; }
    double angle() const { return std::atan2(y, x) * 180.0 / M_PI; }
    double angleavec(const Vecteur2D& autre) const {
        double cosT = (*this * autre) / (norme() * autre.norme());
        return std::acos(std::clamp(cosT, -1.0, 1.0)) * 180.0 / M_PI;
    }

    bool operator==(const Vecteur2D& o) const { return x == o.x && y == o.y; }

    friend std::ostream& operator<<(std::ostream& os, const Vecteur2D& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};
```

---

### 2.5 Héritage simple

**2.5.a — Hiérarchie de formes**

```cpp
#include <memory>
#include <cmath>

class Forme {
public:
    virtual double aire() const = 0;
    virtual double perimetre() const = 0;
    virtual std::string description() const {
        return "Forme (aire=" + std::to_string(aire()) + ", périmètre=" + std::to_string(perimetre()) + ")";
    }
    virtual Forme* clone() const = 0;
    virtual ~Forme() = default;
};

class Cercle : public Forme {
    double rayon;
public:
    explicit Cercle(double r) : rayon(r) {}
    double aire() const override { return M_PI * rayon * rayon; }
    double perimetre() const override { return 2 * M_PI * rayon; }
    Forme* clone() const override { return new Cercle(*this); }
};

class Rectangle : public Forme {
protected:
    double largeur, hauteur;
public:
    Rectangle(double l, double h) : largeur(l), hauteur(h) {}
    double aire() const override { return largeur * hauteur; }
    double perimetre() const override { return 2 * (largeur + hauteur); }
    Forme* clone() const override { return new Rectangle(*this); }
};

class Carre : public Rectangle {
public:
    explicit Carre(double cote) : Rectangle(cote, cote) {}
    Forme* clone() const override { return new Carre(*this); }
};

class Triangle : public Forme {
    double a, b, c;
public:
    Triangle(double a, double b, double c) : a(a), b(b), c(c) {}
    double perimetre() const override { return a + b + c; }
    double aire() const override {
        double s = perimetre() / 2;
        return std::sqrt(s * (s - a) * (s - b) * (s - c));
    }
    Forme* clone() const override { return new Triangle(*this); }
};

class Ellipse : public Forme {
    double a, b;
public:
    Ellipse(double a, double b) : a(a), b(b) {}
    double aire() const override { return M_PI * a * b; }
    double perimetre() const override { return M_PI * (3 * (a + b) - std::sqrt((3 * a + b) * (a + 3 * b))); }
    Forme* clone() const override { return new Ellipse(*this); }
};

// Test polymorphisme
int main() {
    std::vector<std::unique_ptr<Forme>> formes;
    formes.push_back(std::make_unique<Cercle>(3.0));
    formes.push_back(std::make_unique<Carre>(4.0));
    formes.push_back(std::make_unique<Triangle>(3, 4, 5));
    for (const auto& f : formes) std::cout << f->description() << "\n";
}
```

**2.5.b — Système de véhicules**

```cpp
class Vehicule {
protected:
    std::string marque; int annee; double vitesseMax;
public:
    Vehicule(std::string m, int a, double v) : marque(std::move(m)), annee(a), vitesseMax(v) {}
    virtual std::string ficheComplete() const {
        return marque + " (" + std::to_string(annee) + "), vmax=" + std::to_string(vitesseMax);
    }
    virtual ~Vehicule() = default;
};

class VehiculeElectrique : virtual public Vehicule {
protected:
    double autonomieKm; double tempsRechargeH;
public:
    VehiculeElectrique(double autonomie, double temps) : Vehicule("", 0, 0), autonomieKm(autonomie), tempsRechargeH(temps) {}
    std::string ficheComplete() const override {
        return Vehicule::ficheComplete() + " | électrique, autonomie=" + std::to_string(autonomieKm) + "km";
    }
};

class VehiculeThermique : virtual public Vehicule {
protected:
    double cylindree; std::string carburant;
public:
    VehiculeThermique(double cyl, std::string carb) : Vehicule("", 0, 0), cylindree(cyl), carburant(std::move(carb)) {}
    std::string ficheComplete() const override {
        return Vehicule::ficheComplete() + " | thermique " + carburant;
    }
};

class Voiture : public VehiculeThermique {
    int nbPortes; std::string typeBoite;
public:
    Voiture(double cyl, std::string carb, int portes, std::string boite)
        : Vehicule("", 0, 0), VehiculeThermique(cyl, carb), nbPortes(portes), typeBoite(std::move(boite)) {}
    std::string ficheComplete() const override {
        return VehiculeThermique::ficheComplete() + " | " + std::to_string(nbPortes) + " portes";
    }
};

class VehiculeHybride : public VehiculeElectrique, public VehiculeThermique {
public:
    VehiculeHybride(double autonomie, double temps, double cyl, std::string carb)
        : Vehicule("Hybride", 2024, 180),
          VehiculeElectrique(autonomie, temps), VehiculeThermique(cyl, std::move(carb)) {}
    std::string ficheComplete() const override {
        return Vehicule::ficheComplete() + " | hybride (élec + thermique)";
    }
};
```
*(Note pédagogique : l'héritage `virtual` sur `Vehicule` évite le problème du diamant — `VehiculeHybride` ne possède qu'une seule sous-partie `Vehicule`.)*

---

## 🚀 Niveau 3 — Défi

### 3.1 Moteur de rendu de documents

```cpp
class IRendu {
public:
    virtual void debutDocument(const std::string& titre) = 0;
    virtual void finDocument() = 0;
    virtual void titre(const std::string& texte, int niveau) = 0;
    virtual void paragraphe(const std::string& texte) = 0;
    virtual void liste(const std::vector<std::string>& items) = 0;
    virtual void tableau(const std::vector<std::vector<std::string>>& t) = 0;
    virtual std::string getRendu() const = 0;
    virtual ~IRendu() = default;
};

class RenduMarkdown : public IRendu {
    std::string buf;
public:
    void debutDocument(const std::string& t) override { buf += "# " + t + "\n\n"; }
    void finDocument() override {}
    void titre(const std::string& texte, int niveau) override {
        buf += std::string(niveau, '#') + " " + texte + "\n\n";
    }
    void paragraphe(const std::string& texte) override { buf += texte + "\n\n"; }
    void liste(const std::vector<std::string>& items) override {
        for (auto& i : items) buf += "- " + i + "\n";
        buf += "\n";
    }
    void tableau(const std::vector<std::vector<std::string>>& t) override {
        for (auto& ligne : t) {
            for (auto& cell : ligne) buf += "| " + cell + " ";
            buf += "|\n";
        }
        buf += "\n";
    }
    std::string getRendu() const override { return buf; }
};

class RenduHTML : public IRendu {
    std::string buf;
public:
    void debutDocument(const std::string& t) override { buf += "<html><body><h1>" + t + "</h1>\n"; }
    void finDocument() override { buf += "</body></html>\n"; }
    void titre(const std::string& texte, int niveau) override {
        buf += "<h" + std::to_string(niveau) + ">" + texte + "</h" + std::to_string(niveau) + ">\n";
    }
    void paragraphe(const std::string& texte) override { buf += "<p>" + texte + "</p>\n"; }
    void liste(const std::vector<std::string>& items) override {
        buf += "<ul>\n";
        for (auto& i : items) buf += "<li>" + i + "</li>\n";
        buf += "</ul>\n";
    }
    void tableau(const std::vector<std::vector<std::string>>& t) override {
        buf += "<table>\n";
        for (auto& ligne : t) {
            buf += "<tr>";
            for (auto& cell : ligne) buf += "<td>" + cell + "</td>";
            buf += "</tr>\n";
        }
        buf += "</table>\n";
    }
    std::string getRendu() const override { return buf; }
};

class RenduTexte : public IRendu {
    std::string buf;
public:
    void debutDocument(const std::string& t) override { buf += t + "\n" + std::string(t.size(), '=') + "\n\n"; }
    void finDocument() override {}
    void titre(const std::string& texte, int) override { buf += texte + "\n"; }
    void paragraphe(const std::string& texte) override { buf += texte + "\n\n"; }
    void liste(const std::vector<std::string>& items) override {
        for (auto& i : items) buf += " * " + i + "\n";
    }
    void tableau(const std::vector<std::vector<std::string>>& t) override {
        for (auto& ligne : t) {
            for (auto& c : ligne) buf += c + "\t";
            buf += "\n";
        }
    }
    std::string getRendu() const override { return buf; }
};

class Document {
    std::unique_ptr<IRendu> rendu;
public:
    explicit Document(std::unique_ptr<IRendu> r) : rendu(std::move(r)) {}
    void ecrire(const std::string& titreDoc) {
        rendu->debutDocument(titreDoc);
        rendu->titre("Introduction", 2);
        rendu->paragraphe("Ceci est un paragraphe de démonstration.");
        rendu->liste({"Point 1", "Point 2", "Point 3"});
        rendu->tableau({{"Col1", "Col2"}, {"A", "B"}});
        rendu->finDocument();
    }
    std::string compiler() const { return rendu->getRendu(); }
};
```

### 3.2 Classe Fraction

```cpp
#include <cmath>

class Fraction {
    long long numerateur;
    long long denominateur;

    static long long pgcd(long long a, long long b) {
        a = std::abs(a); b = std::abs(b);
        while (b != 0) { long long t = b; b = a % b; a = t; }
        return a == 0 ? 1 : a;
    }

    void reduire() {
        if (denominateur < 0) { numerateur = -numerateur; denominateur = -denominateur; }
        long long g = pgcd(numerateur, denominateur);
        numerateur /= g; denominateur /= g;
    }

public:
    Fraction(long long num = 0, long long den = 1) : numerateur(num), denominateur(den) {
        if (den == 0) throw std::invalid_argument("Dénominateur nul");
        reduire();
    }

    Fraction(double val) {
        long long den = 1000000;
        numerateur = static_cast<long long>(std::llround(val * den));
        denominateur = den;
        reduire();
    }

    Fraction operator+(const Fraction& o) const {
        return Fraction(numerateur * o.denominateur + o.numerateur * denominateur, denominateur * o.denominateur);
    }
    Fraction operator-(const Fraction& o) const {
        return Fraction(numerateur * o.denominateur - o.numerateur * denominateur, denominateur * o.denominateur);
    }
    Fraction operator*(const Fraction& o) const {
        return Fraction(numerateur * o.numerateur, denominateur * o.denominateur);
    }
    Fraction operator/(const Fraction& o) const {
        return Fraction(numerateur * o.denominateur, denominateur * o.numerateur);
    }
    Fraction operator-() const { return Fraction(-numerateur, denominateur); }

    bool operator==(const Fraction& o) const {
        return numerateur == o.numerateur && denominateur == o.denominateur;
    }
    bool operator<(const Fraction& o) const {
        return numerateur * o.denominateur < o.numerateur * denominateur;
    }
    bool operator!=(const Fraction& o) const { return !(*this == o); }
    bool operator>(const Fraction& o) const { return o < *this; }
    bool operator<=(const Fraction& o) const { return !(o < *this); }
    bool operator>=(const Fraction& o) const { return !(*this < o); }

    explicit operator double() const { return static_cast<double>(numerateur) / denominateur; }

    friend std::ostream& operator<<(std::ostream& os, const Fraction& f) {
        return os << f.numerateur << "/" << f.denominateur;
    }
    friend std::istream& operator>>(std::istream& is, Fraction& f) {
        long long n, d;
        char slash;
        is >> n >> slash >> d;
        f = Fraction(n, d);
        return is;
    }
};

// Test : Fraction(1,3) + Fraction(1,6) == Fraction(1,2)  → true
```

### 3.3 Tests unitaires — CompteBancaire

```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CompteBancaire — création", "[compte]") {
    SECTION("Solde initial valide") {
        CompteBancaire c("Alice", 1000.0);
        REQUIRE(c.getSolde() == 1000.0);
    }
    SECTION("Solde négatif refusé") {
        REQUIRE_THROWS_AS(CompteBancaire("Bob", -50.0), std::invalid_argument);
    }
    SECTION("Numéro unique par instance") {
        CompteBancaire c1("A", 0), c2("B", 0);
        REQUIRE(c1.getNumero() != c2.getNumero());
    }
}

TEST_CASE("CompteBancaire — dépôt", "[compte]") {
    CompteBancaire c("Alice", 1000.0);
    SECTION("Dépôt valide augmente le solde") { REQUIRE(c.deposer(100)); REQUIRE(c.getSolde() == 1100.0); }
    SECTION("Dépôt nul est refusé") { REQUIRE_FALSE(c.deposer(0)); }
    SECTION("Dépôt négatif est refusé") { REQUIRE_FALSE(c.deposer(-10)); }
}

TEST_CASE("CompteBancaire — retrait", "[compte]") {
    CompteBancaire c("Alice", 1000.0);
    SECTION("Retrait inférieur au solde") { REQUIRE(c.retirer(500)); }
    SECTION("Retrait égal au solde") { REQUIRE(c.retirer(1000)); REQUIRE(c.getSolde() == 0); }
    SECTION("Retrait supérieur au solde échoue") { REQUIRE_FALSE(c.retirer(2000)); }
}

TEST_CASE("CompteBancaire — virement", "[compte]") {
    CompteBancaire c1("Alice", 1000.0), c2("Bob", 0.0);
    SECTION("Virement valide") {
        REQUIRE(c1.virement(c2, 200));
        REQUIRE(c1.getSolde() == 800.0);
        REQUIRE(c2.getSolde() == 200.0);
    }
    SECTION("Virement solde insuffisant") { REQUIRE_FALSE(c1.virement(c2, 5000)); }
    SECTION("Virement vers le même compte refusé") { REQUIRE_FALSE(c1.virement(c1, 100)); }
}

TEST_CASE("CompteBancaire — historique", "[compte]") {
    CompteBancaire c("Alice", 1000.0);
    c.deposer(100);
    REQUIRE(c.getHistorique().size() == 1);
    REQUIRE(c.getHistorique()[0].type == "Dépôt");
}

TEST_CASE("CompteBancaire — 10 opérations consécutives", "[compte]") {
    CompteBancaire c("Alice", 0.0);
    for (int i = 0; i < 10; ++i) c.deposer(100);
    REQUIRE(c.getSolde() == 1000.0);
}
```
*(20 tests attendus : dupliquer/étendre ces `SECTION` pour couvrir tous les cas listés dans l'énoncé.)*

---

## 🏆 Mini-projet du Jour 1 : Carnet d'adresses — Architecture de correction

```cpp
class CarnetAdresses {
    std::vector<Contact> contacts;
    std::string fichier;
public:
    explicit CarnetAdresses(std::string f) : fichier(std::move(f)) { charger(fichier); }

    void charger(const std::string& path) {
        std::ifstream in(path);
        std::string ligne;
        while (std::getline(in, ligne)) if (!ligne.empty()) contacts.push_back(contactFromCSV(ligne));
    }

    void sauvegarder() const {
        std::ofstream out(fichier);
        for (const auto& c : contacts) out << contactToCSV(c) << "\n";
    }

    void ajouter(Contact c) { contacts.push_back(std::move(c)); sauvegarder(); }

    bool supprimer(const std::string& nom) {
        auto it = std::find_if(contacts.begin(), contacts.end(),
                                [&](const Contact& c) { return c.nom == nom; });
        if (it == contacts.end()) return false;
        contacts.erase(it);
        sauvegarder();
        return true;
    }

    std::vector<Contact> rechercher(const std::string& terme) const {
        std::string termeLower = terme;
        std::transform(termeLower.begin(), termeLower.end(), termeLower.begin(), ::tolower);
        std::vector<Contact> resultats;
        for (const auto& c : contacts) {
            std::string nomLower = c.nom;
            std::transform(nomLower.begin(), nomLower.end(), nomLower.begin(), ::tolower);
            if (nomLower.find(termeLower) != std::string::npos ||
                c.email.find(terme) != std::string::npos)
                resultats.push_back(c);
        }
        return resultats;
    }

    void trier(const std::string& champ = "nom") {
        std::sort(contacts.begin(), contacts.end(), [&](const Contact& a, const Contact& b) {
            if (champ == "nom") return a.nom < b.nom;
            if (champ == "prenom") return a.prenom < b.prenom;
            return a.email < b.email;
        });
    }

    void afficherTous() const { for (const auto& c : contacts) afficherContact(c); }
    int getTaille() const { return static_cast<int>(contacts.size()); }
};
```

Le `MenuPrincipal` se construit comme une simple boucle `while` affichant les options (`ajouter`, `rechercher`, `modifier`, `supprimer`, `afficher`, `quitter`) et déléguant chaque action au `CarnetAdresses`. Points de vigilance corrigés : validation systématique des entrées, `const&` pour les paramètres de lecture, aucune allocation brute.

---

## ✅ Points clés à retenir du Jour 1

- Toujours préférer `const auto&` en boucle et en paramètre pour les objets.
- `explicit` sur les constructeurs à un argument évite les conversions implicites surprenantes.
- Le destructeur d'une classe de base polymorphique doit être `virtual`.
- Le Rule of Zero (déléguer à la STL) est préférable au Rule of Five sauf gestion de ressource brute.
- Les tests doivent couvrir les cas valides ET les cas d'erreur.
