#include "statistiques.hpp"
#include "test.hpp"
#include <nlohmann/json.hpp>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>
void analyser(const std::string &path) {
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("Log inaccessible");
    std::vector<double> durees;
    std::string ligne;
    std::size_t numero = 0;
    while (std::getline(in, ligne)) {
        ++numero;
        auto j = nlohmann::json::parse(ligne);
        if (!j.is_object() || !j.contains("duree_ms") || !j.at("duree_ms").is_number())
            throw std::runtime_error("Log invalide ligne " + std::to_string(numero));
        double ms = j.at("duree_ms").get<double>();
        if (!std::isfinite(ms) || ms < 0)
            throw std::runtime_error("Duree invalide");
        durees.push_back(ms);
        if (ms > 100)
            std::cout << "ALERTE requete=" << j.at("id") << " duree=" << ms << " ms\n";
    }
    if (in.bad())
        throw std::runtime_error("Erreur lecture log");
    if (durees.empty()) {
        std::cout << "Aucune requete\n";
        return;
    }
    for (double p : {50, 95, 99})
        std::cout << 'P' << p << " = " << percentile(durees, p) << " ms\n";
}
int main(int argc, char **argv) {
    try {
        std::vector<double> serie(100);
        std::iota(serie.begin(), serie.end(), 1.0);
        CHECK(percentile(serie, 50) == 50);
        CHECK(percentile(serie, 95) == 95);
        CHECK(percentile(serie, 99) == 99);
        doitLever<std::invalid_argument>([] { percentile({}, 50); });
        // Mode analyse seul : aucun écrasement du fichier fourni.
        if (argc == 3 && std::string(argv[1]) == "--analyser") {
            analyser(argv[2]);
            return 0;
        }
        const std::string path = argc > 1 ? argv[1] : "requetes.jsonl";
        std::ofstream out(path);
        if (!out)
            throw std::runtime_error("Log inaccessible en ecriture");
        for (int id = 1; id <= 24; ++id) {
            auto debut = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(id % 10 == 0 ? 120 : 1 + id % 8));
            double ms =
                std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - debut)
                    .count();
            // Mesurer avec steady_clock ; l'heure murale UTC sert seulement à dater l'événement.
            auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
            nlohmann::json event = {{"event", "requete_terminee"},
                                    {"id", id},
                                    {"timestamp_epoch_ms", epoch},
                                    {"duree_ms", ms},
                                    {"statut", "ok"}};
            out << event.dump() << '\n';
            out.flush(); // Visibilité immédiate pour tail -f, avec coût d'I/O assumé.
            if (!out)
                throw std::runtime_error("Ecriture log echouee");
        }
        out.close();
        if (!out)
            throw std::runtime_error("Fermeture log echouee");
        analyser(path);
        // Le sleep simule une latence d'I/O ; sa durée réelle dépend de l'ordonnanceur.
        // 24 requêtes suffisent pour le TP, pas pour estimer précisément un P99 de production.
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
