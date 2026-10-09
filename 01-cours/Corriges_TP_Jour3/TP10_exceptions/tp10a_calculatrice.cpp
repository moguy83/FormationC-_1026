#include "test.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
class ExceptionCalcul : public std::runtime_error {public: using std::runtime_error::runtime_error;};
class DivisionParZero : public ExceptionCalcul {public: DivisionParZero():ExceptionCalcul("Division par zero") {}};
class SyntaxeInvalide : public ExceptionCalcul {public: using ExceptionCalcul::ExceptionCalcul;};
class DepassementCapacite : public ExceptionCalcul {public: DepassementCapacite():ExceptionCalcul("Resultat non representable") {}};
class Calculatrice {
    double resultat_=0;
public:
    double resultat() const noexcept {return resultat_;}
    double calculer(double a,char op,double b) {
        if(!std::isfinite(a)||!std::isfinite(b)) throw DepassementCapacite{};
        // Calcul local : ne toucher à resultat_ qu'une fois toutes les validations faites.
        // C'est la garantie forte : l'état observable est inchangé en cas d'exception.
        double candidat;
        switch(op) {
            case '+': candidat=a+b; break;
            case '-': candidat=a-b; break;
            case '*': candidat=a*b; break;
            case '/': if(b==0) throw DivisionParZero{}; candidat=a/b; break;
            default: throw SyntaxeInvalide("Operateur inconnu");
        }
        if(!std::isfinite(candidat)) throw DepassementCapacite{};
        resultat_=candidat; return resultat_;
    }
    double evaluer(const std::string& expression) {
        // Grammaire volontairement limitée : nombre opérateur nombre, sans parenthèses.
        // L'extraction double traite les signes et la notation scientifique.
        std::istringstream in(expression); double a,b; char op;
        if(!(in>>a>>op>>b)) throw SyntaxeInvalide("Attendu : nombre operateur nombre");
        in>>std::ws;
        if(!in.eof()) throw SyntaxeInvalide("Texte superflu");
        return calculer(a,op,b);
    }
};
int main() {
    Calculatrice c;
    CHECK(c.evaluer("10 / 2")==5); CHECK(c.evaluer("2 + 3")==5);
    CHECK(c.evaluer("8 - 3")==5); CHECK(c.evaluer("2.5 * 2")==5);
    doitLever<DivisionParZero>([&]{c.evaluer("1 / 0");}); CHECK(c.resultat()==5);
    doitLever<SyntaxeInvalide>([&]{c.evaluer("bonjour");}); CHECK(c.resultat()==5);
    doitLever<SyntaxeInvalide>([&]{c.evaluer("1 + 2 fin");});
    doitLever<SyntaxeInvalide>([&]{c.evaluer("1 % 2");});
    doitLever<DepassementCapacite>([&]{c.calculer(std::numeric_limits<double>::max(),'*',2);});
    CHECK(c.resultat()==5);
    // Les valeurs subnormales/arrondis restent autorisés : dépassement = résultat infini/NaN.
    std::cout<<"TP10A : calculatrice et garantie forte OK\n";
}
