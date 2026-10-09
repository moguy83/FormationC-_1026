#include "date_iso.hpp"
#include "test.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <charconv>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
class InputValidator {
    static bool domaine(std::string_view s) {
        if (s.empty() || s.size() > 253 || s.find('.') == std::string_view::npos)
            return false;
        std::size_t pos = 0;
        while (pos < s.size()) {
            auto fin = s.find('.', pos);
            if (fin == std::string_view::npos)
                fin = s.size();
            auto label = s.substr(pos, fin - pos);
            if (label.empty() || label.size() > 63 || label.front() == '-' || label.back() == '-')
                return false;
            for (char c : label)
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                      c == '-'))
                    return false;
            pos = fin + 1;
        }
        return s.back() != '.';
    }

  public:
    static bool email(const std::string &s) {
        // Politique pédagogique ASCII : pas toute la RFC (pas de local-part entre guillemets).
        if (s.size() > 254)
            return false;
        auto at = s.find('@');
        if (at == std::string::npos || at == 0 || at > 64 ||
            s.find('@', at + 1) != std::string::npos)
            return false;
        auto local = s.substr(0, at);
        if (local.front() == '.' || local.back() == '.' || local.find("..") != std::string::npos)
            return false;
        for (char c : local)
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                  std::string_view("._%+-").find(c) != std::string_view::npos))
                return false;
        return domaine(std::string_view(s).substr(at + 1));
    }
    static bool url(const std::string &s) {
        // Sous-ensemble assumé : http(s), nom DNS ASCII, port facultatif, pas d'identifiants.
        // Valider une forme d'URL ne protège PAS contre SSRF ; aucun accès réseau ici.
        if (s.size() > 2048)
            return false;
        for (unsigned char c : s)
            if (c <= 32 || c >= 127 || c == '\\' || c == '<' || c == '>' || c == '"')
                return false;
        std::size_t start = s.starts_with("https://") ? 8 : s.starts_with("http://") ? 7 : 0;
        if (start == 0)
            return false;
        auto end = s.find_first_of("/?#", start);
        if (end == std::string::npos)
            end = s.size();
        auto host = s.substr(start, end - start);
        auto colon = host.find(':');
        if (colon != std::string::npos) {
            auto port = host.substr(colon + 1);
            unsigned n = 0;
            auto [p, ec] = std::from_chars(port.data(), port.data() + port.size(), n);
            if (ec != std::errc{} || p != port.data() + port.size() || n == 0 || n > 65535)
                return false;
            host.resize(colon);
        }
        if (!domaine(host))
            return false;
        auto hex = [](char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        };
        for (std::size_t i = end; i < s.size(); ++i)
            if (s[i] == '%') {
                if (i + 2 >= s.size() || !hex(s[i + 1]) || !hex(s[i + 2]))
                    return false;
                i += 2;
            }
        return true;
    }
    static bool telephone(const std::string &s) {
        // Forme internationale E.164 simplifiée : + puis 8 à 15 chiffres, premier non nul.
        if (s.size() < 9 || s.size() > 16 || s[0] != '+' || s[1] < '1' || s[1] > '9')
            return false;
        return std::all_of(s.begin() + 2, s.end(), [](char c) { return c >= '0' && c <= '9'; });
    }
    static bool date(const std::string &s) {
        try {
            (void)dateISO(s);
            return true;
        } catch (const std::invalid_argument &) {
            return false;
        }
    }
    static std::string echapperHTML(std::string_view s) {
        // Encodage pour du texte HTML / un attribut entre guillemets, pas JS/CSS/URL.
        // Ce n'est pas un nettoyeur permettant de conserver certaines balises HTML.
        std::string r;
        for (char c : s)
            switch (c) {
            case '&':
                r += "&amp;";
                break;
            case '<':
                r += "&lt;";
                break;
            case '>':
                r += "&gt;";
                break;
            case '"':
                r += "&quot;";
                break;
            case '\'':
                r += "&#39;";
                break;
            default:
                r += c;
            }
        return r;
    }
    static bool jsonValide(const std::string &s) {
        if (s.size() > 65536)
            return false; // Borne d'entrée explicite, pas une garantie complète anti-DoS.
        return nlohmann::json::accept(s);
    }
    static bool utilisateurJSON(const std::string &s) {
        // Parsing valide != schéma métier valide. Exemple de schéma volontairement minimal.
        if (!jsonValide(s))
            return false;
        auto j = nlohmann::json::parse(s);
        return j.is_object() && j.contains("nom") && j["nom"].is_string() &&
               !j["nom"].get<std::string>().empty() && j.contains("email") &&
               j["email"].is_string() && email(j["email"].get<std::string>());
    }
};
int main() {
    CHECK(InputValidator::email("lina+tp@example.fr"));
    CHECK(!InputValidator::email("a..b@example.fr"));
    CHECK(!InputValidator::email("a@-example.fr"));
    CHECK(!InputValidator::email("a@ex..fr"));
    CHECK(InputValidator::url("https://example.fr:443/a%20b?q=1"));
    for (const auto &s :
         {"javascript:alert(1)", "https://u:p@example.fr", "https://example.fr:99999",
          "https://example.fr/%zz", "https://example.fr/ a"})
        CHECK(!InputValidator::url(s));
    CHECK(InputValidator::telephone("+33612345678"));
    CHECK(!InputValidator::telephone("0612345678"));
    CHECK(InputValidator::date("2024-02-29"));
    CHECK(!InputValidator::date("2026-02-29"));
    CHECK(!InputValidator::date("2026-13-01"));
    CHECK(InputValidator::echapperHTML("<&\"'>") == "&lt;&amp;&quot;&#39;&gt;");
    CHECK(InputValidator::jsonValide("{\"a\":1}"));
    CHECK(!InputValidator::jsonValide("{a:1}"));
    CHECK(!InputValidator::utilisateurJSON("[]"));
    CHECK(InputValidator::utilisateurJSON(R"({"nom":"Lina","email":"lina@example.fr"})"));
    std::cout << "TP14B : validation syntaxique, metier et encodage verifies\n";
}
