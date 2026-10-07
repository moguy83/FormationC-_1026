#include <iostream>
#include <stdexcept>

class Trace {
    public:
        Trace() { std::cout << "Constructing Trace object\n"; }
        ~Trace() { std::cout << "Destructing Trace object\n"; }
};

void test(){
    Trace t;

    std::cout << "Avant exception\n";

    throw std::runtime_error("An exception occurred");

    std::cout << "Après exception\n";
}

int main()
{
    try {
        test();
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << '\n';
    }

    return 0;
}