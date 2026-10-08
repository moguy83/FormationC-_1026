// A


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <stdexcept>

// ===== Interfaces =====

class IPaiementGateway {
public:
    virtual bool payer(double montant) = 0;
    virtual ~IPaiementGateway() = default;
};

class IEmailService {
public:
    virtual void envoyer(const std::string& message) = 0;
    virtual ~IEmailService() = default;
};

class ILogger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~ILogger() = default;
};

// ===== Service à tester =====

class ServicePaiement {
    IPaiementGateway& gateway;
    IEmailService& email;
    ILogger& logger;

public:
    ServicePaiement(IPaiementGateway& g,
                    IEmailService& e,
                    ILogger& l)
        : gateway(g), email(e), logger(l) {}

    bool effectuerPaiement(double montant) {
        // TODO :
        // 1. Appeler gateway.payer(montant)
        // 2. Si le paiement reussit :
        //    - Envoyer un email "Paiement confirme"
        //    - Logger "Paiement reussi"
        //    - Retourner true
        // 3. Si le paiement est refuse :
        //    - Logger "Paiement refuse"
        //    - Ne pas envoyer d'email
        //    - Retourner false
        // 4. Si la gateway lance une exception :
        //    - Logger "Erreur reseau"
        //    - Ne pas envoyer d'email
        //    - Retourner false

        return false;
    }
};

// ===== Mocks à compléter =====

class MockPaiementGateway : public IPaiementGateway {
public:
    // TODO : MOCK_METHOD pour payer
};

class MockEmailService : public IEmailService {
public:
    // TODO : MOCK_METHOD pour envoyer
};

class MockLogger : public ILogger {
public:
    // TODO : MOCK_METHOD pour log
};

// ===== Tests =====

TEST(PaiementTest, PaiementReussi) {
    // TODO :
    // Configurer payer(100) pour retourner true
    // Verifier l'envoi de l'email
    // Verifier le log de succes
    // Verifier que effectuerPaiement retourne true
}

TEST(PaiementTest, PaiementRefuse) {
    // TODO :
    // Configurer payer(100) pour retourner false
    // Verifier qu'aucun email n'est envoye
    // Verifier le log de refus
    // Verifier que effectuerPaiement retourne false
}

TEST(PaiementTest, ErreurReseau) {
    // TODO :
    // Configurer payer(100) pour lancer std::runtime_error
    // Verifier qu'aucun email n'est envoye
    // Verifier le log d'erreur
    // Verifier que effectuerPaiement retourne false
}

// B


#include <gtest/gtest.h>

#include <tuple>
#include <stdexcept>
#include <cmath>

// ===== Fonction à tester =====

double calculerTTC(double montantHT, double tauxTVA) {
    // TODO :
    // Si montantHT < 0 :
    //    lancer std::invalid_argument
    //
    // Sinon :
    //    TTC = montantHT * (1 + tauxTVA)

    return 0.0;
}

// ===== Tests parametres =====

// (montant HT, taux TVA, montant TTC attendu)
using CasTVA = std::tuple<double, double, double>;

class TestTVA : public ::testing::TestWithParam<CasTVA> {
};

// Test pour les montants valides
TEST_P(TestTVA, CalculTTC) {
    auto [ht, taux, attendu] = GetParam();

    // TODO :
    // Comparer calculerTTC(ht, taux) avec attendu
    // Utiliser EXPECT_NEAR pour les doubles
}

// Jeux de donnees
INSTANTIATE_TEST_SUITE_P(
    CalculsTVA,
    TestTVA,
    ::testing::Values(
        // TVA 5,5 %
        CasTVA{100.0, 0.055, 105.5},
        CasTVA{200.0, 0.055, 211.0},
        CasTVA{50.0,  0.055, 52.75},
        CasTVA{0.0,   0.055, 0.0},

        // TVA 10 %
        CasTVA{100.0, 0.10, 110.0},
        CasTVA{200.0, 0.10, 220.0},
        CasTVA{50.0,  0.10, 55.0},
        CasTVA{0.0,   0.10, 0.0},

        // TVA 20 %
        CasTVA{100.0, 0.20, 120.0},
        CasTVA{200.0, 0.20, 240.0},
        CasTVA{50.0,  0.20, 60.0},
        CasTVA{0.0,   0.20, 0.0},

        // TVA nulle
        CasTVA{100.0, 0.0, 100.0},
        CasTVA{200.0, 0.0, 200.0},
        CasTVA{50.0,  0.0, 50.0},
        CasTVA{0.0,   0.0, 0.0}
    )
);

// Cas d'erreur : montant negatif
TEST(TVATest, MontantNegatif) {
    // TODO :
    // Verifier qu'une exception std::invalid_argument
    // est declenchee avec -100 et 20 %
}
