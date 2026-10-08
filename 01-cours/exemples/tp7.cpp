
#include <iostream>
#include <vector>
#include <functional>

class CapteurTemperature {
    std::vector<std::function<void(double)>> abonnes;

public:
    void subscribe(std::function<void(double)> callback) {
        // TODO : enregistrer le callback
    }

    void mesurer(double temperature) {
        // TODO : si temperature > 80
        //        notifier tous les abonnes
    }
};

int main() {
    CapteurTemperature capteur;

    // TODO : abonnement email
    // TODO : abonnement SMS
    // TODO : abonnement dashboard

    capteur.mesurer(65);
    capteur.mesurer(85);
    capteur.mesurer(95);
}
