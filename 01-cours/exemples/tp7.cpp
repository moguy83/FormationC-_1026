// A

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <stdexcept>
#include <utility>

// Interface commune à tous les loggers
class ILogger
{
public:
    virtual void log(const std::string &message) = 0;
    virtual ~ILogger() = default;
};

// Logger dans le terminal
class ConsoleLogger : public ILogger
{
public:
    void log(const std::string &message) override
    {
        std::cout << "[CONSOLE] " << message << "\n";
    }
};

// Logger dans un fichier
class FileLogger : public ILogger
{
private:
    std::ofstream fichier;

public:
    explicit FileLogger(const std::string &chemin)
        : fichier(chemin, std::ios::app)
    {
        if (!fichier)
        {
            throw std::runtime_error(
                "Impossible d'ouvrir : " + chemin);
        }
    }

    void log(const std::string &message) override
    {
        fichier << "[FILE] " << message << "\n";

        if (!fichier)
        {
            throw std::runtime_error("Echec d'ecriture");
        }
    }
};

// Echappement minimal des chaines JSON
std::string echapperJSON(const std::string &texte)
{
    std::string resultat;

    for (unsigned char c : texte)
    {
        switch (c)
        {
        case '"':
            resultat += "\\\"";
            break;
        case '\\':
            resultat += "\\\\";
            break;
        case '\n':
            resultat += "\\n";
            break;
        case '\r':
            resultat += "\\r";
            break;
        case '\t':
            resultat += "\\t";
            break;
        default:
            if (c < 0x20)
            {
                const char *hex = "0123456789abcdef";
                resultat += "\\u00";
                resultat += hex[c >> 4];
                resultat += hex[c & 0x0f];
            }
            else
            {
                resultat += static_cast<char>(c);
            }
        }
    }
    return resultat;
}

// Logger au format JSON
class JsonLogger : public ILogger
{
public:
    void log(const std::string &message) override
    {
        std::cout
            << "{\"level\":\"INFO\",\"message\":\""
            << echapperJSON(message)
            << "\"}\n";
    }
};

// Factory Singleton
class LoggerFactory
{
private:
    LoggerFactory() = default;

public:
    LoggerFactory(const LoggerFactory &) = delete;
    LoggerFactory &operator=(const LoggerFactory &) = delete;

    static LoggerFactory &getInstance()
    {
        static LoggerFactory instance;
        return instance;
    }

    std::unique_ptr<ILogger> creer(
        const std::string &type,
        const std::string &chemin = "app.log")
    {
        if (type == "console")
        {
            return std::make_unique<ConsoleLogger>();
        }

        if (type == "file")
        {
            return std::make_unique<FileLogger>(chemin);
        }

        if (type == "json")
        {
            return std::make_unique<JsonLogger>();
        }

        throw std::invalid_argument(
            "Type de logger inconnu : " + type);
    }
};

int main()
{
    try
    {
        // Lecture du fichier de configuration
        std::ifstream config("logger.conf");

        if (!config)
        {
            throw std::runtime_error(
                "Fichier logger.conf introuvable");
        }

        std::string type;
        std::getline(config, type);

        std::string chemin = "app.log";
        std::string ligne;

        if (std::getline(config, ligne) && !ligne.empty())
        {
            chemin = ligne;
        }

        // Acces au Singleton et creation
        auto &factory = LoggerFactory::getInstance();

        std::unique_ptr<ILogger> logger =
            factory.creer(type, chemin);

        logger->log("Application demarree");
        logger->log("Connexion etablie");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erreur : " << e.what() << "\n";
        return 1;
    }

    return 0;
}

// B

#include <iostream>
#include <vector>
#include <functional>

class CapteurTemperature {
    std::vector<std::function<void(double)>> abonnes;

public:
    void subscribe(std::function<void(double)> callback) {
        abonnes.push_back(callback);
    }

    void mesurer(double temperature) {
        std::cout << "Mesure de la température : " << temperature << "°C\n";

        if (temperature > 80.0)
        {
            for (const auto &callback : abonnes)
            {
                callback(temperature);
            }
        }
    }
};

class AlertEmail
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte Email : Température critique de " << temperature << "°C\n";
    }
};

class AlertSMS
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte SMS : Température critique de " << temperature << "°C\n";
    }
};

class AlertDashboard
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte Dashboard : Température critique de " << temperature << "°C\n";
    }
};

int main() {
    CapteurTemperature capteur;

    AlertEmail email;
    AlertSMS sms;
    AlertDashboard dashboard;

    capteur.subscribe([&email](double temperature)
                      { email.notifier(temperature); });

    capteur.subscribe([&sms](double temperature)
                      { sms.notifier(temperature); });

    capteur.subscribe([&dashboard](double temperature)
                      { dashboard.notifier(temperature); });

    capteur.mesurer(65);
    capteur.mesurer(85);
    capteur.mesurer(95);
}

// C

/*
auto query = SQLBuilder()
    .select({"id", "nom", "email"})
    .from("utilisateurs")
    .where("age > 18")
    .orderBy("nom", ASC)
    .limit(50)
    .build();

SELECT id, nom, email FROM utilisateurs WHERE age > 18 ORDER BY nom ASC LIMIT 50
*/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <utility>

enum class Ordre
{
    ASC,
    DESC
};

class SQLBuilder
{
private:
    std::vector<std::string> colonnes;
    std::string table;
    std::string condition;
    std::string colonneTri;
    Ordre ordre = Ordre::ASC;
    int limite = -1;

public:
    SQLBuilder &select(std::vector<std::string> cols)
    {
        colonnes = std::move(cols);
        return *this;
    }

