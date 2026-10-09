#include "tableau_dynamique.hpp"
#include "test.hpp"
#include <iostream>
#include <string>
struct Etudiant { std::string nom; int age; };
// Type de test dont la copie peut échouer : démontre le nettoyage du tableau.
struct CopieFragile {
    inline static int vivants=0;
    inline static int copiesAvantErreur=100;
    int valeur;
    explicit CopieFragile(int v):valeur(v) {++vivants;}
    CopieFragile(const CopieFragile& autre):valeur(autre.valeur) {
        if(copiesAvantErreur--==0) throw std::runtime_error("Copie refusee");
        ++vivants;
    }
    CopieFragile(CopieFragile&& autre) noexcept(false):valeur(autre.valeur) {++vivants;}
    ~CopieFragile() {--vivants;}
};
int main() {
    TableauDynamique<int> a; a.push_back(10); a.push_back(20);
    auto copie=a; copie.at(0)=99; CHECK(a.at(0)==10);
    auto deplace=std::move(copie); CHECK(copie.size()==0); CHECK(deplace.at(0)==99);
    a=a; CHECK(a.size()==2); a.push_back(a.at(0)); CHECK(a.at(2)==10);
    a.pop_back(); CHECK(a.size()==2); a.clear(); CHECK(a.size()==0);
    doitLever<std::out_of_range>([&]{a.at(0);});
    doitLever<std::out_of_range>([&]{a.pop_back();});
    TableauDynamique<double> d; d.push_back(2.5); CHECK(d.at(0)==2.5);
    TableauDynamique<std::string> s; s.push_back("bonjour"); CHECK(s.at(0)=="bonjour");
    TableauDynamique<Etudiant> e; e.push_back({"Lina",22}); CHECK(e.at(0).nom=="Lina");
    TableauDynamique<std::unique_ptr<int>> p; p.push_back(std::make_unique<int>(42));
    p.push_back(std::make_unique<int>(7)); CHECK(*p.at(0)==42);
    {
        TableauDynamique<CopieFragile> f;
        f.push_back(CopieFragile{1}); f.push_back(CopieFragile{2});
        CopieFragile::copiesAvantErreur=0;
        doitLever<std::runtime_error>([&]{f.push_back(CopieFragile{3});});
        CHECK(f.size()==2); CHECK(f.at(0).valeur==1); CHECK(CopieFragile::vivants==2);
        CopieFragile::copiesAvantErreur=1;
        doitLever<std::runtime_error>([&]{auto impossible=f;});
        CHECK(CopieFragile::vivants==2);
    }
    CHECK(CopieFragile::vivants==0);
    std::cout << "TP09A : copies indépendantes, move, bornes et types vérifiés\n";
}
