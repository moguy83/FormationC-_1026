// B

#include <iostream>
#include <vector>
#include <functional>

class CapteurTemperature {
    std::vector<std::function<void(double)>> abonnes;

public:
    void subscribe(std::function<void(double)> callback) {
        abonnes.push_back(callback);
    }

    void mesurer(double temperature) {
        std::cout << "Mesure de la température : " << temperature << "°C\n";

        if (temperature > 80.0)
        {
            for (const auto &callback : abonnes)
            {
                callback(temperature);
            }
        }
    }
};

class AlertEmail
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte Email : Température critique de " << temperature << "°C\n";
    }
};

class AlertSMS
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte SMS : Température critique de " << temperature << "°C\n";
    }
};

class AlertDashboard
{
public:
    void notifier(double temperature)
    {
        std::cout << "Alerte Dashboard : Température critique de " << temperature << "°C\n";
    }
};

int main() {
    CapteurTemperature capteur;

    AlertEmail email;
    AlertSMS sms;
    AlertDashboard dashboard;

    capteur.subscribe([&email](double temperature)
                      { email.notifier(temperature); });

    capteur.subscribe([&sms](double temperature)
                      { sms.notifier(temperature); });

    capteur.subscribe([&dashboard](double temperature)
                      { dashboard.notifier(temperature); });

    capteur.mesurer(65);
    capteur.mesurer(85);
    capteur.mesurer(95);
}