    SQLBuilder &from(const std::string &nomTable)
    {
        table = nomTable;
        return *this;
    }

    SQLBuilder &where(const std::string &cond)
    {
        condition = cond;
        return *this;
    }

    SQLBuilder &orderBy(
        const std::string &colonne,
        Ordre direction)
    {
        colonneTri = colonne;
        ordre = direction;
        return *this;
    }

    SQLBuilder &limit(int valeur)
    {
        if (valeur < 0)
        {
            throw std::invalid_argument(
                "La limite doit etre positive ou nulle");
        }

        limite = valeur;
        return *this;
    }

    std::string build() const
    {
        if (table.empty())
        {
            throw std::runtime_error(
                "La clause FROM est obligatoire");
        }

        std::ostringstream sql;

        sql << "SELECT ";

        if (colonnes.empty())
        {
            sql << "*";
        }
        else
        {
            for (std::size_t i = 0; i < colonnes.size(); ++i)
            {
                if (i > 0)
                {
                    sql << ", ";
                }
                sql << colonnes[i];
            }
        }

        sql << " FROM " << table;

        if (!condition.empty())
        {
            sql << " WHERE " << condition;
        }

        if (!colonneTri.empty())
        {
            sql << " ORDER BY " << colonneTri;

            if (ordre == Ordre::ASC)
            {
                sql << " ASC";
            }
            else
            {
                sql << " DESC";
            }
        }

        if (limite >= 0)
        {
            sql << " LIMIT " << limite;
        }

        return sql.str();
    }
};

int main()
{
    auto query = SQLBuilder()
                     .select({"id", "nom", "email"})
                     .from("utilisateurs")
                     .where("age > 18")
                     .orderBy("nom", Ordre::ASC)
                     .limit(50)
                     .build();

    std::cout << query << "\n";

    return 0;
}

// D

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>

// Strategie 1 : tri croissant
struct TriCroissant
{
    void operator()(std::vector<int> &v) const
    {
        std::sort(v.begin(), v.end());
    }
};

// Strategie 2 : tri decroissant
struct TriDecroissant
{
    void operator()(std::vector<int> &v) const
    {
        std::sort(
            v.begin(),
            v.end(),
            std::greater<int>{});
    }
};

// Strategie 3 : tri par valeur absolue
struct TriValeurAbsolue
{
    void operator()(std::vector<int> &v) const
    {
        std::sort(
            v.begin(),
            v.end(),
            [](int a, int b)
            {
                return std::abs(
                           static_cast<long long>(a)) < std::abs(static_cast<long long>(b));
            });
    }
};

// Strategy generique
template <typename StrategieTriement>
class Trieur
{
private:
    StrategieTriement strategie;

public:
    void trier(std::vector<int> &v)
    {
        strategie(v);
    }
};

// Affichage des valeurs
void afficher(const std::vector<int> &v)
{
    for (int valeur : v)
    {
        std::cout << valeur << " ";
    }
    std::cout << "\n";
}

int main()
{
    std::vector<int> valeurs = {5, -2, 8, -1, 3};

    Trieur<TriCroissant> croissant;
    Trieur<TriDecroissant> decroissant;
    Trieur<TriValeurAbsolue> absolu;

    auto v1 = valeurs;
    croissant.trier(v1);

    std::cout << "Croissant : ";
    afficher(v1);

    auto v2 = valeurs;
    decroissant.trier(v2);

    std::cout << "Decroissant : ";
    afficher(v2);

    auto v3 = valeurs;
    absolu.trier(v3);

    std::cout << "Valeur absolue : ";
    afficher(v3);

    return 0;
}

#include <iostream>
#include <chrono>
#include <memory>
#include <cstdint>

// Version polymorphisme dynamique
struct IOperation
{
    virtual int appliquer(int x) const = 0;
    virtual ~IOperation() = default;
};

struct OperationDynamique : IOperation
{
    int appliquer(int x) const override
    {
        return x + 1;
    }
};

// Version statique
struct OperationStatique
{
    int operator()(int x) const
    {
        return x + 1;
    }
};

template <typename Strategie>
class Calculateur
{
    Strategie strategie;

public:
    int calculer(int x) const
    {
        return strategie(x);
    }
};

int main()
{
    constexpr std::uint64_t N = 10000000;

    std::unique_ptr<IOperation> dynamique =
        std::make_unique<OperationDynamique>();

    Calculateur<OperationStatique> statique;

    volatile std::uint64_t resultatDynamique = 0;
    volatile std::uint64_t resultatStatique = 0;

    auto debut1 = std::chrono::steady_clock::now();

    for (std::uint64_t i = 0; i < N; ++i)
    {
        resultatDynamique =
            resultatDynamique +
            dynamique->appliquer(static_cast<int>(i));
    }

    auto fin1 = std::chrono::steady_clock::now();

    auto debut2 = std::chrono::steady_clock::now();

    for (std::uint64_t i = 0; i < N; ++i)
    {
        resultatStatique =
            resultatStatique +
            statique.calculer(static_cast<int>(i));
    }

    auto fin2 = std::chrono::steady_clock::now();

    auto tempsDyn = std::chrono::duration<double, std::milli>(
                        fin1 - debut1)
                        .count();

    auto tempsStat = std::chrono::duration<double, std::milli>(
                         fin2 - debut2)
                         .count();

    std::cout << "Dynamique : " << tempsDyn << " ms\n";
    std::cout << "Statique  : " << tempsStat << " ms\n";

    std::cout << "Verification : "
              << resultatDynamique << " / "
              << resultatStatique << "\n";
}
