// Copyright 2014 The Galactic Bloodshed Authors. All rights reserved.
// Use of this source code is governed by a license that can be
// found in the COPYING file.

/* declare.c -- declare alliance, neutrality, war, the basic thing. */

#include "declare.h"

#include <stdio.h>

#include "GB_server.h"
#include "buffers.h"
#include "config.h"
#include "files.h"
#include "files_shl.h"
#include "races.h"
#include "rand.h"
#include "shlmisc.h"
#include "tele.h"
#include "tweakables.h"
#include "vars.h"

static void show_votes(int, int);

/* invite people to join your alliance block */
void invite(const command_t &argv, const player_t Playernum,
            const governor_t Governor) {
  bool mode = (argv[0] == "invite") ? true : false;

  int n;
  racetype *Race, *alien;

  if (Governor) {
    notify(Playernum, Governor, "Only leaders may invite.\n");
    return;
  }
  if (!(n = GetPlayer(argv[1]))) {
    sprintf(buf, "No such player.\n");
    notify(Playernum, Governor, buf);
    return;
  }
  if (n == Playernum) {
    notify(Playernum, Governor, "Not needed, you are the leader.\n");
    return;
  }
  Race = races[Playernum - 1];
  alien = races[n - 1];
  if (mode) {
    setbit(Blocks[Playernum - 1].invite, n);
    sprintf(buf, "%s [%d] has invited you to join %s\n", Race->name, Playernum,
            Blocks[Playernum - 1].name);
    warn_race(n, buf);
    sprintf(buf, "%s [%d] has been invited to join %s [%d]\n", alien->name, n,
            Blocks[Playernum - 1].name, Playernum);
    warn_race(Playernum, buf);
  } else {
    clrbit(Blocks[Playernum - 1].invite, n);
    sprintf(buf, "You have been blackballed from %s [%d]\n",
            Blocks[Playernum - 1].name, Playernum);
    warn_race(n, buf);
    sprintf(buf, "%s [%d] has been blackballed from %s [%d]\n", alien->name, n,
            Blocks[Playernum - 1].name, Playernum);
    warn_race(Playernum, buf);
  }
  post(buf, DECLARATION);

  Putblock(Blocks);
}

/* declare that you wish to be included in the alliance block */
void pledge(const command_t &argv, const player_t Playernum,
            const governor_t Governor) {
  bool mode = (argv[0] == "pledge") ? true : false;
  int n;
  racetype *Race;

  if (Governor) {
    notify(Playernum, Governor, "Only leaders may pledge.\n");
    return;
  }
  if (!(n = GetPlayer(argv[1]))) {
    sprintf(buf, "No such player.\n");
    notify(Playernum, Governor, buf);
    return;
  }
  if (n == Playernum) {
    notify(Playernum, Governor, "Not needed, you are the leader.\n");
    return;
  }
  Race = races[Playernum - 1];
  if (mode) {
    setbit(Blocks[n - 1].pledge, Playernum);
    sprintf(buf, "%s [%d] has pledged %s.\n", Race->name, Playernum,
            Blocks[n - 1].name);
    warn_race(n, buf);
    sprintf(buf, "You have pledged allegiance to %s.\n", Blocks[n - 1].name);
    warn_race(Playernum, buf);

    switch (int_rand(1, 20)) {
      case 1:
        sprintf(
            buf,
            "%s [%d] joins the band wagon and pledges allegiance to %s [%d]!\n",
            Race->name, Playernum, Blocks[n - 1].name, n);
        break;
      default:
        sprintf(buf, "%s [%d] pledges allegiance to %s [%d].\n", Race->name,
                Playernum, Blocks[n - 1].name, n);
        break;
    }
  } else {
    clrbit(Blocks[n - 1].pledge, Playernum);
    sprintf(buf, "%s [%d] has quit %s [%d].\n", Race->name, Playernum,
            Blocks[n - 1].name, n);
    warn_race(n, buf);
    sprintf(buf, "You have quit %s\n", Blocks[n - 1].name);
    warn_race(Playernum, buf);

    switch (int_rand(1, 20)) {
      case 1:
        sprintf(buf, "%s [%d] calls %s [%d] a bunch of geeks and QUITS!\n",
                Race->name, Playernum, Blocks[n - 1].name, n);
        break;
      default:
        sprintf(buf, "%s [%d] has QUIT %s [%d]!\n", Race->name, Playernum,
                Blocks[n - 1].name, n);
        break;
    }
  }

  post(buf, DECLARATION);

  compute_power_blocks();
  Putblock(Blocks);
}

