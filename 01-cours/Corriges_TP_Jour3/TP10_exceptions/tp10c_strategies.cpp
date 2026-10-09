#include <chrono>
#include <expected>
#include <iostream>
#include <optional>
#include <stdexcept>
struct Connexion { int id; };
enum class Erreur { indisponible };
// Simulation : aucune vraie connexion réseau dans ce micro-benchmark.
Connexion connecterBDD_exception(bool disponible) {
    if(!disponible) throw std::runtime_error("BDD indisponible");
    return {42};
}
std::optional<Connexion> connecterBDD_optional(bool disponible) {
    if(!disponible) return std::nullopt;
    return Connexion{42}; // optional ne décrit PAS la cause de l'échec.
}
std::expected<Connexion,Erreur> connecterBDD_expected(bool disponible) {
    if(!disponible) return std::unexpected(Erreur::indisponible);
    return Connexion{42}; // C++23 + bibliothèque standard prenant en charge expected.
}
[[nodiscard]] int connecterBDD_code(bool disponible,Connexion& sortie) {
    if(!disponible) return 1;
    sortie={42}; return 0; // Contrat : sortie n'est utilisable que si retour == 0.
}
template<class F> void mesurer(const char* nom,F f,int tauxErreur) {
    constexpr int n=10000;
    // L'entrée volatile évite que le compilateur remplace toute la boucle par une constante.
    volatile int taux=tauxErreur; long somme=0;
    auto debut=std::chrono::steady_clock::now();
    for(int i=0;i<n;++i) somme+=f(i%100>=taux);
    auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-debut).count();
    std::cout<<nom<<", erreurs="<<tauxErreur<<"%, "<<ns<<" ns, controle="<<somme<<'\n';
}
int main() {
    for(int taux:{0,1,50,100}) {
        mesurer("exceptions",[](bool ok){try{return connecterBDD_exception(ok).id;}catch(const std::runtime_error&){return 0;}},taux);
        mesurer("optional",[](bool ok){auto r=connecterBDD_optional(ok); return r?r->id:0;},taux);
        mesurer("expected",[](bool ok){auto r=connecterBDD_expected(ok); return r?r->id:0;},taux);
        mesurer("code retour",[](bool ok){Connexion c{}; return connecterBDD_code(ok,c)==0?c.id:0;},taux);
    }
    // Ne pas en déduire un classement universel : simulation très courte, bruit de mesure,
    // optimiseur et fréquence des erreurs dominent. Une vraie BDD coûte beaucoup plus cher.
    // Exceptions : rupture exceptionnelle propagée sur plusieurs couches.
    // optional : absence normale sans diagnostic. expected : erreur métier détaillée attendue.
    // Code de retour : interop API C, à vérifier systématiquement.
}
