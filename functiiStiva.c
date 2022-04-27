#include "structures.h"

// Functie de initializare a unei stive
AS InitS(size_t dime)
{
    AS stiva = (AS)malloc(sizeof(TStiva));
    if (!stiva) {
        return NULL;
    }

    stiva->dime = dime;
    stiva->vf = NULL;

    return stiva;
}

// Functie de adaugare a unui element in stiva
void Push(AS s, void *el)
{
    if (!s->vf) {
        s->vf = el;
        return;
    }

    ((TLG)el)->urm = s->vf;
    s->vf = el;
}

// Functie de extragere a unui element din stiva, in cazul in care stiva nu e goala
void *Pop(AS s)
{
    if (s->vf) {
        TLG aux = s->vf;
        s->vf = aux->urm;
        aux->urm = NULL;
        return aux;
    }
    return NULL;
}

// Functie care verifica daca stiva este goala sau nu
int VidaS(AS stiva)
{
    if (!stiva->vf) {
        return 1;
    }
    return 0;
}

// Functie de eliberare a memoriei pentru stiva
void FreeS(AS *stiva)
{
    FreeTLG((*stiva)->vf);
    free(*stiva);
    *stiva = NULL;
}