#include "structures.h"

// Functie de alocare a unei cozii
AC InitC(size_t dime)
{
    AC coada = (AC)malloc(sizeof(TCoada));
    if (!coada) {
        return NULL;
    }

    coada->dime = dime;
    coada->ic = coada->sc = NULL;

    return coada;
}

// Functie care verifica daca o coada este vida
int VidaC(AC coada)
{
    if (!coada->ic && !coada->sc) {
        return 1;
    }
    return 0;
}

// Functie care extrage un element din coada, in cazul in care
// aceasta nu este vida
TLG ExtrC(AC coada)
{
    if (!VidaC(coada)) {
        TLG aux = coada->ic;
        coada->ic = coada->ic->urm;
        aux->urm = NULL;
        if (coada->ic == NULL) {
            coada->sc = NULL;
        }

        return aux;
    }
    return NULL;
}

// Functie de inserare a unui element in coada
void InsrC(AC coada, void *el)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        return ;
    }

    coada->sc->urm = el;
    coada->sc = el;
}

// Functie de inserare in coada dupa criteriile mentionate in enuntul temei
void InsrOrdonata(AC coada, void *el)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        return;
    }
    if (comp(el, coada->sc)) {
        InsrC(coada, el);
        return;
    }
    AC aux = InitC(sizeof(TNod));
    void *element = ExtrC(coada);
    do
    {
        if (!comp(el, element)) {
            break;
        }
        InsrC(aux, element);
        element = ExtrC(coada);
    } while (!VidaC(coada));
    InsrC(aux, el);
    InsrC(aux, element);
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        InsrC(aux, element);
    }
    coada->ic = aux->ic;
    coada->sc = aux->sc;
    free(aux);
}

// Functie de inserare ordonata in coada, in care coada times copieaza
// aceleasi miscari precum coada initiala
void InsrOrdonataRun(AC coada, AC times, void *el, void *time)
{
    if (coada->ic == NULL) {
        coada->ic = coada->sc = el;
        times->ic = times->sc = time;
        return;
    }
    if (comp(el, coada->sc)) {
        InsrC(coada, el);
        InsrC(times, time);
        return;
    }
    AC aux = InitC(sizeof(TNod));
    AC time_aux = InitC(sizeof(TNod));
    void *element = ExtrC(coada);
    void *time_element = ExtrC(times);
    do
    {
        if (!comp(el, element)) {
            break;
        }
        InsrC(aux, element);
        InsrC(time_aux, time_element);
        element = ExtrC(coada);
        time_element = ExtrC(times);
    } while (!VidaC(coada));
    InsrC(aux, el);
    InsrC(aux, element);
    InsrC(time_aux, time);
    InsrC(time_aux, time_element);
    while (!VidaC(coada)) {
        element = ExtrC(coada);
        time_element = ExtrC(times);
        InsrC(aux, element);
        InsrC(time_aux, time_element);
    }
    coada->ic = aux->ic;
    coada->sc = aux->sc;
    times->ic = time_aux->ic;
    times->sc = time_aux->sc;
    free(aux);
    free(time_aux);
}

// Functie de eliberare a memoriei pentru coada
void FreeC(AC *coada, void(*Free)(TLG))
{
    Free((*coada)->ic);
    free(*coada);
    *coada = NULL;
}

