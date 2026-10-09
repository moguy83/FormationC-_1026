#include <iostream>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    auto pipeline = vec 
        | std::views::transform([](int n) { 
            std::cout << "Transforming: " << n << std::endl;   
            return n * n;
        });  

        std::cout << "Pipeline construit\n";

        for (int n : pipeline) {
            std::cout << "Résultat: " << n << std::endl;
        }
}

/*
Pipeline construit
Transforming: 1
Résultat: 1
Transforming: 2
Résultat: 4
Transforming: 3
Résultat: 9
Transforming: 4
Résultat: 16
Transforming: 5
Résultat: 25    
*/