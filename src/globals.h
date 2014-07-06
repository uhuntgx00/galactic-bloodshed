// Copyright 2014 The Galactic Bloodshed Authors. All rights reserved.
// Use of this source code is governed by a license that can be
// found in the COPYING file.

// Collection of globals that must be initialized for every standalone
// program.

#ifndef GLOBALS_H
#define GLOBALS_H

#include "power.h"
#include "races.h"
#include "vars.h"

struct power Power[MAXPLAYERS];
struct block Blocks[MAXPLAYERS];
struct power_blocks Power_blocks;

racetype *races[MAXPLAYERS];

#endif // GLOBALS_H
