#include "structures.h"
#pragma once

void FreeC(AC *coada, void(*Free)(TLG));
TLG ExtrC(AC coada);
void InsrC(AC coada, void *el);
int VidaC(AC coada);
AC InitC(size_t dime);
int InsrOrdonata(AC coada, void *el);
int InsrOrdonataRun(AC coada, AC times, void *el, void *time);