void declare(const command_t &argv, const player_t Playernum,
             const governor_t Governor) {
  const int APcount = 1;
  int n, d_mod;
  racetype *Race, *alien;

  if (Governor) {
    notify(Playernum, Governor, "Only leaders may declare.\n");
    return;
  }

  if (!(n = GetPlayer(argv[1]))) {
    sprintf(buf, "No such player.\n");
    notify(Playernum, Governor, buf);
    return;
  }

  /* look in sdata for APs first */
  /* enufAPs would print something */
  if ((int)Sdata.AP[Playernum - 1] >= APcount) {
    deductAPs(Playernum, Governor, APcount, 0, 1);
    /* otherwise use current star */
  } else if ((Dir[Playernum - 1][Governor].level == LEVEL_STAR ||
              Dir[Playernum - 1][Governor].level == LEVEL_PLAN) &&
             enufAP(Playernum, Governor,
                    Stars[Dir[Playernum - 1][Governor].snum]->AP[Playernum - 1],
                    APcount)) {
    deductAPs(Playernum, Governor, APcount, Dir[Playernum - 1][Governor].snum,
              0);
  } else {
    sprintf(buf, "You don't have enough AP's (%d)\n", APcount);
    notify(Playernum, Governor, buf);
    return;
  }

  Race = races[Playernum - 1];
  alien = races[n - 1];

  switch (*argv[2].c_str()) {
    case 'a':
      setbit(Race->allied, n);
      clrbit(Race->atwar, n);
      if (success(5)) {
        sprintf(buf, "But would you want your sister to marry one?\n");
        notify(Playernum, Governor, buf);
      } else {
        sprintf(buf, "Good for you.\n");
        notify(Playernum, Governor, buf);
      }
      sprintf(buf, " Player #%d (%s) has declared an alliance with you!\n",
              Playernum, Race->name);
      warn_race(n, buf);
      sprintf(buf, "%s [%d] declares ALLIANCE with %s [%d].\n", Race->name,
              Playernum, alien->name, n);
      d_mod = 30;
      if (argv.size() > 3) d_mod = std::stoi(argv[3]);
      d_mod = std::max(d_mod, 30);
      break;
    case 'n':
      clrbit(Race->allied, n);
      clrbit(Race->atwar, n);
      sprintf(buf, "Done.\n");
      notify(Playernum, Governor, buf);

      sprintf(buf, " Player #%d (%s) has declared neutrality with you!\n",
              Playernum, Race->name);
      warn_race(n, buf);
      sprintf(buf, "%s [%d] declares a state of neutrality with %s [%d].\n",
              Race->name, Playernum, alien->name, n);
      d_mod = 30;
      break;
    case 'w':
      setbit(Race->atwar, n);
      clrbit(Race->allied, n);
      if (success(4)) {
        sprintf(buf,
                "Your enemies flaunt their secondary male reproductive "
                "glands in your\ngeneral direction.\n");
        notify(Playernum, Governor, buf);
      } else {
        sprintf(buf, "Give 'em hell!\n");
        notify(Playernum, Governor, buf);
      }
      sprintf(buf, " Player #%d (%s) has declared war against you!\n",
              Playernum, Race->name);
      warn_race(n, buf);
      switch (int_rand(1, 5)) {
        case 1:
          sprintf(buf, "%s [%d] declares WAR on %s [%d].\n", Race->name,
                  Playernum, alien->name, n);
          break;
        case 2:
          sprintf(buf, "%s [%d] has had enough of %s [%d] and declares WAR!\n",
                  Race->name, Playernum, alien->name, n);
          break;
        case 3:
          sprintf(
              buf,
              "%s [%d] decided that it is time to declare WAR on %s [%d]!\n",
              Race->name, Playernum, alien->name, n);
          break;
        case 4:
          sprintf(buf,
                  "%s [%d] had no choice but to declare WAR against %s [%d]!\n",
                  Race->name, Playernum, alien->name, n);
          break;
        case 5:
          sprintf(buf,
                  "%s [%d] says 'screw it!' and declares WAR on %s [%d]!\n",
                  Race->name, Playernum, alien->name, n);
          break;
        default:
          break;
      }
      d_mod = 30;
      break;
    default:
      notify(Playernum, Governor, "I don't understand.\n");
      return;
  }

  post(buf, DECLARATION);
  warn_race(Playernum, buf);

  /* They, of course, learn more about you */
  alien->translate[Playernum - 1] =
      MIN(alien->translate[Playernum - 1] + d_mod, 100);

  putrace(alien);
  putrace(Race);
}

