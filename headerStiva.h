#include "structures.h"
#pragma once

AS InitS(size_t dime);
void Push(AS s, void *el);
void *Pop(AS s);
int VidaS(AS stiva);
void FreeS(AS *stiva);