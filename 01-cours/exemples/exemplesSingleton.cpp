#include <mutex>

// avant c++11
class Database
{
    private:
        static std::unique_ptr<Database> instance;
        static std::once_flag initFlag;
        std::string connexion;

        Database(const std::string &url) : connexion(url) {}

    public:
        // Thread-safe avec call_once
        static Database& getInstance() {
            std::call_once(initFlag, []() {
                instance = std::unique_ptr<Database>(new Database("postgresql://localhost/mydb"));
            });

            return *instance;
        }

    void executer(const std::string &query) { /* ... */ }

    // Interdire copie et move

    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;
};

// apres c++11
class DatabaseModern {
    public :
        static DatabaseModern& getInstance() {
            static DatabaseModern instance("postgresql://localhost/mydb");
            return instance;
        }

        void executer(const std::string &query) { /* ... */ }

        DatabaseModern(const DatabaseModern &) = delete;
        DatabaseModern &operator=(const DatabaseModern &) = delete;

    private:
    DatabaseModern() = default;
}

DatabaseModern& db = Database::getInstance();

DatabaseModerne& autre = db; // Erreur de compilation, pas de copie possible

void enregistrerUtilisateur(const std::string &nom) {
    DatabaseModern& db = DatabaseModern::getInstance();
    db.executer("INSERT INTO utilisateurs (nom) VALUES ('" + nom + "')");
} // fonction dependante de DatabaseModern, pas de test unitaire possible

void enregistreUtilisateurTestable(const std::string &nom, DatabaseModern& db) {
    db.executer("INSERT INTO utilisateurs (nom) VALUES ('" + nom + "')");
} // fonction testable, on peut passer un mock de DatabaseModern