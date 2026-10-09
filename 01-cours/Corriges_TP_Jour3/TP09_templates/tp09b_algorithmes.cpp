#include "test.hpp"
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <type_traits>
#include <vector>
template<class Container, class T>
bool contient(const Container& c, const T& val) {
    for (const auto& e:c) if(e==val) return true;
    return false;
}
template<class Container, class Fn>
auto transformer(const Container& c, Fn fn) {
    // Déduire le résultat à la compilation, sans lire un premier élément : c peut être vide.
    using U=std::decay_t<std::invoke_result_t<Fn,const typename Container::value_type&>>;
    std::vector<U> resultat;
    for(const auto& e:c) resultat.push_back(std::invoke(fn,e));
    return resultat;
}
template<class Container,class T>
T reduire(const Container& c,T init,std::function<T(T,T)> fn) {
    for(const auto& e:c) init=fn(init,e);
    return init; // Un conteneur vide renvoie l'élément initial.
}
int main() {
    const std::list<int> valeurs{1,2,3};
    CHECK(contient(valeurs,2)); CHECK(!contient(valeurs,9));
    auto textes=transformer(valeurs,[](int x){return std::to_string(x);});
    CHECK(textes==std::vector<std::string>({"1","2","3"}));
    // std::function dans la signature ne permet pas la déduction directement depuis une lambda.
    std::function<int(int,int)> addition=[](int a,int b){return a+b;};
    CHECK(reduire(valeurs,0,addition)==6);
    CHECK(reduire(std::vector<int>{},7,addition)==7);
    CHECK(transformer(std::vector<int>{},[](int x){return x*2;}).empty());
    std::cout<<"TP09B : contient / transformer / reduire OK\n";
}
