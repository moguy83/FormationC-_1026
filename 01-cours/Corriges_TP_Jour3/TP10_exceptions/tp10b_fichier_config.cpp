#include "fichier_config.hpp"
#include "test.hpp"
#include <filesystem>
#include <iostream>
// Les tests utilisent un répertoire de travail CTest propre à cet exécutable.
void ecrire(const std::string& texte) {std::ofstream("config_test.ini")<<texte;}
int main() {
    FichierConfig c;
    // 01 : définir/lire. 02 : remplacement. 03 : clé absente.
    c.set("host","localhost"); CHECK(c.get("host")=="localhost");
    c.set("host","127.0.0.1"); CHECK(c.get("host")=="127.0.0.1");
    doitLever<KeyNotFound>([&]{c.get("inconnue");});
    // 04 : fichier absent + état conservé.
    doitLever<FileNotFound>([&]{c.charger("repertoire_absent/fichier.ini");});
    CHECK(c.get("host")=="127.0.0.1");
    // 05 : commentaire, espaces, CRLF. 06 : valeur vide. 07 : '=' dans valeur.
    ecrire("# commentaire\r\n\r\n host = serveur \r\n"); c.charger("config_test.ini");
    CHECK(c.get("host")=="serveur");
    ecrire("vide=\n"); c.charger("config_test.ini"); CHECK(c.get("vide").empty());
    ecrire("token=a=b\n"); c.charger("config_test.ini"); CHECK(c.get("token")=="a=b");
    // 08 : séparateur absent après une ligne valide, garantie forte.
    ecrire("nouveau=ok\ninvalide\n");
    doitLever<ParseError>([&]{c.charger("config_test.ini");}); CHECK(c.get("token")=="a=b");
    doitLever<KeyNotFound>([&]{c.get("nouveau");});
    // 09 : clé vide. 10 : doublon.
    ecrire("=val\n"); doitLever<ParseError>([&]{c.charger("config_test.ini");});
    ecrire("a=1\na=2\n"); doitLever<ParseError>([&]{c.charger("config_test.ini");});
    // 11 : un fichier vide remplace correctement l'ancienne configuration.
    ecrire(""); c.charger("config_test.ini"); doitLever<KeyNotFound>([&]{c.get("token");});
    // 12 : set refuse une clé invalide. 13 : set refuse un retour à la ligne.
    doitLever<ParseError>([&]{c.set("a=b","x");});
    doitLever<ParseError>([&]{c.set("a","x\ny");});
    // 14 : sauvegarde/rechargement. 15 : échec de sauvegarde déterministe (parent absent).
    c.set("nom","Lina Martin"); c.sauvegarder("config_test.ini");
    FichierConfig copie; copie.charger("config_test.ini"); CHECK(copie.get("nom")=="Lina Martin");
    doitLever<WriteError>([&]{c.sauvegarder("repertoire_absent/config.ini");});
    std::cout<<"TP10B : 15 scenarios valides\n";
}
