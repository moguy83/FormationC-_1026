#include <vector>
#include <algorithm>
#include <functional>

using Strategie = std::function<int(int)>;

void traiter(
    std::vector<int>& v,
    const Strategie& strategie
) {
    std:transform(
        v.begin(),
        v.end(),
        v.begin(),
        strategie
    );
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    Strategie multiplieParDeux = [](int x) { return x * 2; };
    Strategie ajouterDix = [](int x) { return x + 10; };
    
    traiter(v, multiplieParDeux);

    for (int x : v) {
        std::cout << x << " ";
    
    }

    traiter(v, ajouterDix);

    for (int x : v) {
        std::cout << x << " ";
    
    }
    return 0;
}
