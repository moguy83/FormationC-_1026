#include "test.hpp"
#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
class EventBus {
    using Handler = std::function<void(const std::any &)>;
    // Partager les callbacks préserve aussi l'état des closures mutable entre publications.
    std::map<std::string, std::vector<std::shared_ptr<Handler>>> handlers_;

  public:
    void subscribe(const std::string &event, std::function<void(const std::any &)> handler) {
        if (!handler)
            throw std::invalid_argument("Callback vide");
        handlers_[event].push_back(std::make_shared<Handler>(std::move(handler)));
    }
    void publish(const std::string &event, const std::any &data) {
        auto it = handlers_.find(event);
        if (it == handlers_.end())
            return;
        // Instantané : un callback peut abonner un autre callback sans invalider la boucle.
        // Le nouvel abonné sera appelé à la PROCHAINE publication.
        auto copie = it->second;
        for (const auto &handler : copie)
            (*handler)(data);
        // Contrat : la première exception se propage et interrompt la publication.
        // Ce bus est synchrone et mono-thread ; aucune promesse de sûreté multithread.
    }
};
int main() {
    EventBus bus;
    int total = 0, appels = 0;
    // Capture par référence sûre car total survit au bus. Éviter de capturer un local détruit.
    bus.subscribe("vente",
                  [&total](const std::any &payload) { total += std::any_cast<int>(payload); });
    bus.subscribe("vente", [&appels](const std::any &) { ++appels; });
    bus.publish("vente", 10);
    CHECK(total == 10);
    CHECK(appels == 1);
    bus.publish("inconnu", 0);
    CHECK(appels == 1);
    doitLever<std::bad_any_cast>([&] { bus.publish("vente", std::string("incorrect")); });
    CHECK(appels == 1); // deuxième callback non appelé après l'exception du premier.
    int tardif = 0;
    bus.subscribe("ajout", [&](const std::any &) {
        bus.subscribe("ajout", [&](const std::any &) { ++tardif; });
    });
    bus.publish("ajout", 0);
    CHECK(tardif == 0);
    bus.publish("ajout", 0);
    CHECK(tardif == 1);
    doitLever<std::invalid_argument>([&] { bus.subscribe("vide", {}); });
    int etatInterne = 0;
    bus.subscribe("etat", [n = 0, &etatInterne](const std::any &) mutable { etatInterne = ++n; });
    bus.publish("etat", 0);
    bus.publish("etat", 0);
    CHECK(etatInterne == 2);
    std::cout << "TP13C : callbacks, any, reentrance et erreurs verifies\n";
}
