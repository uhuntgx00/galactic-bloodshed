// Copyright 2014 The Galactic Bloodshed Authors. All rights reserved.
// Use of this source code is governed by a license that can be
// found in the COPYING file.

#ifndef MAKESTAR_H
#define MAKESTAR_H

#include <stdio.h>

#include "vars.h"

int Temperature(double dist, int stemp);
void Makestar_init(void);
startype *Makestar(FILE *, FILE *);
void Makeplanet_init(void);

#endif // MAKESTAR_H