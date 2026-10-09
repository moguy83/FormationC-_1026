#include "test.hpp"
#include <concepts>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
// Tester un objet const garantit que sauvegarder(const T&) pourra appeler serialize/getId.
template<class T> concept Persistable = requires(const T obj,std::ostream& os,std::istream& is) {
    {obj.serialize(os)} -> std::same_as<void>;
    {T::deserialize(is)} -> std::same_as<T>;
    {obj.getId()} -> std::convertible_to<int>;
};
class Etudiant {
    int id_; std::string nom_;
public:
    Etudiant(int id,std::string nom):id_(id),nom_(std::move(nom)) {
        if(id<0 || nom_.empty()) throw std::invalid_argument("Etudiant invalide");
    }
    int getId() const {return id_;}
    const std::string& nom() const {return nom_;}
    void serialize(std::ostream& os) const {
        // quoted préserve espaces, guillemets et antislashs dans le nom.
        os<<id_<<' '<<std::quoted(nom_)<<'\n';
        if(!os) throw std::runtime_error("Ecriture impossible");
    }
    static Etudiant deserialize(std::istream& is) {
        int id; std::string nom;
        if(!(is>>id>>std::quoted(nom))) throw std::runtime_error("Etudiant illisible");
        is>>std::ws;
        if(!is.eof()) throw std::runtime_error("Donnees superflues");
        return Etudiant(id,std::move(nom));
    }
};
template<Persistable T> void sauvegarder(const T& obj,const std::string& path) {
    std::ofstream out(path);
    if(!out) throw std::runtime_error("Ouverture en ecriture impossible");
    obj.serialize(out); out.close();
    if(!out) throw std::runtime_error("Fermeture en echec");
}
template<Persistable T> T charger(const std::string& path) {
    std::ifstream in(path);
    if(!in) throw std::runtime_error("Fichier absent ou inaccessible");
    return T::deserialize(in);
}
static_assert(Persistable<Etudiant>);
static_assert(!Persistable<int>);
int main() {
    Etudiant e(1,"Lina Martin"); sauvegarder(e,"etudiant.txt");
    auto copie=charger<Etudiant>("etudiant.txt");
    CHECK(copie.getId()==1); CHECK(copie.nom()==e.nom());
    std::istringstream invalide("abc");
    doitLever<std::runtime_error>([&]{Etudiant::deserialize(invalide);});
    std::cout<<"TP09D : concept et aller-retour OK\n";
}
