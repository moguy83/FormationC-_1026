#include "bibliotheque/catalogue_bdd.hpp"
#include <libpq-fe.h>
#include <stdexcept>
namespace bibliotheque {
namespace {
using Resultat=std::unique_ptr<PGresult,decltype(&PQclear)>;
Resultat verifier(PGresult* raw) {
    Resultat r(raw,PQclear);
    if(!r) throw std::runtime_error("PostgreSQL : resultat absent");
    auto status=PQresultStatus(r.get());
    if(status!=PGRES_COMMAND_OK&&status!=PGRES_TUPLES_OK)
        throw std::runtime_error(PQresultErrorMessage(r.get()));
    return r;
}
}
struct CatalogueBDD::Impl {
    std::unique_ptr<PGconn,decltype(&PQfinish)> db{nullptr,PQfinish};
    explicit Impl(const std::string& connexion):db(PQconnectdb(connexion.c_str()),PQfinish) {
        if(!db||PQstatus(db.get())!=CONNECTION_OK) throw std::runtime_error("Connexion PostgreSQL impossible");
        // Table TEMP propre à la session : tests isolés, aucun DROP sur une base partagée.
        verifier(PQexec(db.get(),"CREATE TEMP TABLE catalogue_snapshot (id INTEGER PRIMARY KEY CHECK(id=1), contenu TEXT NOT NULL)"));
    }
};
CatalogueBDD::CatalogueBDD(const std::string& connexion):impl_(std::make_unique<Impl>(connexion)) {}
CatalogueBDD::~CatalogueBDD()=default;
void CatalogueBDD::sauvegarder(const Catalogue& catalogue) {
    auto texte=catalogue.toJson(); const char* params[]={texte.c_str()};
    // Paramétrer la requête protège les apostrophes ET évite l'injection SQL.
    // Un seul INSERT...ON CONFLICT est atomique côté serveur (transaction implicite).
    verifier(PQexecParams(impl_->db.get(),
        "INSERT INTO catalogue_snapshot VALUES(1,$1) ON CONFLICT(id) DO UPDATE SET contenu=EXCLUDED.contenu",
        1,nullptr,params,nullptr,nullptr,0));
}
Catalogue CatalogueBDD::charger() const {
    auto r=verifier(PQexec(impl_->db.get(),"SELECT contenu FROM catalogue_snapshot WHERE id=1"));
    Catalogue c;
    if(PQntuples(r.get())==1) c.fromJson(PQgetvalue(r.get(),0,0));
    return c;
}
int CatalogueBDD::backendId() const {return PQbackendPID(impl_->db.get());}
}
