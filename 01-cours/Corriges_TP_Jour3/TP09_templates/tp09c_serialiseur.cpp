#include "test.hpp"
#include <nlohmann/json.hpp>
#include <cmath>
#include <limits>
#include <iostream>
#include <string>
#include <vector>
// C — On délègue le parsing et l'échappement JSON à une vraie bibliothèque.
// Une chaîne contenant des guillemets ou des \n ne doit pas être concaténée à la main.
template<class T> class Serialiseur; // Pas de définition générale : types non prévus refusés.
template<> class Serialiseur<int> {
public:
    std::string toJson(int v) const {return nlohmann::json(v).dump();}
    int fromJson(const std::string& s) const {
        auto j=nlohmann::json::parse(s);
        if(!j.is_number_integer()) throw std::invalid_argument("Entier JSON attendu");
        // get<int>() seul peut convertir/tronquer : contrôler la plage AVANT conversion.
        if(j.is_number_unsigned()) {
            if(j.get<unsigned long long>()>static_cast<unsigned>(std::numeric_limits<int>::max()))
                throw std::out_of_range("Entier trop grand");
        } else {
            auto x=j.get<long long>();
            if(x<std::numeric_limits<int>::min() || x>std::numeric_limits<int>::max())
                throw std::out_of_range("Entier hors plage");
        }
        return j.get<int>();
    }
};
template<> class Serialiseur<double> {
public:
    std::string toJson(double v) const {
        if(!std::isfinite(v)) throw std::invalid_argument("NaN et infini ne sont pas du JSON");
        return nlohmann::json(v).dump();
    }
    double fromJson(const std::string& s) const {
        auto j=nlohmann::json::parse(s);
        if(!j.is_number()) throw std::invalid_argument("Nombre attendu");
        return j.get<double>();
    }
};
template<> class Serialiseur<std::string> {
public:
    std::string toJson(const std::string& v) const {return nlohmann::json(v).dump();}
    std::string fromJson(const std::string& s) const {return nlohmann::json::parse(s).get<std::string>();}
};
template<> class Serialiseur<bool> {
public:
    std::string toJson(bool v) const {return v?"true":"false";}
    bool fromJson(const std::string& s) const {return nlohmann::json::parse(s).get<bool>();}
};
// Spécialisation PARTIELLE : T reste générique, chaque élément réutilise sa spécialisation.
template<class T> class Serialiseur<std::vector<T>> {
public:
    std::string toJson(const std::vector<T>& v) const {
        auto j=nlohmann::json::array();
        for(const auto& x:v) j.push_back(nlohmann::json::parse(Serialiseur<T>{}.toJson(x)));
        return j.dump();
    }
    std::vector<T> fromJson(const std::string& s) const {
        auto j=nlohmann::json::parse(s);
        if(!j.is_array()) throw std::invalid_argument("Tableau JSON attendu");
        std::vector<T> v;
        for(const auto& x:j) v.push_back(Serialiseur<T>{}.fromJson(x.dump()));
        return v;
    }
};
int main() {
    Serialiseur<int> i; CHECK(i.fromJson(i.toJson(42))==42);
    doitLever<std::invalid_argument>([&]{i.fromJson("1.5");});
    doitLever<std::out_of_range>([&]{i.fromJson("9999999999");});
    Serialiseur<double> d; CHECK(d.fromJson(d.toJson(2.5))==2.5);
    doitLever<std::invalid_argument>([&]{d.toJson(std::numeric_limits<double>::infinity());});
    Serialiseur<std::string> s; const std::string texte="Un \"livre\"\nFrançais";
    CHECK(s.fromJson(s.toJson(texte))==texte);
    Serialiseur<bool> b; CHECK(b.fromJson(b.toJson(true)));
    Serialiseur<std::vector<std::string>> v;
    CHECK(v.fromJson(v.toJson({texte,""}))==std::vector<std::string>({texte,""}));
    CHECK(v.fromJson("[]").empty());
    std::cout<<"TP09C : sérialisation et validation JSON OK\n";
}
