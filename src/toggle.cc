// Copyright 2014 The Galactic Bloodshed Authors. All rights reserved.
// Use of this source code is governed by a license that can be
// found in the COPYING file.

/*  toggle.c -- toggles some options */

#include "toggle.h"

#include <stdio.h>

#include "GB_server.h"
#include "buffers.h"
#include "files_shl.h"
#include "races.h"
#include "shlmisc.h"
#include "vars.h"

static void tog(int, int, char *, const char *);

void toggle(int Playernum, int Governor, int APcount) {
  racetype *Race;

  Race = races[Playernum - 1];

  if (argn > 1) {
    if (match(args[1], "inverse"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.inverse,
          "inverse");
    else if (match(args[1], "double_digits"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.double_digits,
          "double_digits");
    else if (match(args[1], "geography"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.geography,
          "geography");
    else if (match(args[1], "gag"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.gag, "gag");
    else if (match(args[1], "autoload"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.autoload,
          "autoload");
    else if (match(args[1], "color"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.color, "color");
    else if (match(args[1], "visible"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.invisible,
          "invisible");
    else if (Race->God && match(args[1], "monitor"))
      tog(Playernum, Governor, &Race->monitor, "monitor");
    else if (match(args[1], "compatibility"))
      tog(Playernum, Governor, &Race->governor[Governor].toggle.compat,
          "compatibility");
    else {
      sprintf(buf, "No such option '%s'\n", args[1]);
      notify(Playernum, Governor, buf);
      return;
    }
    putrace(Race);
  } else {
    sprintf(buf, "gag is %s\n",
            Race->governor[Governor].toggle.gag ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "inverse is %s\n",
            Race->governor[Governor].toggle.inverse ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "double_digits is %s\n",
            Race->governor[Governor].toggle.double_digits ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "geography is %s\n",
            Race->governor[Governor].toggle.geography ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "autoload is %s\n",
            Race->governor[Governor].toggle.autoload ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "color is %s\n",
            Race->governor[Governor].toggle.color ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "compatibility is %s\n",
            Race->governor[Governor].toggle.compat ? "ON" : "OFF");
    notify(Playernum, Governor, buf);
    sprintf(buf, "%s\n", Race->governor[Governor].toggle.invisible ? "INVISIBLE"
                                                                   : "VISIBLE");
    notify(Playernum, Governor, buf);
    sprintf(buf, "highlight player %d\n",
            Race->governor[Governor].toggle.highlight);
    notify(Playernum, Governor, buf);
    if (Race->God) {
      sprintf(buf, "monitor is %s\n", Race->monitor ? "ON" : "OFF");
      notify(Playernum, Governor, buf);
    }
  }
}

void highlight(int Playernum, int Governor, int APcount) {
  player_t n;
  racetype *Race;

  if (!(n = GetPlayer(args[1]))) {
    sprintf(buf, "No such player.\n");
    notify(Playernum, Governor, buf);
    return;
  }
  Race = races[Playernum - 1];
  Race->governor[Governor].toggle.highlight = n;
  putrace(Race);
}

static void tog(int Playernum, int Governor, char *op, const char *name) {
  *op = !(*op);
  sprintf(buf, "%s is now %s\n", name, *op ? "on" : "off");
  notify(Playernum, Governor, buf);
}
