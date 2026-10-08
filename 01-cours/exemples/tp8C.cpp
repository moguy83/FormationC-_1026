#include <catch2/catch_test_macros.hpp>
#include "remise.hpp"

TEST_CASE("Remise sur un gros achat", "[remise]")
{
    REQUIRE(calculerPrixRemise(150, false) == 130);
}

TEST_CASE("Remise fidelite", "[remise]")
{
    REQUIRE(calculerPrixRemise(50, true) == 45);
}


TEST_CASE("Prix negatif refuse", "[remise]") {
    REQUIRE_THROWS_AS(
        calculerPrix(-5, false),
        std::invalid_argument
    );
}

TEST_CASE("Prix nul", "[remise]") {
    REQUIRE(calculerPrix(0, false) == 0);
}

TEST_CASE("Aucune remise", "[remise]") {
    REQUIRE(calculerPrix(50, false) == 50);
}
