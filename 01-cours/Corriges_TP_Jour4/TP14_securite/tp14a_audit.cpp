#include "test.hpp"
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
// Adaptateur de démonstration : en production, lier ces paramètres avec sqlite3_bind_int,
// PQexecParams, etc. Cette classe enregistre l'appel et n'exécute aucune suppression réelle.
struct RequeteEnregistree {
    std::string sql;
    std::vector<int> parametres;
    void executerPreparee(std::string requete, std::vector<int> valeurs) {
        sql = std::move(requete);
        parametres = std::move(valeurs);
    }
};
void traiterCommande(const std::string &cmd, int user_id, int order_id, int utilisateurAuthentifie,
                     RequeteEnregistree &db) {
    // Audit du support :
    // 1. sprintf(buffer[64],...) déborde si cmd est long ; cmd peut aussi être nullptr.
    // 2. new int[user_id] accepte une taille non contrôlée ; fuite + i<=user_id hors limites.
    // 3. to_string(int) n'est pas une injection SQL directe. Le vrai doute est métier :
    //    l'identifiant utilisateur est comparé à l'identifiant d'une commande !
    //    Et aucune autorisation ne vérifie que l'utilisateur possède la commande.
    if (user_id <= 0 || order_id <= 0)
        throw std::invalid_argument("Identifiant invalide");
    if (user_id != utilisateurAuthentifie)
        throw std::runtime_error("Utilisateur non autorise");
    if (cmd.size() > 1024 || cmd.find_first_of("\r\n") != std::string::npos)
        throw std::invalid_argument("Commande trop longue ou log multiline");
    const std::string message = "Commande: " + cmd; // Allocation gérée par std::string.
    // Un identifiant n'est pas une taille : choisir une borne explicite pour les données.
    std::vector<int> data(64);
    std::iota(data.begin(), data.end(), 0);
    CHECK(data.back() == 63);
    CHECK(message.size() == cmd.size() + 10);
    // L'utilisateur authentifié vient de la session, jamais d'un champ libre du client.
    // Le prédicat owner_id fait respecter l'appartenance dans la même requête.
    db.executerPreparee("DELETE FROM orders WHERE id=? AND owner_id=?",
                        {order_id, utilisateurAuthentifie});
    // Dans l'adaptateur réel : contrôler aussi les lignes affectées (absent/non autorisé).
}
int main() {
    RequeteEnregistree db;
    traiterCommande("supprimer", 7, 42, 7, db);
    CHECK(db.parametres == std::vector<int>({42, 7}));
    doitLever<std::invalid_argument>([&] { traiterCommande("x", -1, 42, 7, db); });
    doitLever<std::runtime_error>([&] { traiterCommande("x", 8, 42, 7, db); });
    doitLever<std::invalid_argument>(
        [&] { traiterCommande(std::string(1025, 'x'), 7, 42, 7, db); });
    doitLever<std::invalid_argument>([&] { traiterCommande("x\nfaux log", 7, 42, 7, db); });
    std::cout << "Audit corrige : memoire, bornes, identites et requete parametree\n";
}
