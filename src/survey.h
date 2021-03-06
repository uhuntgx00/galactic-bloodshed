// Copyright 2014 The Galactic Bloodshed Authors. All rights reserved.
// Use of this source code is governed by a license that can be
// found in the COPYING file.

#ifndef SURVEY_H
#define SURVEY_H

#include "vars.h"

void survey(const command_t &argv, const player_t Playernum,
            const governor_t Governor);
void repair(const command_t &argv, const player_t Playernum,
            const governor_t Governor);

#endif  // SURVEY_H
