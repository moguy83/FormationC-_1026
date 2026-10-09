#include "bibliotheque/catalogue.hpp"
#include "test.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
using bibliotheque::Catalogue;
int main() {
    Catalogue c; CHECK(c.size()==0); CHECK(c.toJson()=="[]");
    c.ajouter({1,"Le Hobbit",{1,"Tolkien"}}); CHECK(c.lire(1).titre=="Le Hobbit");
    doitLever<std::invalid_argument>([&]{c.ajouter({1,"Doublon",{1,"A"}});});
    c.modifier({1,"Bilbo",{1,"Tolkien"}}); CHECK(c.lire(1).titre=="Bilbo");
    doitLever<std::out_of_range>([&]{c.modifier({2,"Absent",{1,"A"}});});
    doitLever<std::out_of_range>([&]{c.lire(2);});
    doitLever<std::out_of_range>([&]{c.supprimer(2);});
    for(const auto& l: {bibliotheque::Livre{0,"T",{1,"A"}}, {2,"",{1,"A"}},
                        {2,"T",{0,"A"}}, {2,"T",{1,""}}})
        doitLever<std::invalid_argument>([&]{c.ajouter(l);});
    auto avant=c.toJson();
    for(const std::string mauvais:{"pas json","{}","[{}]",
        "[{\"id\":1.5,\"titre\":\"T\",\"auteur\":{\"id\":1,\"nom\":\"A\"}}]",
        "[{\"id\":999999999999,\"titre\":\"T\",\"auteur\":{\"id\":1,\"nom\":\"A\"}}]"}) {
        doitLever<std::exception>([&]{c.fromJson(mauvais);}); CHECK(c.toJson()==avant);
    }
    auto j=nlohmann::json::parse(avant); j.push_back(j.at(0));
    doitLever<std::invalid_argument>([&]{c.fromJson(j.dump());}); CHECK(c.toJson()==avant);
    c.ajouter({2,"\"Ete\"\nFrançais",{2,"O'Connor"}});
    c.sauvegarder("catalogue_test.json"); Catalogue d; d.charger("catalogue_test.json");
    CHECK(d.toJson()==c.toJson());
    doitLever<std::runtime_error>([&]{d.charger("absent/catalogue.json");});
    doitLever<std::runtime_error>([&]{d.sauvegarder("absent/catalogue.json");});
    std::ofstream("invalide.json")<<"[";
    doitLever<std::exception>([&]{d.charger("invalide.json");}); CHECK(d.size()==2);
    c.supprimer(1); CHECK(c.size()==1); c.fromJson("[]"); CHECK(c.size()==0);
    std::cout<<"TP11 : CRUD, invariants et persistance JSON OK\n";
}
