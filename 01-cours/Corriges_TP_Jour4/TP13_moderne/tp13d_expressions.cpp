#include "test.hpp"
#include <charconv>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
class Calculateur {
    std::string_view texte_;
    std::size_t pos_ = 0;
    unsigned profondeur_ = 0;
    std::map<char, std::function<double(double, double)>> ops_{
        {'+', [](double a, double b) { return a + b; }},
        {'-', [](double a, double b) { return a - b; }},
        {'*', [](double a, double b) { return a * b; }},
        {'/',
         [](double a, double b) {
             if (b == 0)
                 throw std::domain_error("Division par zero");
             return a / b;
         }},
        {'^', [](double a, double b) { return std::pow(a, b); }}};
    void espaces() {
        while (pos_ < texte_.size() &&
               (texte_[pos_] == ' ' || texte_[pos_] == '\t' || texte_[pos_] == '\n'))
            ++pos_;
    }
    bool prendre(char c) {
        espaces();
        if (pos_ < texte_.size() && texte_[pos_] == c) {
            ++pos_;
            return true;
        }
        return false;
    }
    double appliquer(char op, double a, double b) {
        double r = ops_.at(op)(a, b);
        if (!std::isfinite(r))
            throw std::domain_error("Resultat hors domaine ou capacite");
        return r;
    }
    // Grammaire : expr=terme {(+|-) terme}, terme=unaire {(*|/) unaire}.
    double expression() {
        double v = terme();
        for (;;) {
            if (prendre('+'))
                v = appliquer('+', v, terme());
            else if (prendre('-'))
                v = appliquer('-', v, terme());
            else
                return v;
        }
    }
    double terme() {
        double v = unaire();
        for (;;) {
            if (prendre('*'))
                v = appliquer('*', v, unaire());
            else if (prendre('/'))
                v = appliquer('/', v, unaire());
            else
                return v;
        }
    }
    double unaire() {
        // Limiter la récursion : une expression hostile ne doit pas épuiser la pile.
        if (profondeur_ >= 128)
            throw std::invalid_argument("Expression trop imbriquee");
        ++profondeur_;
        struct Garde {
            unsigned &n;
            ~Garde() {
                --n;
            }
        } garde{profondeur_};
        if (prendre('+'))
            return unaire();
        if (prendre('-'))
            return -unaire();
        return puissance();
    }
    double puissance() {
        double base = primaire();
        // Récursion à droite : 2^3^2 = 2^(3^2). L'exposant accepte un signe.
        if (prendre('^'))
            return appliquer('^', base, unaire());
        return base;
    }
    double primaire() {
        espaces();
        if (prendre('(')) {
            double v = expression();
            if (!prendre(')'))
                throw std::invalid_argument("')' attendue");
            return v;
        }
        if (texte_.substr(pos_, 4) == "sqrt") {
            pos_ += 4;
            if (!prendre('('))
                throw std::invalid_argument("'(' apres sqrt attendue");
            double v = expression();
            if (!prendre(')'))
                throw std::invalid_argument("')' attendue");
            if (v < 0)
                throw std::domain_error("sqrt negatif");
            return std::sqrt(v);
        }
        espaces();
        double v = 0;
        const char *debut = texte_.data() + pos_;
        const char *fin = texte_.data() + texte_.size();
        auto [ptr, err] = std::from_chars(debut, fin, v);
        if (err != std::errc{} || ptr == debut || !std::isfinite(v))
            throw std::invalid_argument("Nombre invalide position " + std::to_string(pos_));
        pos_ = static_cast<std::size_t>(ptr - texte_.data());
        return v;
    }

  public:
    double evaluer(std::string_view texte) {
        if (texte.empty() || texte.size() > 10000)
            throw std::invalid_argument("Taille expression invalide");
        texte_ = texte;
        pos_ = 0;
        profondeur_ = 0;
        double v = expression();
        espaces();
        if (pos_ != texte_.size())
            throw std::invalid_argument("Texte superflu");
        return v;
    }
};
int main(int argc, char **argv) {
    try {
        Calculateur c;
        CHECK(c.evaluer("2 + 3 * 4 - 1") == 13);
        CHECK(c.evaluer("(2+3)*4") == 20);
        CHECK(c.evaluer("2^3^2") == 512);
        CHECK(c.evaluer("-2^2") == -4);
        CHECK(c.evaluer("2^-2") == 0.25);
        CHECK(c.evaluer("sqrt(16)+1") == 5);
        CHECK(c.evaluer("1e2 / 4") == 25);
        for (const auto &s : {"1/0", "sqrt(-1)", "1 +", "(2", "2 3", "2**3", "1e999"})
            doitLever<std::exception>([&] { c.evaluer(s); });
        doitLever<std::invalid_argument>([&] { c.evaluer(std::string(200, '-') + "1"); });
        std::cout << c.evaluer(argc > 1 ? argv[1] : "2 + 3 * 4 - 1") << '\n';
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
