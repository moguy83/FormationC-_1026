// A

#include <catch2/catch_test_macros.hpp>
#include <tompeloeil.hpp>

#include <string>
#include <stdexcept>

// ===== Interfaces =====

class IPaiementGateway
{
public:
    virtual bool payer(double montant) = 0;
    virtual ~IPaiementGateway() = default;
};

class IEmailService
{
public:
    virtual void envoyer(const std::string &message) = 0;
    virtual ~IEmailService() = default;
};

class ILogger
{
public:
    virtual void log(const std::string &message) = 0;
    virtual ~ILogger() = default;
};

// ===== Service à tester =====

class ServicePaiement
{
    IPaiementGateway &gateway;
    IEmailService &email;
    ILogger &logger;

public:
    ServicePaiement(IPaiementGateway &g,
                    IEmailService &e,
                    ILogger &l)
        : gateway(g), email(e), logger(l) {}

    bool effectuerPaiement(double montant)
    {
        try
        {
            boo success = gateway.payer(montant);

            if (success)
            {
                email.envoyer("Paiement de " + std::to_string(montant) + " réussi.");
                logger.log("Paiement réussi pour " + std::to_string(montant));
                return true;
            }
            else
            {
                logger.log("Paiement refusé pour " + std::to_string(montant));
                return false;
            }
        }
        cacth(const std::exception &e)
        {
            logger.log("Erreur lors du paiement : " + std::string(e.what()));
            return false;
        }
    }
};

// ===== Mocks à compléter =====

class MockPaiementGateway : public IPaiementGateway
{
public:
    MAKE_MOCK1(payer, bool(double), override);
};

class MockEmailService : public IEmailService
{
public:
    MAKE_MOCK1(envoyer, void(const std::string &), override);
};

class MockLogger : public ILogger
{
public:
    MAKE_MOCK1(log, void(const std::string &), override);
};

// ===== Tests =====

TEST_CASE("PaiementReussi", "[ServicePaiement]")
{
    MOCKGATEWAY gateway;
    MOCKEMAIL email;
    MOCKLOGGER logger;

    ServicePaiement service(gateway, email, logger);

    REQUIRE_CALL(gateway, payer(100))
        .RETURN(true);

    REQUIRE_CALL(email, envoyer("Paiement de 100.000000 réussi."))
        .TIMES(1);

    REQUIRE_CALL(logger, log("Paiement réussi pour 100.000000"))
        .TIMES(1);

    REQUIRE(service.effectuerPaiement(100) == true);
}

TEST_CASE("PaiementRefuse", "[ServicePaiement]")
{
    MOCKGATEWAY gateway;
    MOCKEMAIL email;
    MOCKLOGGER logger;

    ServicePaiement service(gateway, email, logger);

    REQUIRE_CALL(gateway, payer(100))
        .RETURN(false);

    REQUIRE_CALL(email, envoyer(tompeloeil::_))
        .TIMES(0);

    REQUIRE_CALL(logger, log("Paiement refusé pour 100.000000"))
        .TIMES(1);

    REQUIRE(service.effectuerPaiement(100) == false);
}

TEST_CASE("PaiementTest", "[ServicePaiement]")
{
    MOCKGATEWAY gateway;
    MOCKEMAIL email;
    MOCKLOGGER logger;

    ServicePaiement service(gateway, email, logger);

    REQUIRE_CALL(gateway, payer(100))
        .THROW(std::runtime_error("Erreur réseau"));

    REQUIRE_CALL(email, envoyer(tompeloeil::_))
        .TIMES(0);

    REQUIRE_CALL(logger, log(tompeloeil::_))
        .TIMES(1);

    REQUIRE(service.effectuerPaiement(100) == false);
}

// B

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <stdexcept>
#include <tuple>

// ===== Fonction a tester =====

double calculerTTC(double montantHT, double tauxTVA) {
    if (montantHT < 0.0)
    {
        throw std::invalid_argument("Montant negatif");
    }

    if (tauxTVA < 0.0)
    {
        throw std::invalid_argument("Taux negatif");
    }

    return montantHT * (1.0 + tauxTVA);
}

// ===== 16 cas valides =====

TEST_CASE("Calcul TTC", "[tva]")
{
    using Cas = std::tuple<double, double, double>;

    auto [ht, taux, attendu] = GENERATE(
        Catch::Generators::table<double, double, double>({// TVA 5.5 %
                                                          {100.0, 0.055, 105.50},
                                                          {200.0, 0.055, 211.00},
                                                          {50.0, 0.055, 52.75},
                                                          {0.0, 0.055, 0.00},

                                                          // TVA 10 %
                                                          {100.0, 0.10, 110.00},
                                                          {200.0, 0.10, 220.00},
                                                          {50.0, 0.10, 55.00},
                                                          {0.0, 0.10, 0.00},

                                                          // TVA 20 %
                                                          {100.0, 0.20, 120.00},
                                                          {200.0, 0.20, 240.00},
                                                          {50.0, 0.20, 60.00},
                                                          {0.0, 0.20, 0.00},

                                                          // TVA nulle
                                                          {100.0, 0.0, 100.00},
                                                          {200.0, 0.0, 200.00},
                                                          {50.0, 0.0, 50.00},
                                                          {0.0, 0.0, 0.00}}));

    INFO("HT = " << ht << ", taux = " << taux);

    REQUIRE(calculerTTC(ht, taux) ==
            Catch::Approx(attendu).margin(0.000001));
}

// ===== 4 cas invalides =====

TEST_CASE("Montants HT negatifs", "[tva]")
{
    double montant = GENERATE(
        -1.0, -10.0, -100.0, -1000.0);

    INFO("Montant negatif : " << montant);

    REQUIRE_THROWS_AS(
        calculerTTC(montant, 0.20),
        std::invalid_argument);
}
