#pragma once
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
class ConfigError: public std::runtime_error {public: using std::runtime_error::runtime_error;};
class FileNotFound: public ConfigError {public: using ConfigError::ConfigError;};
class ParseError: public ConfigError {public: using ConfigError::ConfigError;};
class WriteError: public ConfigError {public: using ConfigError::ConfigError;};
class KeyNotFound: public ConfigError {public: using ConfigError::ConfigError;};
class FichierConfig {
    std::map<std::string,std::string> valeurs_;
    static std::string trim(const std::string& s) {
        auto a=s.find_first_not_of(" \t\r");
        if(a==std::string::npos) return {};
        return s.substr(a,s.find_last_not_of(" \t\r")-a+1);
    }
    static void valider(const std::string& cle,const std::string& valeur) {
        // Même grammaire à l'écriture et à la lecture : aller-retour sans perte.
        if(cle.empty()||trim(cle)!=cle||cle.front()=='#'||cle.find_first_of("=\n\r")!=std::string::npos)
            throw ParseError("Cle invalide");
        if(trim(valeur)!=valeur||valeur.find_first_of("\n\r")!=std::string::npos)
            throw ParseError("Valeur invalide");
    }
public:
    void charger(const std::string& path) {
        std::ifstream in(path);
        if(!in) throw FileNotFound("Fichier absent ou inaccessible : "+path);
        std::map<std::string,std::string> temporaire;
        std::string ligne; std::size_t numero=0;
        while(std::getline(in,ligne)) {
            ++numero; ligne=trim(ligne);
            if(ligne.empty()||ligne.front()=='#') continue;
            auto sep=ligne.find('=');
            if(sep==std::string::npos) throw ParseError("Ligne "+std::to_string(numero)+" : '=' absent");
            auto cle=trim(ligne.substr(0,sep)), val=trim(ligne.substr(sep+1));
            valider(cle,val);
            if(!temporaire.emplace(cle,val).second)
                throw ParseError("Ligne "+std::to_string(numero)+" : cle dupliquee");
        }
        if(in.bad()) throw ParseError("Erreur de lecture : "+path);
        // COMMIT : swap des map avec allocateur standard ne lève pas.
        // Une erreur de parsing n'a jusqu'ici touché que la map temporaire.
        valeurs_.swap(temporaire);
    }
    void sauvegarder(const std::string& path) noexcept(false) {
        std::ofstream out(path);
        if(!out) throw WriteError("Ouverture impossible : "+path);
        for(const auto& [cle,val]:valeurs_) out<<cle<<'='<<val<<'\n';
        out.close(); // Vérifier aussi les erreurs d'écriture différée à la fermeture.
        if(!out) throw WriteError("Ecriture impossible : "+path);
        // Le TP exige une garantie forte sur charger, pas sur le fichier sauvegardé.
        // Une panne disque peut laisser ce fichier partiel : pas de promesse d'atomicité ici.
    }
    std::string get(const std::string& cle) const {
        auto it=valeurs_.find(cle);
        if(it==valeurs_.end()) throw KeyNotFound("Cle absente : "+cle);
        return it->second;
    }
    void set(const std::string& cle,const std::string& val) {
        valider(cle,val); valeurs_.insert_or_assign(cle,val);
    }
};
