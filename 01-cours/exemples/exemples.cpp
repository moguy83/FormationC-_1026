void ajouterUn(int& valeur) {
    valeur++;
}

void ajouterUnPointeur(int* valeur) {
    (*valeur)++;
}
int n = 10;

ajouterUn(n);

ajouterUnPointeur(&n)