#include "bibliotheque/catalogue.hpp"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
namespace bibliotheque {
namespace {
// get<int>() peut effectuer une conversion réductrice : valider le type et la plage.
int idDepuisJson(const nlohmann::json& j) {
    if(!j.is_number_integer()) throw std::invalid_argument("Identifiant entier attendu");
    if(j.is_number_unsigned()) {
        auto n=j.get<unsigned long long>();
        if(n==0||n>static_cast<unsigned>(std::numeric_limits<int>::max()))
            throw std::invalid_argument("Identifiant hors plage");
        return static_cast<int>(n);
    }
    auto n=j.get<long long>();
    if(n<=0||n>std::numeric_limits<int>::max()) throw std::invalid_argument("Identifiant hors plage");
    return static_cast<int>(n);
}
}
void Catalogue::ajouter(Livre livre) {
    livre.valider(); const int id=livre.id;
    if(!livres_.emplace(id,std::move(livre)).second) throw std::invalid_argument("ID deja present");
    // Champs nommés dans le message (journal clé=valeur), pas de concaténation de format.
    spdlog::debug("event=livre_ajoute livre_id={}",id);
}
const Livre& Catalogue::lire(int id) const {return livres_.at(id);}
void Catalogue::modifier(Livre livre) {
    livre.valider(); auto& ancien=livres_.at(livre.id);
    // Le paramètre est déjà construit : échange sans allocation pour ces types standards.
    using std::swap; swap(ancien,livre);
}
void Catalogue::supprimer(int id) {
    if(livres_.erase(id)==0) throw std::out_of_range("Livre absent");
}
std::size_t Catalogue::size() const noexcept {return livres_.size();}
std::string Catalogue::toJson() const {
    auto result=nlohmann::json::array();
    for(const auto& [id,l]:livres_)
        result.push_back({{"id",id},{"titre",l.titre},{"auteur",{{"id",l.auteur.id},{"nom",l.auteur.nom}}}});
    return result.dump(2);
}
void Catalogue::fromJson(const std::string& texte) {
    auto j=nlohmann::json::parse(texte);
    if(!j.is_array()) throw std::invalid_argument("Catalogue : tableau JSON attendu");
    Catalogue temporaire;
    for(const auto& l:j) {
        const auto& a=l.at("auteur");
        temporaire.ajouter({idDepuisJson(l.at("id")),l.at("titre").get<std::string>(),
                           {idDepuisJson(a.at("id")),a.at("nom").get<std::string>()}});
    }
    // Garantie forte : JSON invalide, doublon ou validation échouée => catalogue intact.
    livres_.swap(temporaire.livres_);
}
void Catalogue::sauvegarder(const std::filesystem::path& path) const {
    const auto texte=toJson();
    std::ofstream out(path);
    if(!out) throw std::runtime_error("Impossible d'ouvrir le catalogue en ecriture");
    out<<texte; out.close();
    if(!out) throw std::runtime_error("Echec d'ecriture du catalogue");
}
void Catalogue::charger(const std::filesystem::path& path) {
    std::ifstream in(path);
    if(!in) throw std::runtime_error("Impossible d'ouvrir le catalogue en lecture");
    std::string texte{std::istreambuf_iterator<char>(in),{}};
    if(in.bad()) throw std::runtime_error("Echec de lecture du catalogue");
    fromJson(texte);
}
}
