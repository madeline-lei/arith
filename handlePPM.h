#ifndef HANDLE_PPM_H
#define HANDLE_PPM_H

#include <stdio.h>
#include "pnm.h"

Pnm_ppm readInPPM(FILE *input);
Pnm_ppm trimImage(Pnm_ppm original);

#endif
