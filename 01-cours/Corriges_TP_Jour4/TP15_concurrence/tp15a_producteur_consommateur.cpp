#include "test.hpp"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <thread>
#include <vector>
class File {
    std::queue<int> q_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> fermee_{false};

  public:
    void push(int n) {
        {
            std::lock_guard lock(mutex_);
            if (fermee_.load())
                throw std::logic_error("File fermee");
            q_.push(n);
        }
        cv_.notify_one(); // Réveiller après déverrouillage réduit la contention.
    }
    std::optional<int> pop() {
        std::unique_lock lock(mutex_);
        // wait relâche le mutex pendant l'attente puis le reprend ; prédicat contre réveils
        // parasites.
        cv_.wait(lock, [&] { return fermee_.load() || !q_.empty(); });
        if (q_.empty())
            return std::nullopt; // Fermeture ET vidage : aucun travail perdu.
        int n = q_.front();
        q_.pop();
        return n;
    }
    void fermer() {
        // Même mutex que le prédicat : empêche une notification perdue entre test et attente.
        {
            std::lock_guard lock(mutex_);
            fermee_.store(true);
        }
        cv_.notify_all();
    }
};
int main() {
    File file;
    constexpr int parProducteur = 1000;
    std::atomic<int> consommes{0};
    std::atomic<std::int64_t> sommeProduite{0}, sommeConsommee{0};
    std::mutex erreurMutex;
    std::exception_ptr erreur;
    auto echec = [&] {
        {
            std::lock_guard lock(erreurMutex);
            if (!erreur)
                erreur = std::current_exception();
        }
        file.fermer();
    };
    std::vector<std::jthread> consommateurs, producteurs;
    try {
        for (int i = 0; i < 3; ++i)
            consommateurs.emplace_back([&] {
                try {
                    while (auto n = file.pop()) {
                        // Convertir AVANT multiplication pour éviter le débordement d'un int.
                        const auto carre = std::int64_t{*n} * (*n);
                        sommeConsommee.fetch_add(carre);
                        consommes.fetch_add(1);
                    }
                } catch (...) {
                    echec();
                }
            });
        for (int i = 0; i < 2; ++i)
            producteurs.emplace_back([&, i] {
                try {
                    std::mt19937 rng(
                        42 + static_cast<unsigned>(i)); // Un générateur par thread, aucune race.
                    std::uniform_int_distribution<int> distribution(1, 100);
                    for (int j = 0; j < parProducteur; ++j) {
                        int n = distribution(rng);
                        file.push(n);
                        sommeProduite.fetch_add(std::int64_t{n} * n);
                    }
                } catch (...) {
                    echec();
                }
            });
        // Les producteurs terminent AVANT fermeture ; les consommateurs drainent ensuite la file.
        for (auto &t : producteurs)
            t.join();
        file.fermer();
        for (auto &t : consommateurs)
            t.join();
    } catch (...) {
        file.fermer(); // Même si créer un thread échoue, débloquer les consommateurs avant les
                       // joins RAII.
        throw;
    }
    if (erreur)
        std::rethrow_exception(erreur);
    CHECK(consommes == 2 * parProducteur);
    CHECK(sommeConsommee == sommeProduite);
    CHECK(!file.pop());
    doitLever<std::logic_error>([&] { file.push(1); });
    std::cout << consommes << " nombres traites ; somme des carres=" << sommeConsommee << '\n';
}
