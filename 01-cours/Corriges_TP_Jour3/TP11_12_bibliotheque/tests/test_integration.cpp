#include "bibliotheque/catalogue_bdd.hpp"
#include "test.hpp"
#include <libpq-fe.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
using namespace bibliotheque;
int main() {
    const char* env=std::getenv("TEST_DATABASE_URL");
    if(!env) {std::cerr<<"TEST_DATABASE_URL requis pour ces tests reels\n"; return 1;}
    const std::string url=env;
    // Chaque bloc ouvre sa session et sa table temporaire : aucun état partagé entre tests.
    // 01 : base vide.
    {CatalogueBDD db(url); CHECK(db.charger().size()==0);}
    // 02 : insertion et lecture.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"T",{1,"A"}}); db.sauvegarder(c); CHECK(db.charger().lire(1).titre=="T");}
    // 03 : modification puis persistance.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"T",{1,"A"}}); db.sauvegarder(c);
     c.modifier({1,"Nouveau",{1,"A"}}); db.sauvegarder(c); CHECK(db.charger().lire(1).titre=="Nouveau");}
    // 04 : suppression persistée.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"T",{1,"A"}}); db.sauvegarder(c);
     c.supprimer(1); db.sauvegarder(c); CHECK(db.charger().size()==0);}
    // 05 : doublon rejeté, données en base inchangées.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"T",{1,"A"}}); db.sauvegarder(c);
     doitLever<std::invalid_argument>([&]{c.ajouter({1,"X",{1,"A"}});}); CHECK(db.charger().lire(1).titre=="T");}
    // 06 : apostrophes, caractères non ASCII et texte ressemblant à une injection SQL.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"Ete'); DROP TABLE livres;--",{1,"O'Connor"}});
     db.sauvegarder(c); CHECK(db.charger().toJson()==c.toJson());}
    // 07 : catalogue contenant 10 000 livres (vrai aller-retour réseau).
    {CatalogueBDD db(url); Catalogue c; for(int i=1;i<=10000;++i) c.ajouter({i,"Livre "+std::to_string(i),{1,"A"}});
     db.sauvegarder(c); auto lu=db.charger(); CHECK(lu.size()==10000); CHECK(lu.lire(10000).titre=="Livre 10000");}
    // 08 : sauvegardes répétées = remplacement de l'instantané, pas duplication.
    {CatalogueBDD db(url); Catalogue c; c.ajouter({1,"T",{1,"A"}}); db.sauvegarder(c); db.sauvegarder(c); CHECK(db.charger().size()==1);}
    // 09 : une connexion initiale indisponible lève une exception.
    doitLever<std::runtime_error>([]{CatalogueBDD db("host=127.0.0.1 port=1 dbname=absente connect_timeout=2");});
    // 10 : perte REELLE de connexion : l'administrateur ferme uniquement NOTRE session de test.
    {CatalogueBDD db(url);
     std::unique_ptr<PGconn,decltype(&PQfinish)> admin(PQconnectdb(url.c_str()),PQfinish);
     CHECK(admin&&PQstatus(admin.get())==CONNECTION_OK);
     auto pid=std::to_string(db.backendId()); const char* params[]={pid.c_str()};
     std::unique_ptr<PGresult,decltype(&PQclear)> r(PQexecParams(admin.get(),
         "SELECT pg_terminate_backend($1::integer,5000)",1,nullptr,params,nullptr,nullptr,0),PQclear);
     CHECK(r&&PQresultStatus(r.get())==PGRES_TUPLES_OK);
     CHECK(std::string(PQgetvalue(r.get(),0,0))=="t");
     doitLever<std::runtime_error>([&]{db.charger();});}
    std::cout<<"TP12A : 10 scenarios PostgreSQL valides\n";
}
