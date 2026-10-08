
#include <catch2/catch_test_macros.hpp>
#include <trompeloeil.hpp>

#include <string>

// ===== Interfaces =====

class ILecteurBadge {
public:
    virtual bool estAutorise(int badgeId) = 0;
    virtual ~ILecteurBadge() = default;
};

class IMoteurPorte {
public:
    virtual void ouvrir() = 0;
    virtual ~IMoteurPorte() = default;
};

// ===== Classe metier =====

class ControleurPorte {
    ILecteurBadge& lecteur;
    IMoteurPorte& moteur;

public:
    ControleurPorte(ILecteurBadge& l, IMoteurPorte& m)
        : lecteur(l), moteur(m) {}

    bool passerBadge(int badgeId) {
        if (!lecteur.estAutorise(badgeId)) {
            return false;
        }

        moteur.ouvrir();
        return true;
    }
};

// ===== Mocks Trompeloeil =====

class MockLecteurBadge : public ILecteurBadge {
public:
    MAKE_MOCK1(estAutorise, bool(int), override);
};

class MockMoteurPorte : public IMoteurPorte {
public:
    MAKE_MOCK0(ouvrir, void(), override);
};

// ===== Test : badge autorise =====

TEST_CASE("Un badge valide ouvre la porte") {
    MockLecteurBadge lecteur;
    MockMoteurPorte moteur;

    ControleurPorte controleur(lecteur, moteur);

    REQUIRE_CALL(lecteur, estAutorise(1234))
        .RETURN(true);

    REQUIRE_CALL(moteur, ouvrir());

    REQUIRE(controleur.passerBadge(1234));
}

// ===== Test : badge refuse =====

TEST_CASE("Un badge invalide ne doit pas ouvrir") {
    MockLecteurBadge lecteur;
    MockMoteurPorte moteur;

    ControleurPorte controleur(lecteur, moteur);

    REQUIRE_CALL(lecteur, estAutorise(9999))
        .RETURN(false);

    FORBID_CALL(moteur, ouvrir());

    REQUIRE_FALSE(controleur.passerBadge(9999));
}