#ifdef VOTING
void vote(const command_t &argv, const player_t Playernum,
          const governor_t Governor) {
  racetype *Race;
  int check, nvotes, nays, yays;

  Race = races[Playernum - 1];

  if (Race->God) {
    sprintf(buf, "Your vote doesn't count, however, here is the count.\n");
    notify(Playernum, Governor, buf);
    show_votes(Playernum, Governor);
    return;
  }
  if (Race->Guest) {
    sprintf(buf, "You are not allowed to vote, but, here is the count.\n");
    notify(Playernum, Governor, buf);
    show_votes(Playernum, Governor);
    return;
  }

  if (argv.size() > 2) {
    check = 0;
    if (argv[1] == "update") {
      if (argv[2] == "go") {
        Race->votes |= VOTE_UPDATE_GO;
        check = 1;
      } else if (argv[2] == "wait")
        Race->votes &= ~VOTE_UPDATE_GO;
      else {
        sprintf(buf, "No such update choice '%s'\n", argv[2].c_str());
        notify(Playernum, Governor, buf);
        return;
      }
    } else {
      sprintf(buf, "No such vote '%s'\n", argv[1].c_str());
      notify(Playernum, Governor, buf);
      return;
    }
    putrace(Race);

    if (check) {
      /* Ok...someone voted yes.  Tally them all up and see if */
      /* we should do something. */
      nays = 0;
      yays = 0;
      nvotes = 0;
      for (player_t pnum = 1; pnum <= Num_races; pnum++) {
        Race = races[pnum - 1];
        if (Race->God || Race->Guest) continue;
        nvotes++;
        if (Race->votes & VOTE_UPDATE_GO)
          yays++;
        else
          nays++;
      }
      /* Is Update/Movement vote unanimous now? */
      if (nvotes > 0 && nvotes == yays && nays == 0) {
        /* Do it... */
        do_next_thing();
      }
    }
  } else {
    sprintf(buf, "Your vote on updates is %s\n",
            (Race->votes & VOTE_UPDATE_GO) ? "go" : "wait");
    notify(Playernum, Governor, buf);
    show_votes(Playernum, Governor);
  }
}

static void show_votes(int Playernum, int Governor) {
  int nvotes, nays, yays, pnum;
  racetype *Race;

  nays = yays = nvotes = 0;
  for (pnum = 1; pnum <= Num_races; pnum++) {
    Race = races[pnum - 1];
    if (Race->God || Race->Guest) continue;
    nvotes++;
    if (Race->votes & VOTE_UPDATE_GO) {
      yays++;
      sprintf(buf, "  %s voted go.\n", Race->name);
    } else {
      nays++;
      sprintf(buf, "  %s voted wait.\n", Race->name);
    }
    if (races[Playernum - 1]->God) notify(Playernum, Governor, buf);
  }
  sprintf(buf, "  Total votes = %d, Go = %d, Wait = %d.\n", nvotes, yays, nays);
  notify(Playernum, Governor, buf);
}
#endif
